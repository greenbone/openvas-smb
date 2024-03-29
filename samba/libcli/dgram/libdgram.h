/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
   Unix SMB/CIFS implementation.

   a raw async NBT DGRAM library

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

#include "librpc/gen_ndr/nbt.h"

/*
  a datagram name request
*/
struct nbt_dgram_request {
	struct nbt_dgram_request *next, *prev;

	/* where to send the request */
	struct socket_address *dest;

	/* the encoded request */
	DATA_BLOB encoded;
};

/*
  context structure for operations on dgram packets
*/
struct nbt_dgram_socket {
	struct socket_context *sock;
	struct event_context *event_ctx;

	/* the fd event */
	struct fd_event *fde;

	/* a queue of outgoing requests */
	struct nbt_dgram_request *send_queue;

	/* a list of mailslot handlers */
	struct dgram_mailslot_handler *mailslot_handlers;

	/* what to do with incoming request packets */
	struct {
		void (*handler)(struct nbt_dgram_socket *, struct nbt_dgram_packet *, 
				struct socket_address *src);
		void *private;
	} incoming;
};


/*
  the mailslot code keeps a list of mailslot handlers. A mailslot
  handler is a function that receives incoming packets for a specific
  mailslot name. When a caller needs to send a mailslot and wants to
  get a reply then it needs to register itself as listening for
  incoming packets on the reply mailslot
*/

typedef void (*dgram_mailslot_handler_t)(struct dgram_mailslot_handler *, 
					 struct nbt_dgram_packet *, 
					 struct socket_address *src);

struct dgram_mailslot_handler {
	struct dgram_mailslot_handler *next, *prev;

	struct nbt_dgram_socket *dgmsock;
	const char *mailslot_name;

	dgram_mailslot_handler_t handler;
	void *private;
};


/* prototypes */
NTSTATUS nbt_dgram_send(struct nbt_dgram_socket *dgmsock,
			struct nbt_dgram_packet *packet,
			struct socket_address *dest);
NTSTATUS dgram_set_incoming_handler(struct nbt_dgram_socket *dgmsock,
				    void (*handler)(struct nbt_dgram_socket *, 
						    struct nbt_dgram_packet *, 
						    struct socket_address *),
				    void *private);
struct nbt_dgram_socket *nbt_dgram_socket_init(TALLOC_CTX *mem_ctx, 
					       struct event_context *event_ctx);

const char *dgram_mailslot_name(struct nbt_dgram_packet *packet);
struct dgram_mailslot_handler *dgram_mailslot_find(struct nbt_dgram_socket *dgmsock,
						   const char *mailslot_name);
struct dgram_mailslot_handler *dgram_mailslot_listen(struct nbt_dgram_socket *dgmsock,
						     const char *mailslot_name,
						     dgram_mailslot_handler_t handler,
						     void *private);
struct dgram_mailslot_handler *dgram_mailslot_temp(struct nbt_dgram_socket *dgmsock,
						   const char *mailslot_name,
						   dgram_mailslot_handler_t handler,
						   void *private);
DATA_BLOB dgram_mailslot_data(struct nbt_dgram_packet *dgram);


NTSTATUS dgram_mailslot_send(struct nbt_dgram_socket *dgmsock,
			     enum dgram_msg_type msg_type,
			     const char *mailslot_name,
			     struct nbt_name *dest_name,
			     struct socket_address *dest,
			     struct nbt_name *src_name,
			     DATA_BLOB *request);

NTSTATUS dgram_mailslot_netlogon_send(struct nbt_dgram_socket *dgmsock,
				      struct nbt_name *dest_name,
				      struct socket_address *dest,
				      struct nbt_name *src_name,
				      struct nbt_netlogon_packet *request);
NTSTATUS dgram_mailslot_netlogon_reply(struct nbt_dgram_socket *dgmsock,
				       struct nbt_dgram_packet *request,
				       const char *mailslot_name,
				       struct nbt_netlogon_packet *reply);
NTSTATUS dgram_mailslot_netlogon_parse(struct dgram_mailslot_handler *dgmslot,
				       TALLOC_CTX *mem_ctx,
				       struct nbt_dgram_packet *dgram,
				       struct nbt_netlogon_packet *netlogon);

NTSTATUS dgram_mailslot_ntlogon_send(struct nbt_dgram_socket *dgmsock,
				     enum dgram_msg_type msg_type,
				     struct nbt_name *dest_name,
				     struct socket_address *dest,
				     struct nbt_name *src_name,
				     struct nbt_ntlogon_packet *request);
NTSTATUS dgram_mailslot_ntlogon_reply(struct nbt_dgram_socket *dgmsock,
				       struct nbt_dgram_packet *request,
				       const char *mailslot_name,
				       struct nbt_ntlogon_packet *reply);
NTSTATUS dgram_mailslot_ntlogon_parse(struct dgram_mailslot_handler *dgmslot,
				       TALLOC_CTX *mem_ctx,
				       struct nbt_dgram_packet *dgram,
				       struct nbt_ntlogon_packet *ntlogon);

NTSTATUS dgram_mailslot_browse_send(struct nbt_dgram_socket *dgmsock,
				    struct nbt_name *dest_name,
				    struct socket_address *dest,
				    struct nbt_name *src_name,
				    struct nbt_browse_packet *request);

NTSTATUS dgram_mailslot_browse_reply(struct nbt_dgram_socket *dgmsock,
				     struct nbt_dgram_packet *request,
				     const char *mailslot_name,
				     struct nbt_browse_packet *reply);

NTSTATUS dgram_mailslot_browse_parse(struct dgram_mailslot_handler *dgmslot,
				     TALLOC_CTX *mem_ctx,
				     struct nbt_dgram_packet *dgram,
				     struct nbt_browse_packet *pkt);
