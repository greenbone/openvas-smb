/*
 * SPDX-FileCopyrightText: Stefan Metzmacher	2005
 * SPDX-FileCopyrightText: Andrew Tridgell	2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   SMB2 client request handling

   Copyright (C) Andrew Tridgell	2005
   Copyright (C) Stefan Metzmacher	2005
   
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
#include "libcli/raw/libcliraw.h"
#include "libcli/smb2/smb2.h"
#include "lib/util/dlinklist.h"
#include "lib/events/events.h"

/*
  initialise a smb2 request
*/
struct smb2_request *smb2_request_init(struct smb2_transport *transport, uint16_t opcode,
				       uint16_t body_fixed_size, BOOL body_dynamic_present,
				       uint32_t body_dynamic_size)
{
	struct smb2_request *req;
	uint64_t seqnum;

	if (body_dynamic_present) {
		if (body_dynamic_size == 0) {
			body_dynamic_size = 1;
		}
	} else {
		body_dynamic_size = 0;
	}

	req = talloc(transport, struct smb2_request);
	if (req == NULL) return NULL;

	seqnum = transport->seqnum++;
	if (seqnum == UINT64_MAX) {
		seqnum = transport->seqnum++;
	}

	req->state     = SMB2_REQUEST_INIT;
	req->transport = transport;
	req->session   = NULL;
	req->tree      = NULL;
	req->seqnum    = seqnum;
	req->status    = NT_STATUS_OK;
	req->async.fn  = NULL;
	req->next = req->prev = NULL;

	ZERO_STRUCT(req->cancel);
	ZERO_STRUCT(req->in);

	req->out.size      = SMB2_HDR_BODY+NBT_HDR_SIZE+body_fixed_size;

	req->out.allocated = req->out.size + body_dynamic_size;
	req->out.buffer    = talloc_size(req, req->out.allocated);
	if (req->out.buffer == NULL) {
		talloc_free(req);
		return NULL;
	}

	req->out.hdr       = req->out.buffer + NBT_HDR_SIZE;
	req->out.body      = req->out.hdr + SMB2_HDR_BODY;
	req->out.body_fixed= body_fixed_size;
	req->out.body_size = body_fixed_size;
	req->out.dynamic   = (body_dynamic_size ? req->out.body + body_fixed_size : NULL);

	SIVAL(req->out.hdr, 0,                SMB2_MAGIC);
	SSVAL(req->out.hdr, SMB2_HDR_LENGTH,  SMB2_HDR_BODY);
	SSVAL(req->out.hdr, SMB2_HDR_PAD1,    0);
	SIVAL(req->out.hdr, SMB2_HDR_STATUS,  0);
	SSVAL(req->out.hdr, SMB2_HDR_OPCODE,  opcode);
	SSVAL(req->out.hdr, SMB2_HDR_UNKNOWN1,0);
	SIVAL(req->out.hdr, SMB2_HDR_FLAGS,   0);
	SIVAL(req->out.hdr, SMB2_HDR_UNKNOWN2,0);
	SBVAL(req->out.hdr, SMB2_HDR_SEQNUM,  req->seqnum);
	SIVAL(req->out.hdr, SMB2_HDR_PID,     0);
	SIVAL(req->out.hdr, SMB2_HDR_TID,     0);
	SBVAL(req->out.hdr, SMB2_HDR_UID,     0);
	memset(req->out.hdr+SMB2_HDR_SIG, 0, 16);

	/* set the length of the fixed body part and +1 if there's a dynamic part also */
	SSVAL(req->out.body, 0, body_fixed_size + (body_dynamic_size?1:0));

	/* 
	 * if we have a dynamic part, make sure the first byte
	 * which is always be part of the packet is initialized
	 */
	if (body_dynamic_size) {
		req->out.size += 1;
		SCVAL(req->out.dynamic, 0, 0);
	}

	return req;
}

/*
    initialise a smb2 request for tree operations
*/
struct smb2_request *smb2_request_init_tree(struct smb2_tree *tree, uint16_t opcode,
					    uint16_t body_fixed_size, BOOL body_dynamic_present,
					    uint32_t body_dynamic_size)
{
	struct smb2_request *req = smb2_request_init(tree->session->transport, opcode, 
						     body_fixed_size, body_dynamic_present,
						     body_dynamic_size);
	if (req == NULL) return NULL;

	SBVAL(req->out.hdr,  SMB2_HDR_UID, tree->session->uid);
	SIVAL(req->out.hdr,  SMB2_HDR_TID, tree->tid);
	req->session = tree->session;
	req->tree = tree;

	return req;	
}

/* destroy a request structure and return final status */
NTSTATUS smb2_request_destroy(struct smb2_request *req)
{
	NTSTATUS status;

	/* this is the error code we give the application for when a
	   _send() call fails completely */
	if (!req) return NT_STATUS_UNSUCCESSFUL;

	if (req->transport) {
		/* remove it from the list of pending requests (a null op if
		   its not in the list) */
		DLIST_REMOVE(req->transport->pending_recv, req);
	}

	if (req->state == SMB2_REQUEST_ERROR &&
	    NT_STATUS_IS_OK(req->status)) {
		req->status = NT_STATUS_INTERNAL_ERROR;
	}

	status = req->status;
	talloc_free(req);
	return status;
}

/*
  receive a response to a packet
*/
BOOL smb2_request_receive(struct smb2_request *req)
{
	/* req can be NULL when a send has failed. This eliminates lots of NULL
	   checks in each module */
	if (!req) return False;

	/* keep receiving packets until this one is replied to */
	while (req->state <= SMB2_REQUEST_RECV) {
		if (event_loop_once(req->transport->socket->event.ctx) != 0) {
			return False;
		}
	}

	return req->state == SMB2_REQUEST_DONE;
}

/* Return true if the last packet was in error */
BOOL smb2_request_is_error(struct smb2_request *req)
{
	return NT_STATUS_IS_ERR(req->status);
}

/* Return true if the last packet was OK */
BOOL smb2_request_is_ok(struct smb2_request *req)
{
	return NT_STATUS_IS_OK(req->status);
}

/*
  check if a range in the reply body is out of bounds
*/
BOOL smb2_oob(struct smb2_request_buffer *buf, const uint8_t *ptr, size_t size)
{
	/* be careful with wraparound! */
	if (ptr < buf->body ||
	    ptr >= buf->body + buf->body_size ||
	    size > buf->body_size ||
	    ptr + size > buf->body + buf->body_size) {
		return True;
	}
	return False;
}

size_t smb2_padding_size(uint32_t offset, size_t n)
{
	if ((offset & (n-1)) == 0) return 0;
	return n - (offset & (n-1));
}

static size_t smb2_padding_fix(struct smb2_request_buffer *buf)
{
	if (buf->dynamic == (buf->body + buf->body_fixed)) {
		return 1;
	}
	return 0;
}

/*
  grow a SMB2 buffer by the specified amount
*/
static NTSTATUS smb2_grow_buffer(struct smb2_request_buffer *buf, size_t increase)
{
	size_t dynamic_ofs;
	uint8_t *buffer_ptr;
	uint32_t newsize = buf->size + increase;

	/* a packet size should be limited a bit */
	if (newsize >= 0x00FFFFFF) return NT_STATUS_MARSHALL_OVERFLOW;

	if (newsize <= buf->allocated) return NT_STATUS_OK;

	dynamic_ofs = buf->dynamic - buf->buffer;

	buffer_ptr = talloc_realloc_size(buf, buf->buffer, newsize);
	NT_STATUS_HAVE_NO_MEMORY(buffer_ptr);

	buf->buffer	= buffer_ptr;
	buf->hdr	= buf->buffer + NBT_HDR_SIZE;
	buf->body	= buf->hdr    + SMB2_HDR_BODY;
	buf->dynamic	= buf->buffer + dynamic_ofs;
	buf->allocated	= newsize;

	return NT_STATUS_OK;
}

/*
  pull a uint16_t ofs/ uint16_t length/blob triple from a data blob
  the ptr points to the start of the offset/length pair
*/
NTSTATUS smb2_pull_o16s16_blob(struct smb2_request_buffer *buf, TALLOC_CTX *mem_ctx, uint8_t *ptr, DATA_BLOB *blob)
{
	uint16_t ofs, size;
	if (smb2_oob(buf, ptr, 4)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	ofs  = SVAL(ptr, 0);
	size = SVAL(ptr, 2);
	if (ofs == 0 || size == 0) {
		*blob = data_blob(NULL, 0);
		return NT_STATUS_OK;
	}
	if (smb2_oob(buf, buf->hdr + ofs, size)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	*blob = data_blob_talloc(mem_ctx, buf->hdr + ofs, size);
	NT_STATUS_HAVE_NO_MEMORY(blob->data);
	return NT_STATUS_OK;
}

/*
  push a uint16_t ofs/ uint16_t length/blob triple into a data blob
  the ofs points to the start of the offset/length pair, and is relative
  to the body start
*/
NTSTATUS smb2_push_o16s16_blob(struct smb2_request_buffer *buf, 
			       uint16_t ofs, DATA_BLOB blob)
{
	NTSTATUS status;
	size_t offset;
	size_t padding_length;
	size_t padding_fix;
	uint8_t *ptr = buf->body+ofs;

	if (buf->dynamic == NULL) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* we have only 16 bit for the size */
	if (blob.length > 0xFFFF) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	/* check if there're enough room for ofs and size */
	if (smb2_oob(buf, ptr, 4)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	if (blob.length == 0) {
		SSVAL(ptr, 0, 0);
		SSVAL(ptr, 2, 0);
		return NT_STATUS_OK;
	}

	offset = buf->dynamic - buf->hdr;
	padding_length = smb2_padding_size(offset, 2);
	offset += padding_length;
	padding_fix = smb2_padding_fix(buf);

	SSVAL(ptr, 0, offset);
	SSVAL(ptr, 2, blob.length);

	status = smb2_grow_buffer(buf, blob.length + padding_length - padding_fix);
	NT_STATUS_NOT_OK_RETURN(status);

	memset(buf->dynamic, 0, padding_length);
	buf->dynamic += padding_length;

	memcpy(buf->dynamic, blob.data, blob.length);
	buf->dynamic += blob.length;

	buf->size += blob.length + padding_length - padding_fix;
	buf->body_size += blob.length + padding_length;

	return NT_STATUS_OK;
}


/*
  push a uint16_t ofs/ uint32_t length/blob triple into a data blob
  the ofs points to the start of the offset/length pair, and is relative
  to the body start
*/
NTSTATUS smb2_push_o16s32_blob(struct smb2_request_buffer *buf, 
			       uint16_t ofs, DATA_BLOB blob)
{
	NTSTATUS status;
	size_t offset;
	size_t padding_length;
	size_t padding_fix;
	uint8_t *ptr = buf->body+ofs;

	if (buf->dynamic == NULL) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* check if there're enough room for ofs and size */
	if (smb2_oob(buf, ptr, 6)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	if (blob.length == 0) {
		SSVAL(ptr, 0, 0);
		SIVAL(ptr, 2, 0);
		return NT_STATUS_OK;
	}

	offset = buf->dynamic - buf->hdr;
	padding_length = smb2_padding_size(offset, 2);
	offset += padding_length;
	padding_fix = smb2_padding_fix(buf);

	SSVAL(ptr, 0, offset);
	SIVAL(ptr, 2, blob.length);

	status = smb2_grow_buffer(buf, blob.length + padding_length - padding_fix);
	NT_STATUS_NOT_OK_RETURN(status);

	memset(buf->dynamic, 0, padding_length);
	buf->dynamic += padding_length;

	memcpy(buf->dynamic, blob.data, blob.length);
	buf->dynamic += blob.length;

	buf->size += blob.length + padding_length - padding_fix;
	buf->body_size += blob.length + padding_length;

	return NT_STATUS_OK;
}


/*
  push a uint32_t ofs/ uint32_t length/blob triple into a data blob
  the ofs points to the start of the offset/length pair, and is relative
  to the body start
*/
NTSTATUS smb2_push_o32s32_blob(struct smb2_request_buffer *buf, 
			       uint32_t ofs, DATA_BLOB blob)
{
	NTSTATUS status;
	size_t offset;
	size_t padding_length;
	size_t padding_fix;
	uint8_t *ptr = buf->body+ofs;

	if (buf->dynamic == NULL) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* check if there're enough room for ofs and size */
	if (smb2_oob(buf, ptr, 8)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	if (blob.length == 0) {
		SIVAL(ptr, 0, 0);
		SIVAL(ptr, 4, 0);
		return NT_STATUS_OK;
	}

	offset = buf->dynamic - buf->hdr;
	padding_length = smb2_padding_size(offset, 8);
	offset += padding_length;
	padding_fix = smb2_padding_fix(buf);

	SIVAL(ptr, 0, offset);
	SIVAL(ptr, 4, blob.length);

	status = smb2_grow_buffer(buf, blob.length + padding_length - padding_fix);
	NT_STATUS_NOT_OK_RETURN(status);

	memset(buf->dynamic, 0, padding_length);
	buf->dynamic += padding_length;

	memcpy(buf->dynamic, blob.data, blob.length);
	buf->dynamic += blob.length;

	buf->size += blob.length + padding_length - padding_fix;
	buf->body_size += blob.length + padding_length;

	return NT_STATUS_OK;
}


/*
  push a uint32_t length/ uint32_t ofs/blob triple into a data blob
  the ofs points to the start of the length/offset pair, and is relative
  to the body start
*/
NTSTATUS smb2_push_s32o32_blob(struct smb2_request_buffer *buf, 
			       uint32_t ofs, DATA_BLOB blob)
{
	NTSTATUS status;
	size_t offset;
	size_t padding_length;
	size_t padding_fix;
	uint8_t *ptr = buf->body+ofs;

	if (buf->dynamic == NULL) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* check if there're enough room for ofs and size */
	if (smb2_oob(buf, ptr, 8)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}

	if (blob.length == 0) {
		SIVAL(ptr, 0, 0);
		SIVAL(ptr, 4, 0);
		return NT_STATUS_OK;
	}

	offset = buf->dynamic - buf->hdr;
	padding_length = smb2_padding_size(offset, 8);
	offset += padding_length;
	padding_fix = smb2_padding_fix(buf);

	SIVAL(ptr, 0, blob.length);
	SIVAL(ptr, 4, offset);

	status = smb2_grow_buffer(buf, blob.length + padding_length - padding_fix);
	NT_STATUS_NOT_OK_RETURN(status);

	memset(buf->dynamic, 0, padding_length);
	buf->dynamic += padding_length;

	memcpy(buf->dynamic, blob.data, blob.length);
	buf->dynamic += blob.length;

	buf->size += blob.length + padding_length - padding_fix;
	buf->body_size += blob.length + padding_length;

	return NT_STATUS_OK;
}

/*
  pull a uint16_t ofs/ uint32_t length/blob triple from a data blob
  the ptr points to the start of the offset/length pair
*/
NTSTATUS smb2_pull_o16s32_blob(struct smb2_request_buffer *buf, TALLOC_CTX *mem_ctx, uint8_t *ptr, DATA_BLOB *blob)
{
	uint16_t ofs;
	uint32_t size;

	if (smb2_oob(buf, ptr, 6)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	ofs  = SVAL(ptr, 0);
	size = IVAL(ptr, 2);
	if (ofs == 0 || size == 0) {
		*blob = data_blob(NULL, 0);
		return NT_STATUS_OK;
	}
	if (smb2_oob(buf, buf->hdr + ofs, size)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	*blob = data_blob_talloc(mem_ctx, buf->hdr + ofs, size);
	NT_STATUS_HAVE_NO_MEMORY(blob->data);
	return NT_STATUS_OK;
}

/*
  pull a uint32_t ofs/ uint32_t length/blob triple from a data blob
  the ptr points to the start of the offset/length pair
*/
NTSTATUS smb2_pull_o32s32_blob(struct smb2_request_buffer *buf, TALLOC_CTX *mem_ctx, uint8_t *ptr, DATA_BLOB *blob)
{
	uint32_t ofs, size;
	if (smb2_oob(buf, ptr, 8)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	ofs  = IVAL(ptr, 0);
	size = IVAL(ptr, 4);
	if (ofs == 0 || size == 0) {
		*blob = data_blob(NULL, 0);
		return NT_STATUS_OK;
	}
	if (smb2_oob(buf, buf->hdr + ofs, size)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	*blob = data_blob_talloc(mem_ctx, buf->hdr + ofs, size);
	NT_STATUS_HAVE_NO_MEMORY(blob->data);
	return NT_STATUS_OK;
}

/*
  pull a uint32_t length/ uint32_t ofs/blob triple from a data blob
  the ptr points to the start of the offset/length pair
*/
NTSTATUS smb2_pull_s32o32_blob(struct smb2_request_buffer *buf, TALLOC_CTX *mem_ctx, uint8_t *ptr, DATA_BLOB *blob)
{
	uint32_t ofs, size;
	if (smb2_oob(buf, ptr, 8)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	size = IVAL(ptr, 0);
	ofs  = IVAL(ptr, 4);
	if (ofs == 0 || size == 0) {
		*blob = data_blob(NULL, 0);
		return NT_STATUS_OK;
	}
	if (smb2_oob(buf, buf->hdr + ofs, size)) {
		return NT_STATUS_BUFFER_TOO_SMALL;
	}
	*blob = data_blob_talloc(mem_ctx, buf->hdr + ofs, size);
	NT_STATUS_HAVE_NO_MEMORY(blob->data);
	return NT_STATUS_OK;
}

/*
  pull a string in a uint16_t ofs/ uint16_t length/blob format
  UTF-16 without termination
*/
NTSTATUS smb2_pull_o16s16_string(struct smb2_request_buffer *buf, TALLOC_CTX *mem_ctx,
				 uint8_t *ptr, const char **str)
{
	DATA_BLOB blob;
	NTSTATUS status;
	ssize_t size;
	void *vstr;

	status = smb2_pull_o16s16_blob(buf, mem_ctx, ptr, &blob);
	NT_STATUS_NOT_OK_RETURN(status);

	if (blob.length == 0) {
		char *s;
		s = talloc_strdup(mem_ctx, "");
		NT_STATUS_HAVE_NO_MEMORY(s);
		*str = s;
		return NT_STATUS_OK;
	}

	size = convert_string_talloc(mem_ctx, CH_UTF16, CH_UNIX, 
				     blob.data, blob.length, &vstr);
	data_blob_free(&blob);
	(*str) = vstr;
	if (size == -1) {
		return NT_STATUS_ILLEGAL_CHARACTER;
	}
	return NT_STATUS_OK;
}

/*
  push a string in a uint16_t ofs/ uint16_t length/blob format
  UTF-16 without termination
*/
NTSTATUS smb2_push_o16s16_string(struct smb2_request_buffer *buf,
				 uint16_t ofs, const char *str)
{
	DATA_BLOB blob;
	NTSTATUS status;
	ssize_t size;

	if (strcmp("", str) == 0) {
		return smb2_push_o16s16_blob(buf, ofs, data_blob(NULL, 0));
	}

	size = convert_string_talloc(buf->buffer, CH_UNIX, CH_UTF16, 
				     str, strlen(str), (void **)&blob.data);
	if (size == -1) {
		return NT_STATUS_ILLEGAL_CHARACTER;
	}
	blob.length = size;

	status = smb2_push_o16s16_blob(buf, ofs, blob);
	data_blob_free(&blob);
	return status;
}

/*
  push a file handle into a buffer
*/
void smb2_push_handle(uint8_t *data, struct smb2_handle *h)
{
	SBVAL(data, 0, h->data[0]);
	SBVAL(data, 8, h->data[1]);
}

/*
  pull a file handle from a buffer
*/
void smb2_pull_handle(uint8_t *ptr, struct smb2_handle *h)
{
	h->data[0] = BVAL(ptr, 0);
	h->data[1] = BVAL(ptr, 8);
}
