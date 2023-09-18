/*
 * SPDX-FileCopyrightText: Jelmer Vernooij					  2004-2005.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   COM standard objects
   Copyright (C) Jelmer Vernooij					  2004-2005.
   
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

#ifndef _DCOM_H /* _DCOM_H */
#define _DCOM_H 

struct cli_credentials;
struct dcerpc_pipe;
struct IRemUnknown;
struct rpc_request;

#include "lib/com/com.h"
#include "librpc/gen_ndr/orpc.h"

#define NT_STATUS_RPC_NT_CALL_FAILED	NT_STATUS(0xC002001BL)

typedef NTSTATUS (*marshal_fn)(struct IUnknown *pv, struct OBJREF *o);
typedef NTSTATUS (*unmarshal_fn)(struct OBJREF *o, struct IUnknown **pv);

#include "lib/com/dcom/proto.h"

struct dcom_client_context {
	struct dcom_server_credentials {
		const char *server;
		struct cli_credentials *credentials;
		struct dcom_server_credentials *prev, *next;
	} *credentials;
	struct dcom_object_exporter {
		uint64_t oxid;
		char *host;
		struct IRemUnknown *rem_unknown;
		struct DUALSTRINGARRAY *bindings;
		struct dcerpc_pipe *pipe;
		struct dcom_object_exporter *prev, *next;
	} *object_exporters;
};

struct dcom_proxy_async_call_state {
        struct IUnknown *d;
        const struct dcerpc_interface_table *table;
        uint32_t opnum;
        void (*continuation)(struct rpc_request *);
        TALLOC_CTX *mem_ctx;
        void *r;
};

#define WERR_NDR_CHECK(call) do { NTSTATUS _status; \
				_status = call; \
				if (!NT_STATUS_IS_OK(_status)) \
				return ntstatus_to_werror(_status); \
			    } while (0)

#define IUnknown_ipid(d) ((d)->obj.u_objref.u_standard.std.ipid)

#endif /* _DCOM_H */
