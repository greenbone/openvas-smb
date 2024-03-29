/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2003-2005
 * SPDX-FileCopyrightText: Tim Potter 2003
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   DCERPC client side interface structures

   Copyright (C) Tim Potter 2003
   Copyright (C) Andrew Tridgell 2003-2005
   
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

#ifndef __DCERPC_H__
#define __DCERPC_H__

#include "core.h"
#include "librpc/gen_ndr/dcerpc.h"
#include "librpc/ndr/libndr.h"

enum dcerpc_transport_t {
	NCACN_NP, NCACN_IP_TCP, NCACN_IP_UDP, NCACN_VNS_IPC, NCACN_VNS_SPP, 
	NCACN_AT_DSP, NCADG_AT_DDP, NCALRPC, NCACN_UNIX_STREAM, NCADG_UNIX_DGRAM,
	NCACN_HTTP, NCADG_IPX, NCACN_SPX };

/*
  this defines a generic security context for signed/sealed dcerpc pipes.
*/
struct dcerpc_connection;
struct dcerpc_security {
	struct dcerpc_auth *auth_info;
	struct gensec_security *generic_state;

	/* get the session key */
	NTSTATUS (*session_key)(struct dcerpc_connection *, DATA_BLOB *);
};

/*
  this holds the information that is not specific to a particular rpc context_id
*/
struct dcerpc_connection {
	uint32_t call_id;
	uint32_t srv_max_xmit_frag;
	uint32_t srv_max_recv_frag;
	uint32_t flags;
	struct dcerpc_security security_state;
	const char *binding_string;
	struct event_context *event_ctx;

	struct dcerpc_transport {
		enum dcerpc_transport_t transport;
		void *private;

		NTSTATUS (*shutdown_pipe)(struct dcerpc_connection *);

		const char *(*peer_name)(struct dcerpc_connection *);

		const char *(*target_hostname)(struct dcerpc_connection *);

		/* send a request to the server */
		NTSTATUS (*send_request)(struct dcerpc_connection *, DATA_BLOB *, BOOL trigger_read);

		/* send a read request to the server */
		NTSTATUS (*send_read)(struct dcerpc_connection *);

		/* a callback to the dcerpc code when a full fragment
		   has been received */
		void (*recv_data)(struct dcerpc_connection *, DATA_BLOB *, NTSTATUS status);
	} transport;

	/* Requests that have been sent, waiting for a reply */
	struct rpc_request *pending;

	/* Sync requests waiting to be shipped */
	struct rpc_request *request_queue;

	/* the next context_id to be assigned */
	uint32_t next_context_id;
};

/*
  this encapsulates a full dcerpc client side pipe 
*/
struct dcerpc_pipe {
	uint32_t context_id;

	struct dcerpc_syntax_id syntax;
	struct dcerpc_syntax_id transfer_syntax;

	struct dcerpc_connection *conn;
	struct dcerpc_binding *binding;

	/* the last fault code from a DCERPC fault */
	uint32_t last_fault_code;

	/* timeout for individual rpc requests, in seconds */
	uint32_t request_timeout;
};

/* default timeout for all rpc requests, in seconds */
#define DCERPC_REQUEST_TIMEOUT 60


/* dcerpc pipe flags */
#define DCERPC_DEBUG_PRINT_IN          (1<<0)
#define DCERPC_DEBUG_PRINT_OUT         (1<<1)
#define DCERPC_DEBUG_PRINT_BOTH (DCERPC_DEBUG_PRINT_IN | DCERPC_DEBUG_PRINT_OUT)

#define DCERPC_DEBUG_VALIDATE_IN       (1<<2)
#define DCERPC_DEBUG_VALIDATE_OUT      (1<<3)
#define DCERPC_DEBUG_VALIDATE_BOTH (DCERPC_DEBUG_VALIDATE_IN | DCERPC_DEBUG_VALIDATE_OUT)

#define DCERPC_CONNECT                 (1<<4)
#define DCERPC_SIGN                    (1<<5)
#define DCERPC_SEAL                    (1<<6)

#define DCERPC_PUSH_BIGENDIAN          (1<<7)
#define DCERPC_PULL_BIGENDIAN          (1<<8)

#define DCERPC_SCHANNEL                (1<<9)

/* use a 128 bit session key */
#define DCERPC_SCHANNEL_128            (1<<12)

/* check incoming pad bytes */
#define DCERPC_DEBUG_PAD_CHECK         (1<<13)

/* set LIBNDR_FLAG_REF_ALLOC flag when decoding NDR */
#define DCERPC_NDR_REF_ALLOC           (1<<14)

#define DCERPC_AUTH_OPTIONS    (DCERPC_SEAL|DCERPC_SIGN|DCERPC_SCHANNEL|DCERPC_AUTH_SPNEGO|DCERPC_AUTH_KRB5|DCERPC_AUTH_NTLM)

/* select spnego auth */
#define DCERPC_AUTH_SPNEGO             (1<<15)

/* select krb5 auth */
#define DCERPC_AUTH_KRB5               (1<<16)

#define DCERPC_SMB2                    (1<<17)

/* select NTLM auth */
#define DCERPC_AUTH_NTLM               (1<<18)

/*
  this is used to find pointers to calls
*/
struct dcerpc_interface_call {
	const char *name;
	size_t struct_size;
	ndr_push_flags_fn_t ndr_push;
	ndr_pull_flags_fn_t ndr_pull;
	ndr_print_function_t ndr_print;
	BOOL async;
};

struct dcerpc_endpoint_list {
	uint32_t count;
	const char * const *names;
};

struct dcerpc_authservice_list {
	uint32_t count;
	const char * const *names;
};

struct dcerpc_interface_table {
	const char *name;
	struct dcerpc_syntax_id syntax_id;
	const char *helpstring;
	uint32_t num_calls;
	const struct dcerpc_interface_call *calls;
	const struct dcerpc_endpoint_list *endpoints;
	const struct dcerpc_authservice_list *authservices;
};

struct dcerpc_interface_list {
	struct dcerpc_interface_list *prev, *next;
	const struct dcerpc_interface_table *table;
};

/* this describes a binding to a particular transport/pipe */
struct dcerpc_binding {
	enum dcerpc_transport_t transport;
	struct dcerpc_syntax_id object;
	const char *host;
	const char *target_hostname;
	const char *endpoint;
	const char **options;
	uint32_t flags;
};


struct dcerpc_pipe_connect {
	struct dcerpc_pipe *pipe;
	struct dcerpc_binding *binding;
	const char *pipe_name;
	const struct dcerpc_interface_table *interface;
	struct cli_credentials *creds;
};


enum rpc_request_state {
	RPC_REQUEST_PENDING,
	RPC_REQUEST_DONE
};

/*
  handle for an async dcerpc request
*/
struct rpc_request {
	struct rpc_request *next, *prev;
	struct dcerpc_pipe *p;
	NTSTATUS status;
	uint32_t call_id;
	enum rpc_request_state state;
	DATA_BLOB payload;
	uint32_t flags;
	uint32_t fault_code;

	/* this is used to distinguish bind and alter_context requests
	   from normal requests */
	void (*recv_handler)(struct rpc_request *conn, 
			     DATA_BLOB *blob, struct ncacn_packet *pkt);

	const struct GUID *object;
	uint16_t opnum;
	DATA_BLOB request_data;
	BOOL async_call;

	/* use by the ndr level async recv call */
	struct {
		const struct dcerpc_interface_table *table;
		uint32_t opnum;
		void *struct_ptr;
		TALLOC_CTX *mem_ctx;
	} ndr;

	struct {
		void (*callback)(struct rpc_request *);
		void *private;
	} async;
};

struct epm_tower;
struct epm_floor;

struct smbcli_tree;
struct smb2_tree;
struct socket_address;

#include "librpc/rpc/dcerpc_proto.h"

#endif /* __DCERPC_H__ */
