/*
 * SPDX-FileCopyrightText: Stefan Metzmacher 2004
 * SPDX-FileCopyrightText: Andrew Bartlett <abartlet@samba.org> 2004-2005
 * SPDX-FileCopyrightText: Andrew Tridgell 2003
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   Generic Authentication Interface

   Copyright (C) Andrew Tridgell 2003
   Copyright (C) Andrew Bartlett <abartlet@samba.org> 2004-2005
   Copyright (C) Stefan Metzmacher 2004
   
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

#include "includes.h"
#include "libcli/composite/composite.h"
#include "auth/gensec/gensec.h"
#include "librpc/rpc/dcerpc.h"

/*
  return the rpc syntax and transfer syntax given the pipe uuid and version
*/
static NTSTATUS dcerpc_init_syntaxes(const struct dcerpc_interface_table *table,
			      struct dcerpc_syntax_id *syntax,
			      struct dcerpc_syntax_id *transfer_syntax)
{
	syntax->uuid = table->syntax_id.uuid;
	syntax->if_version = table->syntax_id.if_version;

	*transfer_syntax = ndr_transfer_syntax;

	return NT_STATUS_OK;
}


/*
  Send request to do a non-authenticated dcerpc bind
*/
struct composite_context *dcerpc_bind_auth_none_send(TALLOC_CTX *mem_ctx,
						     struct dcerpc_pipe *p,
						     const struct dcerpc_interface_table *table)
{
	struct dcerpc_syntax_id syntax;
	struct dcerpc_syntax_id transfer_syntax;

	struct composite_context *c;

	c = composite_create(mem_ctx, p->conn->event_ctx);
	if (c == NULL) return NULL;

	c->status = dcerpc_init_syntaxes(table,
					 &syntax, &transfer_syntax);
	if (!NT_STATUS_IS_OK(c->status)) {
		DEBUG(2,("Invalid uuid string in "
			 "dcerpc_bind_auth_none_send\n"));
		composite_error(c, c->status);
		return c;
	}

	/* c was only allocated as a container for a possible error */
	talloc_free(c);

	return dcerpc_bind_send(p, mem_ctx, &syntax, &transfer_syntax);
}


/*
  Receive result of a non-authenticated dcerpc bind
*/
NTSTATUS dcerpc_bind_auth_none_recv(struct composite_context *ctx)
{
	return dcerpc_bind_recv(ctx);
}


/*
  Perform sync non-authenticated dcerpc bind
*/
NTSTATUS dcerpc_bind_auth_none(struct dcerpc_pipe *p,
			       const struct dcerpc_interface_table *table)
{
	struct composite_context *ctx;

	ctx = dcerpc_bind_auth_none_send(p, p, table);
	return dcerpc_bind_auth_none_recv(ctx);
}


struct bind_auth_state {
	struct dcerpc_pipe *pipe;
	DATA_BLOB credentials;
	BOOL more_processing;	/* Is there anything more to do after the
				 * first bind itself received? */
};

static void bind_auth_recv_alter(struct composite_context *creq);

static void bind_auth_next_step(struct composite_context *c)
{
	struct bind_auth_state *state;
	struct dcerpc_security *sec;
	struct composite_context *creq;
	BOOL more_processing = False;

	state = talloc_get_type(c->private_data, struct bind_auth_state);
	sec = &state->pipe->conn->security_state;

	/* The status value here, from GENSEC is vital to the security
	 * of the system.  Even if the other end accepts, if GENSEC
	 * claims 'MORE_PROCESSING_REQUIRED' then you must keep
	 * feeding it blobs, or else the remote host/attacker might
	 * avoid mutal authentication requirements.
	 *
	 * Likewise, you must not feed GENSEC too much (after the OK),
	 * it doesn't like that either
	 */

	c->status = gensec_update(sec->generic_state, state,
				  sec->auth_info->credentials,
				  &state->credentials);

	if (NT_STATUS_EQUAL(c->status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		more_processing = True;
		c->status = NT_STATUS_OK;
	}

	if (!composite_is_ok(c)) return;

	if (state->credentials.length == 0) {
		composite_done(c);
		return;
	}

	sec->auth_info->credentials = state->credentials;

	if (!more_processing) {
		/* NO reply expected, so just send it */
		c->status = dcerpc_auth3(state->pipe->conn, state);
		if (!composite_is_ok(c)) return;

		composite_done(c);
		return;
	}

	/* We are demanding a reply, so use a request that will get us one */

	creq = dcerpc_alter_context_send(state->pipe, state,
					 &state->pipe->syntax,
					 &state->pipe->transfer_syntax);
	if (composite_nomem(creq, c)) return;

	composite_continue(c, creq, bind_auth_recv_alter, c);
}


static void bind_auth_recv_alter(struct composite_context *creq)
{
	struct composite_context *c = talloc_get_type(creq->async.private_data,
						      struct composite_context);

	c->status = dcerpc_alter_context_recv(creq);
	if (!composite_is_ok(c)) return;

	bind_auth_next_step(c);
}


static void bind_auth_recv_bindreply(struct composite_context *creq)
{
	struct composite_context *c = talloc_get_type(creq->async.private_data,
						      struct composite_context);
	struct bind_auth_state *state =	talloc_get_type(c->private_data,
							struct bind_auth_state);

	c->status = dcerpc_bind_recv(creq);
	if (!composite_is_ok(c)) return;

	if (!state->more_processing) {
		/* The first gensec_update has not requested a second run, so
		 * we're done here. */
		composite_done(c);
		return;
	}

	bind_auth_next_step(c);
}


/**
   Bind to a DCE/RPC pipe, send async request
   @param mem_ctx TALLOC_CTX for the allocation of the composite_context
   @param p The dcerpc_pipe to bind (must already be connected)
   @param table The interface table to use (the DCE/RPC bind both selects and interface and authenticates)
   @param credentials The credentials of the account to connect with 
   @param auth_type Select the authentication scheme to use
   @param auth_level Chooses between unprotected (connect), signed or sealed
   @param service The service (used by Kerberos to select the service principal to contact)
   @retval A composite context describing the partial state of the bind
*/

struct composite_context *dcerpc_bind_auth_send(TALLOC_CTX *mem_ctx,
						struct dcerpc_pipe *p,
						const struct dcerpc_interface_table *table,
						struct cli_credentials *credentials,
						uint8_t auth_type, uint8_t auth_level,
						const char *service)
{
	struct composite_context *c, *creq;
	struct bind_auth_state *state;
	struct dcerpc_security *sec;

	struct dcerpc_syntax_id syntax, transfer_syntax;

	/* composite context allocation and setup */
	c = composite_create(mem_ctx, p->conn->event_ctx);
	if (c == NULL) return NULL;

	state = talloc(c, struct bind_auth_state);
	if (composite_nomem(state, c)) return c;
	c->private_data = state;

	state->pipe = p;

	c->status = dcerpc_init_syntaxes(table,
					 &syntax,
					 &transfer_syntax);
	if (!composite_is_ok(c)) return c;

	sec = &p->conn->security_state;

	c->status = gensec_client_start(p, &sec->generic_state,
					p->conn->event_ctx);
	if (!NT_STATUS_IS_OK(c->status)) {
		DEBUG(1, ("Failed to start GENSEC client mode: %s\n",
			  nt_errstr(c->status)));
		composite_error(c, c->status);
		return c;
	}

	c->status = gensec_set_credentials(sec->generic_state, credentials);
	if (!NT_STATUS_IS_OK(c->status)) {
		DEBUG(1, ("Failed to set GENSEC client credentails: %s\n",
			  nt_errstr(c->status)));
		composite_error(c, c->status);
		return c;
	}

	c->status = gensec_set_target_hostname(sec->generic_state,
					       p->conn->transport.target_hostname(p->conn));
	if (!NT_STATUS_IS_OK(c->status)) {
		DEBUG(1, ("Failed to set GENSEC target hostname: %s\n", 
			  nt_errstr(c->status)));
		composite_error(c, c->status);
		return c;
	}

	if (service != NULL) {
		c->status = gensec_set_target_service(sec->generic_state,
						      service);
		if (!NT_STATUS_IS_OK(c->status)) {
			DEBUG(1, ("Failed to set GENSEC target service: %s\n",
				  nt_errstr(c->status)));
			composite_error(c, c->status);
			return c;
		}
	}

	c->status = gensec_start_mech_by_authtype(sec->generic_state,
						  auth_type, auth_level);
	if (!NT_STATUS_IS_OK(c->status)) {
		DEBUG(1, ("Failed to start GENSEC client mechanism %s: %s\n",
			  gensec_get_name_by_authtype(auth_type),
			  nt_errstr(c->status)));
		composite_error(c, c->status);
		return c;
	}

	sec->auth_info = talloc(p, struct dcerpc_auth);
	if (composite_nomem(sec->auth_info, c)) return c;

	sec->auth_info->auth_type = auth_type;
	sec->auth_info->auth_level = auth_level,
	sec->auth_info->auth_pad_length = 0;
	sec->auth_info->auth_reserved = 0;
	sec->auth_info->auth_context_id = random();
	sec->auth_info->credentials = data_blob(NULL, 0);

	/* The status value here, from GENSEC is vital to the security
	 * of the system.  Even if the other end accepts, if GENSEC
	 * claims 'MORE_PROCESSING_REQUIRED' then you must keep
	 * feeding it blobs, or else the remote host/attacker might
	 * avoid mutal authentication requirements.
	 *
	 * Likewise, you must not feed GENSEC too much (after the OK),
	 * it doesn't like that either
	 */

	c->status = gensec_update(sec->generic_state, state,
				  sec->auth_info->credentials,
				  &state->credentials);
	if (!NT_STATUS_IS_OK(c->status) &&
	    !NT_STATUS_EQUAL(c->status, NT_STATUS_MORE_PROCESSING_REQUIRED)) {
		composite_error(c, c->status);
		return c;
	}

	state->more_processing = NT_STATUS_EQUAL(c->status,
						 NT_STATUS_MORE_PROCESSING_REQUIRED);

	if (state->credentials.length == 0) {
		composite_done(c);
		return c;
	}

	sec->auth_info->credentials = state->credentials;

	/* The first request always is a dcerpc_bind. The subsequent ones
	 * depend on gensec results */
	creq = dcerpc_bind_send(p, state, &syntax, &transfer_syntax);
	if (composite_nomem(creq, c)) return c;

	composite_continue(c, creq, bind_auth_recv_bindreply, c);
	return c;
}


/**
   Bind to a DCE/RPC pipe, receive result
   @param creq A composite context describing state of async call
   @retval NTSTATUS code
*/

NTSTATUS dcerpc_bind_auth_recv(struct composite_context *creq)
{
	NTSTATUS result = composite_wait(creq);
	struct bind_auth_state *state = talloc_get_type(creq->private_data,
							struct bind_auth_state);

	if (NT_STATUS_IS_OK(result)) {
		/*
		  after a successful authenticated bind the session
		  key reverts to the generic session key
		*/
		state->pipe->conn->security_state.session_key = dcerpc_generic_session_key;
	}
	
	talloc_free(creq);
	return result;
}


/**
   Perform a GENSEC authenticated bind to a DCE/RPC pipe, sync
   @param p The dcerpc_pipe to bind (must already be connected)
   @param table The interface table to use (the DCE/RPC bind both selects and interface and authenticates)
   @param credentials The credentials of the account to connect with 
   @param auth_type Select the authentication scheme to use
   @param auth_level Chooses between unprotected (connect), signed or sealed
   @param service The service (used by Kerberos to select the service principal to contact)
   @retval NTSTATUS status code
*/

NTSTATUS dcerpc_bind_auth(struct dcerpc_pipe *p,
			  const struct dcerpc_interface_table *table,
			  struct cli_credentials *credentials,
			  uint8_t auth_type, uint8_t auth_level,
			  const char *service)
{
	struct composite_context *creq;
	creq = dcerpc_bind_auth_send(p, p, table, credentials,
				     auth_type, auth_level, service);
	return dcerpc_bind_auth_recv(creq);
}
