/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   Copyright (C) Andrew Tridgell 2005
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/*
  a composite API for making a full SMB connection
*/

#include "includes.h"
#include "libcli/raw/libcliraw.h"
#include "libcli/composite/composite.h"
#include "libcli/smb_composite/smb_composite.h"
#include "lib/events/events.h"
#include "libcli/resolve/resolve.h"
#include "auth/credentials/credentials.h"
#include "librpc/gen_ndr/ndr_nbt.h"

/* the stages of this call */
enum connect_stage {CONNECT_RESOLVE, 
		    CONNECT_SOCKET, 
		    CONNECT_SESSION_REQUEST,
		    CONNECT_NEGPROT,
		    CONNECT_SESSION_SETUP,
		    CONNECT_SESSION_SETUP_ANON,
		    CONNECT_TCON};

struct connect_state {
	enum connect_stage stage;
	struct smbcli_socket *sock;
	struct smbcli_transport *transport;
	struct smbcli_session *session;
	struct smb_composite_connect *io;
	union smb_tcon *io_tcon;
	struct smb_composite_sesssetup *io_setup;
	struct smbcli_request *req;
	struct composite_context *creq;
};


static void request_handler(struct smbcli_request *);
static void composite_handler(struct composite_context *);

/*
  setup a negprot send 
*/
static NTSTATUS connect_send_negprot(struct composite_context *c, 
				     struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);

	state->req = smb_raw_negotiate_send(state->transport, lp_cli_maxprotocol());
	NT_STATUS_HAVE_NO_MEMORY(state->req);

	state->req->async.fn = request_handler;
	state->req->async.private = c;
	state->stage = CONNECT_NEGPROT;
	
	return NT_STATUS_OK;
}


/*
  a tree connect request has completed
*/
static NTSTATUS connect_tcon(struct composite_context *c, 
			     struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;

	status = smb_raw_tcon_recv(state->req, c, state->io_tcon);
	NT_STATUS_NOT_OK_RETURN(status);

	io->out.tree->tid = state->io_tcon->tconx.out.tid;
	if (state->io_tcon->tconx.out.dev_type) {
		io->out.tree->device = talloc_strdup(io->out.tree, 
						     state->io_tcon->tconx.out.dev_type);
	}
	if (state->io_tcon->tconx.out.fs_type) {
		io->out.tree->fs_type = talloc_strdup(io->out.tree, 
						      state->io_tcon->tconx.out.fs_type);
	}

	/* all done! */
	c->state = COMPOSITE_STATE_DONE;

	return NT_STATUS_OK;
}


/*
  a session setup request with anonymous fallback has completed
*/
static NTSTATUS connect_session_setup_anon(struct composite_context *c, 
					   struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;

	status = smb_composite_sesssetup_recv(state->creq);
	NT_STATUS_NOT_OK_RETURN(status);

	io->out.anonymous_fallback_done = True;
	
	state->session->vuid = state->io_setup->out.vuid;
	
	/* setup for a tconx */
	io->out.tree = smbcli_tree_init(state->session, state, True);
	NT_STATUS_HAVE_NO_MEMORY(io->out.tree);

	state->io_tcon = talloc(c, union smb_tcon);
	NT_STATUS_HAVE_NO_MEMORY(state->io_tcon);

	/* connect to a share using a tree connect */
	state->io_tcon->generic.level = RAW_TCON_TCONX;
	state->io_tcon->tconx.in.flags = 0;
	state->io_tcon->tconx.in.password = data_blob(NULL, 0);	
	
	state->io_tcon->tconx.in.path = talloc_asprintf(state->io_tcon, 
						 "\\\\%s\\%s", 
						 io->in.called_name, 
						 io->in.service);
	NT_STATUS_HAVE_NO_MEMORY(state->io_tcon->tconx.in.path);
	if (!io->in.service_type) {
		state->io_tcon->tconx.in.device = "?????";
	} else {
		state->io_tcon->tconx.in.device = io->in.service_type;
	}

	state->req = smb_raw_tcon_send(io->out.tree, state->io_tcon);
	NT_STATUS_HAVE_NO_MEMORY(state->req);
	if (state->req->state == SMBCLI_REQUEST_ERROR) {
		return state->req->status;
	}

	state->req->async.fn = request_handler;
	state->req->async.private = c;
	state->stage = CONNECT_TCON;

	return NT_STATUS_OK;
}

/*
  a session setup request has completed
*/
static NTSTATUS connect_session_setup(struct composite_context *c, 
				      struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;

	status = smb_composite_sesssetup_recv(state->creq);

	if (!NT_STATUS_IS_OK(status) &&
	    !cli_credentials_is_anonymous(state->io->in.credentials) &&
	    io->in.fallback_to_anonymous) {

		state->io_setup->in.credentials = cli_credentials_init(state);
		NT_STATUS_HAVE_NO_MEMORY(state->io_setup->in.credentials);
		cli_credentials_set_conf(state->io_setup->in.credentials);
		cli_credentials_set_anonymous(state->io_setup->in.credentials);

		/* If the preceding attempt was with extended security, we
		 * have been given a uid in the NTLMSSP_CHALLENGE reply. This
		 * would lead to an invalid uid in the anonymous fallback */
		state->session->vuid = 0;
		data_blob_free(&state->session->user_session_key);
		talloc_free(state->session->gensec);
		state->session->gensec = NULL;

		state->creq = smb_composite_sesssetup_send(state->session,
							   state->io_setup);
		NT_STATUS_HAVE_NO_MEMORY(state->creq);
		if (state->creq->state == COMPOSITE_STATE_ERROR) {
			return state->creq->status;
		}
		state->creq->async.fn = composite_handler;
		state->creq->async.private_data = c;
		state->stage = CONNECT_SESSION_SETUP_ANON;

		return NT_STATUS_OK;
	}

	NT_STATUS_NOT_OK_RETURN(status);
	
	state->session->vuid = state->io_setup->out.vuid;
	
	/* setup for a tconx */
	io->out.tree = smbcli_tree_init(state->session, state, True);
	NT_STATUS_HAVE_NO_MEMORY(io->out.tree);

	state->io_tcon = talloc(c, union smb_tcon);
	NT_STATUS_HAVE_NO_MEMORY(state->io_tcon);

	/* connect to a share using a tree connect */
	state->io_tcon->generic.level = RAW_TCON_TCONX;
	state->io_tcon->tconx.in.flags = 0;
	state->io_tcon->tconx.in.password = data_blob(NULL, 0);	
	
	state->io_tcon->tconx.in.path = talloc_asprintf(state->io_tcon, 
						 "\\\\%s\\%s", 
						 io->in.called_name, 
						 io->in.service);
	NT_STATUS_HAVE_NO_MEMORY(state->io_tcon->tconx.in.path);
	if (!io->in.service_type) {
		state->io_tcon->tconx.in.device = "?????";
	} else {
		state->io_tcon->tconx.in.device = io->in.service_type;
	}

	state->req = smb_raw_tcon_send(io->out.tree, state->io_tcon);
	NT_STATUS_HAVE_NO_MEMORY(state->req);
	if (state->req->state == SMBCLI_REQUEST_ERROR) {
		return state->req->status;
	}

	state->req->async.fn = request_handler;
	state->req->async.private = c;
	state->stage = CONNECT_TCON;

	return NT_STATUS_OK;
}

/*
  a negprot request has completed
*/
static NTSTATUS connect_negprot(struct composite_context *c, 
				struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;

	status = smb_raw_negotiate_recv(state->req);
	NT_STATUS_NOT_OK_RETURN(status);

	/* next step is a session setup */
	state->session = smbcli_session_init(state->transport, state, True);
	NT_STATUS_HAVE_NO_MEMORY(state->session);

	state->io_setup = talloc(c, struct smb_composite_sesssetup);
	NT_STATUS_HAVE_NO_MEMORY(state->io_setup);

	/* prepare a session setup to establish a security context */
	state->io_setup->in.sesskey      = state->transport->negotiate.sesskey;
	state->io_setup->in.capabilities = state->transport->negotiate.capabilities;
	state->io_setup->in.credentials  = io->in.credentials;
	state->io_setup->in.workgroup    = io->in.workgroup;

	state->creq = smb_composite_sesssetup_send(state->session, state->io_setup);
	NT_STATUS_HAVE_NO_MEMORY(state->creq);
	if (state->creq->state == COMPOSITE_STATE_ERROR) {
		return state->creq->status;
	}

	state->creq->async.fn = composite_handler;
	state->creq->async.private_data = c;
	state->stage = CONNECT_SESSION_SETUP;
	
	return NT_STATUS_OK;
}


/*
  a session request operation has completed
*/
static NTSTATUS connect_session_request(struct composite_context *c, 
					struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;

	status = smbcli_transport_connect_recv(state->req);
	NT_STATUS_NOT_OK_RETURN(status);

	/* next step is a negprot */
	return connect_send_negprot(c, io);
}

/*
  a socket connection operation has completed
*/
static NTSTATUS connect_socket(struct composite_context *c, 
			       struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;
	struct nbt_name calling, called;

	status = smbcli_sock_connect_recv(state->creq, state, &state->sock);
	NT_STATUS_NOT_OK_RETURN(status);

	/* the socket is up - we can initialise the smbcli transport layer */
	state->transport = smbcli_transport_init(state->sock, state, True);
	NT_STATUS_HAVE_NO_MEMORY(state->transport);

	if (is_ipaddress(state->sock->hostname) &&
	    (state->io->in.called_name != NULL)) {
		/* If connecting to an IP address, we might want the real name
		 * of the host for later kerberos. The called name is a better
		 * approximation */
		state->sock->hostname =
			talloc_strdup(state->sock, io->in.called_name);
		NT_STATUS_HAVE_NO_MEMORY(state->sock->hostname);
	}

	make_nbt_name_client(&calling, cli_credentials_get_workstation(io->in.credentials));

	nbt_choose_called_name(state, &called, io->in.called_name, NBT_NAME_SERVER);

	/* we have a connected socket - next step is a session
	   request, if needed. Port 445 doesn't need it, so it goes
	   straight to the negprot */
	if (state->sock->port == 445) {
		status = nbt_name_dup(state->transport, &called, 
				      &state->transport->called);
		NT_STATUS_NOT_OK_RETURN(status);
		return connect_send_negprot(c, io);
	}

	state->req = smbcli_transport_connect_send(state->transport, &calling, &called);
	NT_STATUS_HAVE_NO_MEMORY(state->req);

	state->req->async.fn = request_handler;
	state->req->async.private = c;
	state->stage = CONNECT_SESSION_REQUEST;

	return NT_STATUS_OK;
}


/*
  called when name resolution is finished
*/
static NTSTATUS connect_resolve(struct composite_context *c, 
				struct smb_composite_connect *io)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
	NTSTATUS status;
	const char *address;

	status = resolve_name_recv(state->creq, state, &address);
	NT_STATUS_NOT_OK_RETURN(status);

	state->creq = smbcli_sock_connect_send(state, address, io->in.port,
					       io->in.dest_host, c->event_ctx);
	NT_STATUS_HAVE_NO_MEMORY(state->creq);

	state->stage = CONNECT_SOCKET;
	state->creq->async.private_data = c;
	state->creq->async.fn = composite_handler;

	return NT_STATUS_OK;
}


/*
  handle and dispatch state transitions
*/
static void state_handler(struct composite_context *c)
{
	struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);

	switch (state->stage) {
	case CONNECT_RESOLVE:
		c->status = connect_resolve(c, state->io);
		break;
	case CONNECT_SOCKET:
		c->status = connect_socket(c, state->io);
		break;
	case CONNECT_SESSION_REQUEST:
		c->status = connect_session_request(c, state->io);
		break;
	case CONNECT_NEGPROT:
		c->status = connect_negprot(c, state->io);
		break;
	case CONNECT_SESSION_SETUP:
		c->status = connect_session_setup(c, state->io);
		break;
	case CONNECT_SESSION_SETUP_ANON:
		c->status = connect_session_setup_anon(c, state->io);
		break;
	case CONNECT_TCON:
		c->status = connect_tcon(c, state->io);
		break;
	}

	if (!NT_STATUS_IS_OK(c->status)) {
		c->state = COMPOSITE_STATE_ERROR;
	}

	if (c->state >= COMPOSITE_STATE_DONE &&
	    c->async.fn) {
		c->async.fn(c);
	}
}


/*
  handler for completion of a smbcli_request sub-request
*/
static void request_handler(struct smbcli_request *req)
{
	struct composite_context *c = talloc_get_type(req->async.private, 
						     struct composite_context);
	state_handler(c);
}

/*
  handler for completion of a smbcli_composite sub-request
*/
static void composite_handler(struct composite_context *creq)
{
	struct composite_context *c = talloc_get_type(creq->async.private_data, 
						     struct composite_context);
	state_handler(c);
}

/*
  a function to establish a smbcli_tree from scratch
*/
struct composite_context *smb_composite_connect_send(struct smb_composite_connect *io,
						     TALLOC_CTX *mem_ctx,
						     struct event_context *event_ctx)
{
	struct composite_context *c;
	struct connect_state *state;
	struct nbt_name name;

	c = talloc_zero(mem_ctx, struct composite_context);
	if (c == NULL) goto failed;

	state = talloc_zero(c, struct connect_state);
	if (state == NULL) goto failed;

	if (event_ctx == NULL) {
		event_ctx = event_context_init(mem_ctx);
	}

	state->io = io;

	c->state = COMPOSITE_STATE_IN_PROGRESS;
	c->event_ctx = talloc_reference(c, event_ctx);
	c->private_data = state;

	state->stage = CONNECT_RESOLVE;
	make_nbt_name_server(&name, io->in.dest_host);
	state->creq = resolve_name_send(&name, c->event_ctx, lp_name_resolve_order());

	if (state->creq == NULL) goto failed;
	state->creq->async.private_data = c;
	state->creq->async.fn = composite_handler;

	return c;
failed:
	talloc_free(c);
	return NULL;
}

/*
  recv half of async composite connect code
*/
NTSTATUS smb_composite_connect_recv(struct composite_context *c, TALLOC_CTX *mem_ctx)
{
	NTSTATUS status;

	status = composite_wait(c);

	if (NT_STATUS_IS_OK(status)) {
		struct connect_state *state = talloc_get_type(c->private_data, struct connect_state);
		talloc_steal(mem_ctx, state->io->out.tree);
	}

	talloc_free(c);
	return status;
}

/*
  sync version of smb_composite_connect 
*/
NTSTATUS smb_composite_connect(struct smb_composite_connect *io, TALLOC_CTX *mem_ctx,
			       struct event_context *ev)
{
	struct composite_context *c = smb_composite_connect_send(io, mem_ctx, ev);
	return smb_composite_connect_recv(c, mem_ctx);
}
