/*
 * SPDX-FileCopyrightText: Jelmer Vernooij 2004-2005
 * SPDX-FileCopyrightText: Andrew Tridgell 2003-2005
 * SPDX-FileCopyrightText: Tim Potter 2003
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
   Unix SMB/CIFS implementation.
   raw dcerpc operations

   Copyright (C) Tim Potter 2003
   Copyright (C) Andrew Tridgell 2003-2005
   Copyright (C) Jelmer Vernooij 2004-2005

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
#include "lib/util/dlinklist.h"
#include "lib/events/events.h"
#include "librpc/rpc/dcerpc.h"
#include "librpc/gen_ndr/ndr_misc.h"
#include "librpc/gen_ndr/ndr_dcerpc.h"
#include "libcli/composite/composite.h"
#include "auth/gensec/gensec.h"

NTSTATUS dcerpc_init(void)
{
	gensec_init();

	return NT_STATUS_OK;
}

static void dcerpc_ship_next_request(struct dcerpc_connection *c);
static int dcerpc_req_destructor(struct rpc_request *r);
static int dcerpc_req_deny_destructor(struct rpc_request *r);

/* destroy a dcerpc connection */
static int dcerpc_connection_destructor(struct dcerpc_connection *c)
{
	if (c->transport.shutdown_pipe) {
		c->transport.shutdown_pipe(c);
	}
	return 0;
}


/* initialise a dcerpc connection.
   the event context is optional
*/
static struct dcerpc_connection *dcerpc_connection_init(TALLOC_CTX *mem_ctx,
						 struct event_context *ev)
{
	struct dcerpc_connection *c;

	c = talloc_zero(mem_ctx, struct dcerpc_connection);
	if (!c) {
		return NULL;
	}

	if (ev == NULL) {
		ev = event_context_init(c);
		if (ev == NULL) {
			talloc_free(c);
			return NULL;
		}
	}

	c->event_ctx = ev;

	if (!talloc_reference(c, ev)) {
		talloc_free(c);
		return NULL;
	}
	c->call_id = 1;
	c->security_state.auth_info = NULL;
	c->security_state.session_key = dcerpc_generic_session_key;
	c->security_state.generic_state = NULL;
	c->binding_string = NULL;
	c->flags = 0;
	c->srv_max_xmit_frag = 0;
	c->srv_max_recv_frag = 0;
	c->pending = NULL;

	talloc_set_destructor(c, dcerpc_connection_destructor);

	return c;
}

/* initialise a dcerpc pipe. */
struct dcerpc_pipe *dcerpc_pipe_init(TALLOC_CTX *mem_ctx, struct event_context *ev)
{
	struct dcerpc_pipe *p;

	p = talloc(mem_ctx, struct dcerpc_pipe);
	if (!p) {
		return NULL;
	}

	p->conn = dcerpc_connection_init(p, ev);
	if (p->conn == NULL) {
		talloc_free(p);
		return NULL;
	}

	p->last_fault_code = 0;
	p->context_id = 0;
	p->request_timeout = DCERPC_REQUEST_TIMEOUT;

	ZERO_STRUCT(p->syntax);
	ZERO_STRUCT(p->transfer_syntax);

	return p;
}


/*
   choose the next call id to use
*/
static uint32_t next_call_id(struct dcerpc_connection *c)
{
	c->call_id++;
	if (c->call_id == 0) {
		c->call_id++;
	}
	return c->call_id;
}

/* we need to be able to get/set the fragment length without doing a full
   decode */
void dcerpc_set_frag_length(DATA_BLOB *blob, uint16_t v)
{
	if (CVAL(blob->data,DCERPC_DREP_OFFSET) & DCERPC_DREP_LE) {
		SSVAL(blob->data, DCERPC_FRAG_LEN_OFFSET, v);
	} else {
		RSSVAL(blob->data, DCERPC_FRAG_LEN_OFFSET, v);
	}
}

uint16_t dcerpc_get_frag_length(const DATA_BLOB *blob)
{
	if (CVAL(blob->data,DCERPC_DREP_OFFSET) & DCERPC_DREP_LE) {
		return SVAL(blob->data, DCERPC_FRAG_LEN_OFFSET);
	} else {
		return RSVAL(blob->data, DCERPC_FRAG_LEN_OFFSET);
	}
}

void dcerpc_set_auth_length(DATA_BLOB *blob, uint16_t v)
{
	if (CVAL(blob->data,DCERPC_DREP_OFFSET) & DCERPC_DREP_LE) {
		SSVAL(blob->data, DCERPC_AUTH_LEN_OFFSET, v);
	} else {
		RSSVAL(blob->data, DCERPC_AUTH_LEN_OFFSET, v);
	}
}


/*
  setup for a ndr pull, also setting up any flags from the binding string
*/
static struct ndr_pull *ndr_pull_init_flags(struct dcerpc_connection *c,
					    DATA_BLOB *blob, TALLOC_CTX *mem_ctx)
{
	struct ndr_pull *ndr = ndr_pull_init_blob(blob, mem_ctx);

	if (ndr == NULL) return ndr;

	if (c->flags & DCERPC_DEBUG_PAD_CHECK) {
		ndr->flags |= LIBNDR_FLAG_PAD_CHECK;
	}

	if (c->flags & DCERPC_NDR_REF_ALLOC) {
		ndr->flags |= LIBNDR_FLAG_REF_ALLOC;
	}

	return ndr;
}

/*
   parse a data blob into a ncacn_packet structure. This handles both
   input and output packets
*/
static NTSTATUS ncacn_pull(struct dcerpc_connection *c, DATA_BLOB *blob, TALLOC_CTX *mem_ctx,
			    struct ncacn_packet *pkt)
{
	struct ndr_pull *ndr;

	ndr = ndr_pull_init_flags(c, blob, mem_ctx);
	if (!ndr) {
		return NT_STATUS_NO_MEMORY;
	}

	if (! (CVAL(blob->data, DCERPC_DREP_OFFSET) & DCERPC_DREP_LE)) {
		ndr->flags |= LIBNDR_FLAG_BIGENDIAN;
	}

	return ndr_pull_ncacn_packet(ndr, NDR_SCALARS|NDR_BUFFERS, pkt);
}

/*
  generate a CONNECT level verifier
*/
static NTSTATUS dcerpc_connect_verifier(TALLOC_CTX *mem_ctx, DATA_BLOB *blob)
{
	*blob = data_blob_talloc(mem_ctx, NULL, 16);
	if (blob->data == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	SIVAL(blob->data, 0, 1);
	memset(blob->data+4, 0, 12);
	return NT_STATUS_OK;
}

/*
  check a CONNECT level verifier
*/
static NTSTATUS dcerpc_check_connect_verifier(DATA_BLOB *blob)
{
	if (blob->length != 16 ||
	    IVAL(blob->data, 0) != 1) {
		return NT_STATUS_ACCESS_DENIED;
	}
	return NT_STATUS_OK;
}

/*
   parse the authentication information on a dcerpc response packet
*/
static NTSTATUS ncacn_pull_request_auth(struct dcerpc_connection *c, TALLOC_CTX *mem_ctx,
					DATA_BLOB *raw_packet,
					struct ncacn_packet *pkt)
{
	struct ndr_pull *ndr;
	NTSTATUS status;
	struct dcerpc_auth auth;
	DATA_BLOB auth_blob;

	if (pkt->auth_length == 0 &&
	    c->security_state.auth_info->auth_level == DCERPC_AUTH_LEVEL_CONNECT) {
		return NT_STATUS_OK;
	}

	auth_blob.length = 8 + pkt->auth_length;

	/* check for a valid length */
	if (pkt->u.response.stub_and_verifier.length < auth_blob.length) {
		return NT_STATUS_INFO_LENGTH_MISMATCH;
	}

	auth_blob.data =
		pkt->u.response.stub_and_verifier.data +
		pkt->u.response.stub_and_verifier.length - auth_blob.length;
	pkt->u.response.stub_and_verifier.length -= auth_blob.length;

	/* pull the auth structure */
	ndr = ndr_pull_init_flags(c, &auth_blob, mem_ctx);
	if (!ndr) {
		return NT_STATUS_NO_MEMORY;
	}

	if (!(pkt->drep[0] & DCERPC_DREP_LE)) {
		ndr->flags |= LIBNDR_FLAG_BIGENDIAN;
	}

	status = ndr_pull_dcerpc_auth(ndr, NDR_SCALARS|NDR_BUFFERS, &auth);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* check signature or unseal the packet */
	switch (c->security_state.auth_info->auth_level) {
	case DCERPC_AUTH_LEVEL_PRIVACY:
		status = gensec_unseal_packet(c->security_state.generic_state,
					      mem_ctx,
					      raw_packet->data + DCERPC_REQUEST_LENGTH,
					      pkt->u.response.stub_and_verifier.length,
					      raw_packet->data,
					      raw_packet->length - auth.credentials.length,
					      &auth.credentials);
		memcpy(pkt->u.response.stub_and_verifier.data,
		       raw_packet->data + DCERPC_REQUEST_LENGTH,
		       pkt->u.response.stub_and_verifier.length);
		break;

	case DCERPC_AUTH_LEVEL_INTEGRITY:
		status = gensec_check_packet(c->security_state.generic_state,
					     mem_ctx,
					     pkt->u.response.stub_and_verifier.data,
					     pkt->u.response.stub_and_verifier.length,
					     raw_packet->data,
					     raw_packet->length - auth.credentials.length,
					     &auth.credentials);
		break;

	case DCERPC_AUTH_LEVEL_CONNECT:
		status = dcerpc_check_connect_verifier(&auth.credentials);
		break;

	case DCERPC_AUTH_LEVEL_NONE:
		break;

	default:
		status = NT_STATUS_INVALID_LEVEL;
		break;
	}

	/* remove the indicated amount of paddiing */
	if (pkt->u.response.stub_and_verifier.length < auth.auth_pad_length) {
		return NT_STATUS_INFO_LENGTH_MISMATCH;
	}
	pkt->u.response.stub_and_verifier.length -= auth.auth_pad_length;

	return status;
}


/*
   push a dcerpc request packet into a blob, possibly signing it.
*/
static NTSTATUS ncacn_push_request_sign(struct dcerpc_connection *c,
					 DATA_BLOB *blob, TALLOC_CTX *mem_ctx,
					 struct ncacn_packet *pkt)
{
	NTSTATUS status;
	struct ndr_push *ndr;
	DATA_BLOB creds2;
	size_t payload_length;

	/* non-signed packets are simpler */
	if (!c->security_state.auth_info ||
	    !c->security_state.generic_state) {
		return ncacn_push_auth(blob, mem_ctx, pkt, c->security_state.auth_info);
	}

	ndr = ndr_push_init_ctx(mem_ctx);
	if (!ndr) {
		return NT_STATUS_NO_MEMORY;
	}

	if (c->flags & DCERPC_PUSH_BIGENDIAN) {
		ndr->flags |= LIBNDR_FLAG_BIGENDIAN;
	}

	if (pkt->pfc_flags & DCERPC_PFC_FLAG_ORPC) {
		ndr->flags |= LIBNDR_FLAG_OBJECT_PRESENT;
	}

	status = ndr_push_ncacn_packet(ndr, NDR_SCALARS|NDR_BUFFERS, pkt);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* pad to 16 byte multiple in the payload portion of the
	   packet. This matches what w2k3 does */
	c->security_state.auth_info->auth_pad_length =
		(16 - (pkt->u.request.stub_and_verifier.length & 15)) & 15;
	ndr_push_zero(ndr, c->security_state.auth_info->auth_pad_length);

	payload_length = pkt->u.request.stub_and_verifier.length +
		c->security_state.auth_info->auth_pad_length;

	/* sign or seal the packet */
	switch (c->security_state.auth_info->auth_level) {
	case DCERPC_AUTH_LEVEL_PRIVACY:
	case DCERPC_AUTH_LEVEL_INTEGRITY:
		/* We hope this length is accruate.  If must be if the
		 * GENSEC mech does AEAD signing of the packet
		 * headers */
		c->security_state.auth_info->credentials
			= data_blob_talloc(mem_ctx, NULL, gensec_sig_size(c->security_state.generic_state,
									  payload_length));
		data_blob_clear(&c->security_state.auth_info->credentials);
		break;

	case DCERPC_AUTH_LEVEL_CONNECT:
		status = dcerpc_connect_verifier(mem_ctx, &c->security_state.auth_info->credentials);
		break;

	case DCERPC_AUTH_LEVEL_NONE:
		c->security_state.auth_info->credentials = data_blob(NULL, 0);
		break;

	default:
		status = NT_STATUS_INVALID_LEVEL;
		break;
	}

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* add the auth verifier */
	status = ndr_push_dcerpc_auth(ndr, NDR_SCALARS|NDR_BUFFERS, c->security_state.auth_info);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* extract the whole packet as a blob */
	*blob = ndr_push_blob(ndr);

	/* fill in the fragment length and auth_length, we can't fill
	   in these earlier as we don't know the signature length (it
	   could be variable length) */
	dcerpc_set_frag_length(blob, blob->length);
	/* We hope this value is accruate.  If must be if the GENSEC
	 * mech does AEAD signing of the packet headers */
	dcerpc_set_auth_length(blob, c->security_state.auth_info->credentials.length);

	/* sign or seal the packet */
	switch (c->security_state.auth_info->auth_level) {
	case DCERPC_AUTH_LEVEL_PRIVACY:
		status = gensec_seal_packet(c->security_state.generic_state,
					    mem_ctx,
					    blob->data + DCERPC_REQUEST_LENGTH,
					    payload_length,
					    blob->data,
					    blob->length -
					    c->security_state.auth_info->credentials.length,
					    &creds2);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		blob->length -= c->security_state.auth_info->credentials.length;
		status = data_blob_append(mem_ctx, blob,
					  creds2.data, creds2.length);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		dcerpc_set_auth_length(blob, creds2.length);
		if (c->security_state.auth_info->credentials.length == 0) {
			/* this is needed for krb5 only, to correct the total packet
			   length */
			dcerpc_set_frag_length(blob,
					       dcerpc_get_frag_length(blob)
					       +creds2.length);
		}
		break;

	case DCERPC_AUTH_LEVEL_INTEGRITY:
		status = gensec_sign_packet(c->security_state.generic_state,
					    mem_ctx,
					    blob->data + DCERPC_REQUEST_LENGTH,
					    payload_length,
					    blob->data,
					    blob->length -
					    c->security_state.auth_info->credentials.length,
					    &creds2);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		blob->length -= c->security_state.auth_info->credentials.length;
		status = data_blob_append(mem_ctx, blob,
					  creds2.data, creds2.length);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		dcerpc_set_auth_length(blob, creds2.length);
		if (c->security_state.auth_info->credentials.length == 0) {
			/* this is needed for krb5 only, to correct the total packet
			   length */
			dcerpc_set_frag_length(blob,
					       dcerpc_get_frag_length(blob)
					       +creds2.length);
		}
		break;

	case DCERPC_AUTH_LEVEL_CONNECT:
		break;

	case DCERPC_AUTH_LEVEL_NONE:
		c->security_state.auth_info->credentials = data_blob(NULL, 0);
		break;

	default:
		status = NT_STATUS_INVALID_LEVEL;
		break;
	}

	data_blob_free(&c->security_state.auth_info->credentials);

	return NT_STATUS_OK;
}


/*
   fill in the fixed values in a dcerpc header
*/
static void init_ncacn_hdr(struct dcerpc_connection *c, struct ncacn_packet *pkt)
{
	pkt->rpc_vers = 5;
	pkt->rpc_vers_minor = 0;
	if (c->flags & DCERPC_PUSH_BIGENDIAN) {
		pkt->drep[0] = 0;
	} else {
		pkt->drep[0] = DCERPC_DREP_LE;
	}
	pkt->drep[1] = 0;
	pkt->drep[2] = 0;
	pkt->drep[3] = 0;
}

/*
  map a bind nak reason to a NTSTATUS
*/
static NTSTATUS dcerpc_map_reason(uint16_t reason)
{
	switch (reason) {
	case DCERPC_BIND_REASON_ASYNTAX:
		return NT_STATUS_RPC_UNSUPPORTED_NAME_SYNTAX;
	case DCERPC_BIND_REASON_INVALID_AUTH_TYPE:
		return NT_STATUS_INVALID_PARAMETER;
	}
	return NT_STATUS_UNSUCCESSFUL;
}

/*
  a bind or alter context has failed
*/
static void dcerpc_composite_fail(struct rpc_request *req)
{
	struct composite_context *c = talloc_get_type(req->async.private,
						      struct composite_context);
	composite_error(c, req->status);
}

/*
  mark the dcerpc connection dead. All outstanding requests get an error
*/
static void dcerpc_connection_dead(struct dcerpc_connection *conn, NTSTATUS status)
{
	/* bump ref count on connection to make sure we don't prematurely free memory 
	   in the callback */
	void *reference_keeper = talloc_named_const(NULL, 1, "CONN_REFERENCE_KEEPER");
	talloc_reference(reference_keeper, conn);

	/* make sure requests don't get destroyed too soon */
	struct rpc_request *tmpreq;
	for(tmpreq = conn->pending; tmpreq; tmpreq = tmpreq->next) {
		talloc_set_destructor(tmpreq, dcerpc_req_deny_destructor);
	} 

	/* all pending requests get the error */
	while (conn->pending) {
		struct rpc_request *req = talloc_get_type(conn->pending, struct rpc_request);
		if (!req) {
			DEBUG(0,("FAILED to extract rpc_request from connection pending list\n"));
			abort();
		}

		req->state = RPC_REQUEST_DONE;
		req->status = status;
		DLIST_REMOVE(conn->pending, req);
		if (req->async.callback) {
			req->async.callback(req);
		}

		/* finished with request now, reset dtor and free it */
		talloc_set_destructor(req, NULL);
		talloc_free(req);
	}

	talloc_free(reference_keeper);
}

/*
  forward declarations of the recv_data handlers for the types of
  packets we need to handle
*/
static void dcerpc_request_recv_data(struct dcerpc_connection *c,
				     DATA_BLOB *raw_packet, struct ncacn_packet *pkt);

/*
  receive a dcerpc reply from the transport. Here we work out what
  type of reply it is (normal request, bind or alter context) and
  dispatch to the appropriate handler
*/
static void dcerpc_recv_data(struct dcerpc_connection *conn, DATA_BLOB *blob, NTSTATUS status)
{
	struct ncacn_packet pkt;

	if (NT_STATUS_IS_OK(status) && blob->length == 0) {
		status = NT_STATUS_UNEXPECTED_NETWORK_ERROR;
	}

	/* the transport may be telling us of a severe error, such as
	   a dropped socket */
	if (!NT_STATUS_IS_OK(status)) {
		data_blob_free(blob);
		dcerpc_connection_dead(conn, status);
		return;
	}

	/* parse the basic packet to work out what type of response this is */
	status = ncacn_pull(conn, blob, blob->data, &pkt);
	if (!NT_STATUS_IS_OK(status)) {
		data_blob_free(blob);
		dcerpc_connection_dead(conn, status);
	}

	dcerpc_request_recv_data(conn, blob, &pkt);
}


/*
  Receive a bind reply from the transport
*/
static void dcerpc_bind_recv_handler(struct rpc_request *req,
				     DATA_BLOB *raw_packet, struct ncacn_packet *pkt)
{
	struct composite_context *c;
	struct dcerpc_connection *conn;

	c = talloc_get_type(req->async.private, struct composite_context);

	if (pkt->ptype == DCERPC_PKT_BIND_NAK) {
		DEBUG(2,("dcerpc: bind_nak reason %d\n",
			 pkt->u.bind_nak.reject_reason));
		composite_error(c, dcerpc_map_reason(pkt->u.bind_nak.
						     reject_reason));
		return;
	}

	if ((pkt->ptype != DCERPC_PKT_BIND_ACK) ||
	    (pkt->u.bind_ack.num_results == 0) ||
	    (pkt->u.bind_ack.ctx_list[0].result != 0)) {
		composite_error(c, NT_STATUS_NET_WRITE_FAULT);
		return;
	}

	conn = req->p->conn;

	conn->srv_max_xmit_frag = pkt->u.bind_ack.max_xmit_frag;
	conn->srv_max_recv_frag = pkt->u.bind_ack.max_recv_frag;

	/* the bind_ack might contain a reply set of credentials */
	if (conn->security_state.auth_info &&
	    pkt->u.bind_ack.auth_info.length) {
		c->status = ndr_pull_struct_blob(
			&pkt->u.bind_ack.auth_info, conn,
			conn->security_state.auth_info,
			(ndr_pull_flags_fn_t)ndr_pull_dcerpc_auth);
		if (!composite_is_ok(c)) return;
	}

	composite_done(c);
}

/*
  handle timeouts of individual dcerpc requests
*/
static void dcerpc_timeout_handler(struct event_context *ev, struct timed_event *te,
				   struct timeval t, void *private)
{
	struct rpc_request *req = talloc_get_type(private, struct rpc_request);

	if (req->state != RPC_REQUEST_PENDING) {
		return;
	}

	req->status = NT_STATUS_IO_TIMEOUT;
	req->state = RPC_REQUEST_DONE;
	DLIST_REMOVE(req->p->conn->pending, req);
	if (req->async.callback) {
		req->async.callback(req);
	}
}


/*
  send a async dcerpc bind request
*/
struct composite_context *dcerpc_bind_send(struct dcerpc_pipe *p,
					   TALLOC_CTX *mem_ctx,
					   const struct dcerpc_syntax_id *syntax,
					   const struct dcerpc_syntax_id *transfer_syntax)
{
	struct composite_context *c;
	struct ncacn_packet pkt;
	DATA_BLOB blob;
	struct rpc_request *req;

	c = composite_create(mem_ctx,p->conn->event_ctx);
	if (c == NULL) return NULL;

	c->private_data = p;

	p->syntax = *syntax;
	p->transfer_syntax = *transfer_syntax;

	init_ncacn_hdr(p->conn, &pkt);

	pkt.ptype = DCERPC_PKT_BIND;
	pkt.pfc_flags = DCERPC_PFC_FLAG_FIRST | DCERPC_PFC_FLAG_LAST;
	pkt.call_id = p->conn->call_id;
	pkt.auth_length = 0;

	pkt.u.bind.max_xmit_frag = 5840;
	pkt.u.bind.max_recv_frag = 5840;
	pkt.u.bind.assoc_group_id = 0;
	pkt.u.bind.num_contexts = 1;
	pkt.u.bind.ctx_list = talloc_array(mem_ctx, struct dcerpc_ctx_list, 1);
	if (composite_nomem(pkt.u.bind.ctx_list, c)) return c;
	pkt.u.bind.ctx_list[0].context_id = p->context_id;
	pkt.u.bind.ctx_list[0].num_transfer_syntaxes = 1;
	pkt.u.bind.ctx_list[0].abstract_syntax = p->syntax;
	pkt.u.bind.ctx_list[0].transfer_syntaxes = &p->transfer_syntax;
	pkt.u.bind.auth_info = data_blob(NULL, 0);

	/* construct the NDR form of the packet */
	c->status = ncacn_push_auth(&blob, c, &pkt,
				    p->conn->security_state.auth_info);
	if (!composite_is_ok(c)) return c;

	p->conn->transport.recv_data = dcerpc_recv_data;

	/*
	 * we allocate a dcerpc_request so we can be in the same
	 * request queue as normal requests
	 */
	req = talloc_zero(c, struct rpc_request);
	if (composite_nomem(req, c)) return c;

	req->state = RPC_REQUEST_PENDING;
	req->call_id = pkt.call_id;
	req->async.private = c;
	req->async.callback = dcerpc_composite_fail;
	req->p = p;
	req->recv_handler = dcerpc_bind_recv_handler;
	DLIST_ADD_END(p->conn->pending, req, struct rpc_request *);

	c->status = p->conn->transport.send_request(p->conn, &blob,
						    True);
	if (!composite_is_ok(c)) return c;

	event_add_timed(c->event_ctx, req,
			timeval_current_ofs(DCERPC_REQUEST_TIMEOUT, 0),
			dcerpc_timeout_handler, req);

	return c;
}

/*
  recv side of async dcerpc bind request
*/
NTSTATUS dcerpc_bind_recv(struct composite_context *ctx)
{
	NTSTATUS result = composite_wait(ctx);
	talloc_free(ctx);
	return result;
}

/*
   perform a continued bind (and auth3)
*/
NTSTATUS dcerpc_auth3(struct dcerpc_connection *c,
		      TALLOC_CTX *mem_ctx)
{
	struct ncacn_packet pkt;
	NTSTATUS status;
	DATA_BLOB blob;

	init_ncacn_hdr(c, &pkt);

	pkt.ptype = DCERPC_PKT_AUTH3;
	pkt.pfc_flags = DCERPC_PFC_FLAG_FIRST | DCERPC_PFC_FLAG_LAST;
	pkt.call_id = next_call_id(c);
	pkt.auth_length = 0;
	pkt.u.auth3._pad = 0;
	pkt.u.auth3.auth_info = data_blob(NULL, 0);

	/* construct the NDR form of the packet */
	status = ncacn_push_auth(&blob, mem_ctx, &pkt, c->security_state.auth_info);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* send it on its way */
	status = c->transport.send_request(c, &blob, False);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	return status;
}


/*
  process a fragment received from the transport layer during a
  request

  This function frees the data
*/
static void dcerpc_request_recv_data(struct dcerpc_connection *c,
				     DATA_BLOB *raw_packet, struct ncacn_packet *pkt)
{
	struct rpc_request *req;
	uint_t length;
	NTSTATUS status = NT_STATUS_OK;

	/*
	  if this is an authenticated connection then parse and check
	  the auth info. We have to do this before finding the
	  matching packet, as the request structure might have been
	  removed due to a timeout, but if it has been we still need
	  to run the auth routines so that we don't get the sign/seal
	  info out of step with the server
	*/
	if (c->security_state.auth_info && c->security_state.generic_state &&
	    pkt->ptype == DCERPC_PKT_RESPONSE) {
		status = ncacn_pull_request_auth(c, raw_packet->data, raw_packet, pkt);
	}

	/* find the matching request */
	for (req=c->pending;req;req=req->next) {
		if (pkt->call_id == req->call_id) break;
	}

#if 0
	/* useful for testing certain vendors RPC servers */
	if (req == NULL && c->pending && pkt->call_id == 0) {
		DEBUG(0,("HACK FOR INCORRECT CALL ID\n"));
		req = c->pending;
	}
#endif

	if (req == NULL) {
		DEBUG(2,("dcerpc_request: unmatched call_id %u in response packet\n", pkt->call_id));
		data_blob_free(raw_packet);
		return;
	}

	talloc_steal(req, raw_packet->data);

	if (req->recv_handler != NULL) {
		req->state = RPC_REQUEST_DONE;
		DLIST_REMOVE(c->pending, req);
		req->recv_handler(req, raw_packet, pkt);
		return;
	}

	if (pkt->ptype == DCERPC_PKT_FAULT) {
		DEBUG(5,("rpc fault: %s\n", dcerpc_errstr(c, pkt->u.fault.status)));
		req->fault_code = pkt->u.fault.status;

		/*
		 * added by cgibbons@zenoss on 2008/09/11 - propagate access denied
		 * errors up to higher layers
		 */
		if (req->fault_code == DCERPC_FAULT_ACCESS_DENIED)
		    req->status = NT_STATUS_ACCESS_DENIED;
		else
		    req->status = NT_STATUS_NET_WRITE_FAULT;
		goto req_done;
	}

	if (pkt->ptype != DCERPC_PKT_RESPONSE) {
		DEBUG(2,("Unexpected packet type %d in dcerpc response\n",
			 (int)pkt->ptype));
		req->fault_code = DCERPC_FAULT_OTHER;
		req->status = NT_STATUS_NET_WRITE_FAULT;
		goto req_done;
	}

	/* now check the status from the auth routines, and if it failed then fail
	   this request accordingly */
	if (!NT_STATUS_IS_OK(status)) {
		req->status = status;
		goto req_done;
	}

	length = pkt->u.response.stub_and_verifier.length;

	if (length > 0) {
		req->payload.data = talloc_realloc(req,
						   req->payload.data,
						   uint8_t,
						   req->payload.length + length);
		if (!req->payload.data) {
			req->status = NT_STATUS_NO_MEMORY;
			goto req_done;
		}
		memcpy(req->payload.data+req->payload.length,
		       pkt->u.response.stub_and_verifier.data, length);
		req->payload.length += length;
	}

	if (!(pkt->pfc_flags & DCERPC_PFC_FLAG_LAST)) {
		c->transport.send_read(c);
		return;
	}

	if (!(pkt->drep[0] & DCERPC_DREP_LE)) {
		req->flags |= DCERPC_PULL_BIGENDIAN;
	} else {
		req->flags &= ~DCERPC_PULL_BIGENDIAN;
	}


req_done:
	/* we've got the full payload */
	req->state = RPC_REQUEST_DONE;
	DLIST_REMOVE(c->pending, req);

	if (c->request_queue != NULL) {
		/* We have to look at shipping further requests before calling
		 * the async function, that one might close the pipe */
		dcerpc_ship_next_request(c);
	}

	if (req->async.callback) {
		req->async.callback(req);
	}
}

/*
  make sure requests are cleaned up
 */
static int dcerpc_req_destructor(struct rpc_request *req)
{
	DLIST_REMOVE(req->p->conn->pending, req);
	return 0;
}

static int dcerpc_req_deny_destructor(struct rpc_request *req)
{
	return -1;
}

/*
  perform the send side of a async dcerpc request
*/
static struct rpc_request *dcerpc_request_send(struct dcerpc_pipe *p,
					       const struct GUID *object,
					       uint16_t opnum,
					       BOOL async,
					       DATA_BLOB *stub_data)
{
	struct rpc_request *req;

	p->conn->transport.recv_data = dcerpc_recv_data;

	req = talloc(p, struct rpc_request);
	if (req == NULL) {
		return NULL;
	}

	req->p = p;
	req->call_id = next_call_id(p->conn);
	req->status = NT_STATUS_OK;
	req->state = RPC_REQUEST_PENDING;
	req->payload = data_blob(NULL, 0);
	req->flags = 0;
	req->fault_code = 0;
	req->async_call = async;
	req->async.callback = NULL;
	req->async.private = NULL;
	req->recv_handler = NULL;

	if (object != NULL) {
		req->object = talloc_memdup(req, object, sizeof(*object));
		if (req->object == NULL) {
			talloc_free(req);
			return NULL;
		}
	} else {
		req->object = NULL;
	}

	req->opnum = opnum;
	req->request_data.length = stub_data->length;
	req->request_data.data = talloc_reference(req, stub_data->data);
	if (req->request_data.data == NULL) {
		return NULL;
	}

	DLIST_ADD_END(p->conn->request_queue, req, struct rpc_request *);

	dcerpc_ship_next_request(p->conn);

	if (p->request_timeout) {
		event_add_timed(dcerpc_event_context(p), req,
				timeval_current_ofs(p->request_timeout, 0),
				dcerpc_timeout_handler, req);
	}

	talloc_set_destructor(req, dcerpc_req_destructor);
	return req;
}

/*
  Send a request using the transport
*/

static void dcerpc_ship_next_request(struct dcerpc_connection *c)
{
	struct rpc_request *req;
	struct dcerpc_pipe *p;
	DATA_BLOB *stub_data;
	struct ncacn_packet pkt;
	DATA_BLOB blob;
	uint32_t remaining, chunk_size;
	BOOL first_packet = True;

	req = c->request_queue;
	if (req == NULL) {
		return;
	}

	p = req->p;
	stub_data = &req->request_data;

	if (!req->async_call && (c->pending != NULL)) {
		return;
	}

	DLIST_REMOVE(c->request_queue, req);
	DLIST_ADD(c->pending, req);

	init_ncacn_hdr(p->conn, &pkt);

	remaining = stub_data->length;

	/* we can write a full max_recv_frag size, minus the dcerpc
	   request header size */
	chunk_size = p->conn->srv_max_recv_frag - (DCERPC_MAX_SIGN_SIZE+DCERPC_REQUEST_LENGTH);

	pkt.ptype = DCERPC_PKT_REQUEST;
	pkt.call_id = req->call_id;
	pkt.auth_length = 0;
	pkt.pfc_flags = 0;
	pkt.u.request.alloc_hint = remaining;
	pkt.u.request.context_id = p->context_id;
	pkt.u.request.opnum = req->opnum;

	if (req->object) {
		pkt.u.request.object.object = *req->object;
		pkt.pfc_flags |= DCERPC_PFC_FLAG_ORPC;
		chunk_size -= ndr_size_GUID(req->object,0);
	}

	/* we send a series of pdus without waiting for a reply */
	while (remaining > 0 || first_packet) {
		uint32_t chunk = MIN(chunk_size, remaining);
		BOOL last_frag = False;

		first_packet = False;
		pkt.pfc_flags &= ~(DCERPC_PFC_FLAG_FIRST |DCERPC_PFC_FLAG_LAST);

		if (remaining == stub_data->length) {
			pkt.pfc_flags |= DCERPC_PFC_FLAG_FIRST;
		}
		if (chunk == remaining) {
			pkt.pfc_flags |= DCERPC_PFC_FLAG_LAST;
			last_frag = True;
		}

		pkt.u.request.stub_and_verifier.data = stub_data->data +
			(stub_data->length - remaining);
		pkt.u.request.stub_and_verifier.length = chunk;

		req->status = ncacn_push_request_sign(p->conn, &blob, req, &pkt);
		if (!NT_STATUS_IS_OK(req->status)) {
			req->state = RPC_REQUEST_DONE;
			DLIST_REMOVE(p->conn->pending, req);
			return;
		}

		req->status = p->conn->transport.send_request(p->conn, &blob, last_frag);
		if (!NT_STATUS_IS_OK(req->status)) {
			req->state = RPC_REQUEST_DONE;
			DLIST_REMOVE(p->conn->pending, req);
			return;
		}

		remaining -= chunk;
	}
}

/*
  return the event context for a dcerpc pipe
  used by callers who wish to operate asynchronously
*/
struct event_context *dcerpc_event_context(struct dcerpc_pipe *p)
{
	return p->conn->event_ctx;
}



/*
  perform the receive side of a async dcerpc request
*/
NTSTATUS dcerpc_request_recv(struct rpc_request *req,
			     TALLOC_CTX *mem_ctx,
			     DATA_BLOB *stub_data)
{
	NTSTATUS status;

	while (req->state == RPC_REQUEST_PENDING) {
		struct event_context *ctx = dcerpc_event_context(req->p);
		if (event_loop_once(ctx) != 0) {
			return NT_STATUS_CONNECTION_DISCONNECTED;
		}
	}
	*stub_data = req->payload;
	status = req->status;
	if (stub_data->data) {
		stub_data->data = talloc_steal(mem_ctx, stub_data->data);
	}
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(9,(__location__": dcerpc_request_recv status = %s\n", nt_errstr(status)));
	}
	if (!NT_STATUS_IS_OK(status)) {
		req->p->last_fault_code = req->fault_code ? req->fault_code : DCERPC_FAULT_OTHER;
	}
	talloc_free(req);
	return status;
}

/*
  perform a full request/response pair on a dcerpc pipe
*/
NTSTATUS dcerpc_request(struct dcerpc_pipe *p,
			struct GUID *object,
			uint16_t opnum,
			BOOL async,
			TALLOC_CTX *mem_ctx,
			DATA_BLOB *stub_data_in,
			DATA_BLOB *stub_data_out)
{
	struct rpc_request *req;

	req = dcerpc_request_send(p, object, opnum, async, stub_data_in);
	if (req == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	return dcerpc_request_recv(req, mem_ctx, stub_data_out);
}


/*
  this is a paranoid NDR validator. For every packet we push onto the wire
  we pull it back again, then push it again. Then we compare the raw NDR data
  for that to the NDR we initially generated. If they don't match then we know
  we must have a bug in either the pull or push side of our code
*/
static NTSTATUS dcerpc_ndr_validate_in(struct dcerpc_connection *c,
				       TALLOC_CTX *mem_ctx,
				       DATA_BLOB blob,
				       size_t struct_size,
				       ndr_push_flags_fn_t ndr_push,
				       ndr_pull_flags_fn_t ndr_pull)
{
	void *st;
	struct ndr_pull *pull;
	struct ndr_push *push;
	NTSTATUS status;
	DATA_BLOB blob2;

	st = talloc_size(mem_ctx, struct_size);
	if (!st) {
		return NT_STATUS_NO_MEMORY;
	}

	pull = ndr_pull_init_flags(c, &blob, mem_ctx);
	if (!pull) {
		return NT_STATUS_NO_MEMORY;
	}
	pull->flags |= LIBNDR_FLAG_REF_ALLOC;

	status = ndr_pull(pull, NDR_IN, st);
	if (!NT_STATUS_IS_OK(status)) {
		return ndr_pull_error(pull, NDR_ERR_VALIDATE,
				      "failed input validation pull - %s",
				      nt_errstr(status));
	}

	push = ndr_push_init_ctx(mem_ctx);
	if (!push) {
		return NT_STATUS_NO_MEMORY;
	}

	status = ndr_push(push, NDR_IN, st);
	if (!NT_STATUS_IS_OK(status)) {
		return ndr_push_error(push, NDR_ERR_VALIDATE,
				      "failed input validation push - %s",
				      nt_errstr(status));
	}

	blob2 = ndr_push_blob(push);

	if (!data_blob_equal(&blob, &blob2)) {
		DEBUG(3,("original:\n"));
		dump_data(3, blob.data, blob.length);
		DEBUG(3,("secondary:\n"));
		dump_data(3, blob2.data, blob2.length);
		return ndr_push_error(push, NDR_ERR_VALIDATE,
				      "failed input validation data - %s",
				      nt_errstr(status));
	}

	return NT_STATUS_OK;
}

/*
  this is a paranoid NDR input validator. For every packet we pull
  from the wire we push it back again then pull and push it
  again. Then we compare the raw NDR data for that to the NDR we
  initially generated. If they don't match then we know we must have a
  bug in either the pull or push side of our code
*/
static NTSTATUS dcerpc_ndr_validate_out(struct dcerpc_connection *c,
					struct ndr_pull *pull_in,
					void *struct_ptr,
					size_t struct_size,
					ndr_push_flags_fn_t ndr_push,
					ndr_pull_flags_fn_t ndr_pull,
					ndr_print_function_t ndr_print)
{
	void *st;
	struct ndr_pull *pull;
	struct ndr_push *push;
	NTSTATUS status;
	DATA_BLOB blob, blob2;
	TALLOC_CTX *mem_ctx = pull_in;
	char *s1, *s2;

	st = talloc_size(mem_ctx, struct_size);
	if (!st) {
		return NT_STATUS_NO_MEMORY;
	}
	memcpy(st, struct_ptr, struct_size);

	push = ndr_push_init_ctx(mem_ctx);
	if (!push) {
		return NT_STATUS_NO_MEMORY;
	}

	status = ndr_push(push, NDR_OUT, struct_ptr);
	if (!NT_STATUS_IS_OK(status)) {
		return ndr_push_error(push, NDR_ERR_VALIDATE,
				      "failed output validation push - %s",
				      nt_errstr(status));
	}

	blob = ndr_push_blob(push);

	pull = ndr_pull_init_flags(c, &blob, mem_ctx);
	if (!pull) {
		return NT_STATUS_NO_MEMORY;
	}

	pull->flags |= LIBNDR_FLAG_REF_ALLOC;
	status = ndr_pull(pull, NDR_OUT, st);
	if (!NT_STATUS_IS_OK(status)) {
		return ndr_pull_error(pull, NDR_ERR_VALIDATE,
				      "failed output validation pull - %s",
				      nt_errstr(status));
	}

	push = ndr_push_init_ctx(mem_ctx);
	if (!push) {
		return NT_STATUS_NO_MEMORY;
	}

	status = ndr_push(push, NDR_OUT, st);
	if (!NT_STATUS_IS_OK(status)) {
		return ndr_push_error(push, NDR_ERR_VALIDATE,
				      "failed output validation push2 - %s",
				      nt_errstr(status));
	}

	blob2 = ndr_push_blob(push);

	if (!data_blob_equal(&blob, &blob2)) {
		DEBUG(3,("original:\n"));
		dump_data(3, blob.data, blob.length);
		DEBUG(3,("secondary:\n"));
		dump_data(3, blob2.data, blob2.length);
		return ndr_push_error(push, NDR_ERR_VALIDATE,
				      "failed output validation data - %s",
				      nt_errstr(status));
	}

	/* this checks the printed forms of the two structures, which effectively
	   tests all of the value() attributes */
	s1 = ndr_print_function_string(mem_ctx, ndr_print, "VALIDATE",
				       NDR_OUT, struct_ptr);
	s2 = ndr_print_function_string(mem_ctx, ndr_print, "VALIDATE",
				       NDR_OUT, st);
	if (strcmp(s1, s2) != 0) {
#if 1
		printf("VALIDATE ERROR:\nWIRE:\n%s\n GEN:\n%s\n", s1, s2);
#else
		/* this is sometimes useful */
		printf("VALIDATE ERROR\n");
		file_save("wire.dat", s1, strlen(s1));
		file_save("gen.dat", s2, strlen(s2));
		system("diff -u wire.dat gen.dat");
#endif
	}

	return NT_STATUS_OK;
}


/*
 send a rpc request given a dcerpc_call structure
 */
struct rpc_request *dcerpc_ndr_request_send(struct dcerpc_pipe *p,
						const struct GUID *object,
						const struct dcerpc_interface_table *table,
						uint32_t opnum,
						TALLOC_CTX *mem_ctx,
						void *r)
{
	const struct dcerpc_interface_call *call;
	struct ndr_push *push;
	NTSTATUS status;
	DATA_BLOB request;
	struct rpc_request *req;

	call = &table->calls[opnum];

	/* setup for a ndr_push_* call */
	push = ndr_push_init_ctx(mem_ctx);
	if (!push) {
		return NULL;
	}

	if (p->conn->flags & DCERPC_PUSH_BIGENDIAN) {
		push->flags |= LIBNDR_FLAG_BIGENDIAN;
	}

	/* push the structure into a blob */
	status = call->ndr_push(push, NDR_IN, r);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(2,("Unable to ndr_push structure in dcerpc_ndr_request_send - %s\n",
			 nt_errstr(status)));
		talloc_free(push);
		return NULL;
	}

	/* retrieve the blob */
	request = ndr_push_blob(push);

	if (p->conn->flags & DCERPC_DEBUG_VALIDATE_IN) {
		status = dcerpc_ndr_validate_in(p->conn, push, request, call->struct_size,
						call->ndr_push, call->ndr_pull);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(2,("Validation failed in dcerpc_ndr_request_send - %s\n",
				 nt_errstr(status)));
			talloc_free(push);
			return NULL;
		}
	}

	DEBUG(10,("rpc request data:\n"));
	dump_data(10, request.data, request.length);

	/* make the actual dcerpc request */
	req = dcerpc_request_send(p, object, opnum, table->calls[opnum].async,
				  &request);

	if (req != NULL) {
		req->ndr.table = table;
		req->ndr.opnum = opnum;
		req->ndr.struct_ptr = r;
		req->ndr.mem_ctx = mem_ctx;
	}

	talloc_free(push);

	return req;
}

/*
  receive the answer from a dcerpc_ndr_request_send()
*/
_PUBLIC_ NTSTATUS dcerpc_ndr_request_recv(struct rpc_request *req)
{
	struct dcerpc_pipe *p = req->p;
	NTSTATUS status;
	DATA_BLOB response;
	struct ndr_pull *pull;
	uint_t flags;
	TALLOC_CTX *mem_ctx = req->ndr.mem_ctx;
	void *r = req->ndr.struct_ptr;
	uint32_t opnum = req->ndr.opnum;
	const struct dcerpc_interface_table *table = req->ndr.table;
	const struct dcerpc_interface_call *call = &table->calls[opnum];

	/* make sure the recv code doesn't free the request, as we
	   need to grab the flags element before it is freed */
	talloc_increase_ref_count(req);

	status = dcerpc_request_recv(req, mem_ctx, &response);
	if (!NT_STATUS_IS_OK(status)) {
		talloc_free(req);
		return status;
	}

	flags = req->flags;

	/* prepare for ndr_pull_* */
	pull = ndr_pull_init_flags(p->conn, &response, mem_ctx);
	if (!pull) {
		talloc_free(req);
		return NT_STATUS_NO_MEMORY;
	}

	if (pull->data) {
		pull->data = talloc_steal(pull, pull->data);
	}
	talloc_free(req);

	if (flags & DCERPC_PULL_BIGENDIAN) {
		pull->flags |= LIBNDR_FLAG_BIGENDIAN;
	}

	DEBUG(10,("rpc reply data:\n"));
	dump_data(10, pull->data, pull->data_size);

	/* pull the structure from the blob */
	status = call->ndr_pull(pull, NDR_OUT, r);
	if (!NT_STATUS_IS_OK(status)) {
		dcerpc_log_packet(table, opnum, NDR_OUT,
				  &response);
		return status;
	}

	if (p->conn->flags & DCERPC_DEBUG_VALIDATE_OUT) {
		status = dcerpc_ndr_validate_out(p->conn, pull, r, call->struct_size,
						 call->ndr_push, call->ndr_pull,
						 call->ndr_print);
		if (!NT_STATUS_IS_OK(status)) {
			dcerpc_log_packet(table, opnum, NDR_OUT,
				  &response);
			return status;
		}
	}

	if (pull->offset != pull->data_size) {
		DEBUG(0,("Warning! ignoring %d unread bytes in rpc packet!\n",
			 pull->data_size - pull->offset));
		/* we used to return NT_STATUS_INFO_LENGTH_MISMATCH here,
		   but it turns out that early versions of NT
		   (specifically NT3.1) add junk onto the end of rpc
		   packets, so if we want to interoperate at all with
		   those versions then we need to ignore this error */
	}

	/* TODO: make pull context independent from the output mem_ctx and free the pull context */

	return NT_STATUS_OK;
}


/*
  a useful helper function for synchronous rpc requests

  this can be used when you have ndr push/pull functions in the
  standard format
*/
NTSTATUS dcerpc_ndr_request(struct dcerpc_pipe *p,
			    const struct GUID *object,
			    const struct dcerpc_interface_table *table,
			    uint32_t opnum,
			    TALLOC_CTX *mem_ctx,
			    void *r)
{
	struct rpc_request *req;

	req = dcerpc_ndr_request_send(p, object, table, opnum, mem_ctx, r);
	if (req == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	return dcerpc_ndr_request_recv(req);
}


/*
  a useful function for retrieving the server name we connected to
*/
const char *dcerpc_server_name(struct dcerpc_pipe *p)
{
	if (!p->conn->transport.peer_name) {
		return "";
	}
	return p->conn->transport.peer_name(p->conn);
}


/*
  get the dcerpc auth_level for a open connection
*/
uint32_t dcerpc_auth_level(struct dcerpc_connection *c)
{
	uint8_t auth_level;

	if (c->flags & DCERPC_SEAL) {
		auth_level = DCERPC_AUTH_LEVEL_PRIVACY;
	} else if (c->flags & DCERPC_SIGN) {
		auth_level = DCERPC_AUTH_LEVEL_INTEGRITY;
	} else if (c->flags & DCERPC_CONNECT) {
		auth_level = DCERPC_AUTH_LEVEL_CONNECT;
	} else {
		auth_level = DCERPC_AUTH_LEVEL_NONE;
	}
	return auth_level;
}

/*
  Receive an alter reply from the transport
*/
static void dcerpc_alter_recv_handler(struct rpc_request *req,
				      DATA_BLOB *raw_packet, struct ncacn_packet *pkt)
{
	struct composite_context *c;
	struct dcerpc_pipe *recv_pipe;

	c = talloc_get_type(req->async.private, struct composite_context);
	recv_pipe = talloc_get_type(c->private_data, struct dcerpc_pipe);

	if (pkt->ptype == DCERPC_PKT_ALTER_RESP &&
	    pkt->u.alter_resp.num_results == 1 &&
	    pkt->u.alter_resp.ctx_list[0].result != 0) {
		DEBUG(2,("dcerpc: alter_resp failed - reason %d\n",
			 pkt->u.alter_resp.ctx_list[0].reason));
		composite_error(c, dcerpc_map_reason(pkt->u.alter_resp.ctx_list[0].reason));
		return;
	}

	if (pkt->ptype != DCERPC_PKT_ALTER_RESP ||
	    pkt->u.alter_resp.num_results == 0 ||
	    pkt->u.alter_resp.ctx_list[0].result != 0) {
		composite_error(c, NT_STATUS_NET_WRITE_FAULT);
		return;
	}

	/* the alter_resp might contain a reply set of credentials */
	if (recv_pipe->conn->security_state.auth_info &&
	    pkt->u.alter_resp.auth_info.length) {
		c->status = ndr_pull_struct_blob(
			&pkt->u.alter_resp.auth_info, recv_pipe,
			recv_pipe->conn->security_state.auth_info,
			(ndr_pull_flags_fn_t)ndr_pull_dcerpc_auth);
		if (!composite_is_ok(c)) return;
	}

	composite_done(c);
}

/*
   send a dcerpc alter_context request
*/
struct composite_context *dcerpc_alter_context_send(struct dcerpc_pipe *p,
						    TALLOC_CTX *mem_ctx,
						    const struct dcerpc_syntax_id *syntax,
						    const struct dcerpc_syntax_id *transfer_syntax)
{
	struct composite_context *c;
	struct ncacn_packet pkt;
	DATA_BLOB blob;
	struct rpc_request *req;

	c = composite_create(mem_ctx, p->conn->event_ctx);
	if (c == NULL) return NULL;

	c->private_data = p;

	p->syntax = *syntax;
	p->transfer_syntax = *transfer_syntax;

	init_ncacn_hdr(p->conn, &pkt);

	pkt.ptype = DCERPC_PKT_ALTER;
	pkt.pfc_flags = DCERPC_PFC_FLAG_FIRST | DCERPC_PFC_FLAG_LAST;
	pkt.call_id = p->conn->call_id;
	pkt.auth_length = 0;

	pkt.u.alter.max_xmit_frag = 5840;
	pkt.u.alter.max_recv_frag = 5840;
	pkt.u.alter.assoc_group_id = 0;
	pkt.u.alter.num_contexts = 1;
	pkt.u.alter.ctx_list = talloc_array(c, struct dcerpc_ctx_list, 1);
	if (composite_nomem(pkt.u.alter.ctx_list, c)) return c;
	pkt.u.alter.ctx_list[0].context_id = ++p->context_id;
	pkt.u.alter.ctx_list[0].num_transfer_syntaxes = 1;
	pkt.u.alter.ctx_list[0].abstract_syntax = p->syntax;
	pkt.u.alter.ctx_list[0].transfer_syntaxes = &p->transfer_syntax;
	pkt.u.alter.auth_info = data_blob(NULL, 0);

	/* construct the NDR form of the packet */
	/* FIXME:low mem_ctx can be freed earlier */
	c->status = ncacn_push_auth(&blob, c, &pkt,
				    p->conn->security_state.auth_info);
	if (!composite_is_ok(c)) return c;

	p->conn->transport.recv_data = dcerpc_recv_data;

	/*
	 * we allocate a dcerpc_request so we can be in the same
	 * request queue as normal requests
	 */
	req = talloc_zero(c, struct rpc_request);
	if (composite_nomem(req, c)) return c;

	req->state = RPC_REQUEST_PENDING;
	req->call_id = pkt.call_id;
	req->async.private = c;
	req->async.callback = dcerpc_composite_fail;
	req->p = p;
	req->recv_handler = dcerpc_alter_recv_handler;
	DLIST_ADD_END(p->conn->pending, req, struct rpc_request *);

	c->status = p->conn->transport.send_request(p->conn, &blob, True);
	if (!composite_is_ok(c)) return c;

	event_add_timed(c->event_ctx, req,
			timeval_current_ofs(DCERPC_REQUEST_TIMEOUT, 0),
			dcerpc_timeout_handler, req);

	return c;
}

NTSTATUS dcerpc_alter_context_recv(struct composite_context *ctx)
{
	NTSTATUS result = composite_wait(ctx);
	talloc_free(ctx);
	return result;
}

/*
   send a dcerpc alter_context request
*/
NTSTATUS dcerpc_alter_context(struct dcerpc_pipe *p,
			      TALLOC_CTX *mem_ctx,
			      const struct dcerpc_syntax_id *syntax,
			      const struct dcerpc_syntax_id *transfer_syntax)
{
	struct composite_context *creq;
	creq = dcerpc_alter_context_send(p, mem_ctx, syntax, transfer_syntax);
	return dcerpc_alter_context_recv(creq);
}
