/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   SMB2 client tree handling

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

#include "includes.h"
#include "libcli/smb2/smb2.h"
#include "libcli/smb2/smb2_calls.h"

/*
  initialise a smb2_session structure
 */
struct smb2_tree *smb2_tree_init(struct smb2_session *session,
				 TALLOC_CTX *parent_ctx, BOOL primary)
{
	struct smb2_tree *tree;

	tree = talloc_zero(parent_ctx, struct smb2_tree);
	if (!session) {
		return NULL;
	}
	if (primary) {
		tree->session = talloc_steal(tree, session);
	} else {
		tree->session = talloc_reference(tree, session);
	}
	return tree;
}

/*
  send a tree connect
*/
struct smb2_request *smb2_tree_connect_send(struct smb2_tree *tree, 
					    struct smb2_tree_connect *io)
{
	struct smb2_request *req;
	NTSTATUS status;

	req = smb2_request_init(tree->session->transport, SMB2_OP_TCON, 
				0x08, True, 0);
	if (req == NULL) return NULL;

	SBVAL(req->out.hdr,  SMB2_HDR_UID, tree->session->uid);

	SSVAL(req->out.body, 0x02, io->in.unknown1);
	status = smb2_push_o16s16_string(&req->out, 0x04, io->in.path);
	if (!NT_STATUS_IS_OK(status)) {
		talloc_free(req);
		return NULL;
	}

	smb2_transport_send(req);

	return req;
}


/*
  recv a tree connect reply
*/
NTSTATUS smb2_tree_connect_recv(struct smb2_request *req, struct smb2_tree_connect *io)
{
	if (!smb2_request_receive(req) || 
	    smb2_request_is_error(req)) {
		return smb2_request_destroy(req);
	}

	SMB2_CHECK_PACKET_RECV(req, 0x10, False);

	io->out.tid      = IVAL(req->in.hdr,  SMB2_HDR_TID);

	io->out.unknown1    = SVAL(req->in.body, 0x02);
	io->out.unknown2    = IVAL(req->in.body, 0x04);
	io->out.unknown3    = IVAL(req->in.body, 0x08);
	io->out.access_mask = IVAL(req->in.body, 0x0C);
	
	return smb2_request_destroy(req);
}

/*
  sync tree connect request
*/
NTSTATUS smb2_tree_connect(struct smb2_tree *tree, struct smb2_tree_connect *io)
{
	struct smb2_request *req = smb2_tree_connect_send(tree, io);
	return smb2_tree_connect_recv(req, io);
}
