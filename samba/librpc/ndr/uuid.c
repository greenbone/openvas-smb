/*
 * SPDX-FileCopyrightText: Theodore Ts'o               1996, 1997,
 * SPDX-FileCopyrightText: Jim McDonough                     2002.
 * SPDX-FileCopyrightText: Andrew Tridgell                   2003.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   UUID/GUID functions

   Copyright (C) Theodore Ts'o               1996, 1997,
   Copyright (C) Jim McDonough                     2002.
   Copyright (C) Andrew Tridgell                   2003.
   
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

/**
  build a GUID from a string
*/
_PUBLIC_ NTSTATUS GUID_from_string(const char *s, struct GUID *guid)
{
	NTSTATUS status = NT_STATUS_INVALID_PARAMETER;
	uint32_t time_low;
	uint32_t time_mid, time_hi_and_version;
	uint32_t clock_seq[2];
	uint32_t node[6];
	int i;

	if (s == NULL) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (11 == sscanf(s, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			 &time_low, &time_mid, &time_hi_and_version, 
			 &clock_seq[0], &clock_seq[1],
			 &node[0], &node[1], &node[2], &node[3], &node[4], &node[5])) {
	        status = NT_STATUS_OK;
	} else if (11 == sscanf(s, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
				&time_low, &time_mid, &time_hi_and_version, 
				&clock_seq[0], &clock_seq[1],
				&node[0], &node[1], &node[2], &node[3], &node[4], &node[5])) {
		status = NT_STATUS_OK;
	}

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	guid->time_low = time_low;
	guid->time_mid = time_mid;
	guid->time_hi_and_version = time_hi_and_version;
	guid->clock_seq[0] = clock_seq[0];
	guid->clock_seq[1] = clock_seq[1];
	for (i=0;i<6;i++) {
		guid->node[i] = node[i];
	}

	return NT_STATUS_OK;
}

/**
 * generate a random GUID
 */
struct GUID GUID_random(void)
{
	struct GUID guid;

	generate_random_buffer((uint8_t *)&guid, sizeof(guid));
	guid.clock_seq[0] = (guid.clock_seq[0] & 0x3F) | 0x80;
	guid.time_hi_and_version = (guid.time_hi_and_version & 0x0FFF) | 0x4000;

	return guid;
}

/**
 * generate an empty GUID 
 */
_PUBLIC_ struct GUID GUID_zero(void)
{
	struct GUID guid;

	ZERO_STRUCT(guid);

	return guid;
}

_PUBLIC_ BOOL GUID_all_zero(const struct GUID *u)
{
	if (u->time_low != 0 ||
	    u->time_mid != 0 ||
	    u->time_hi_and_version != 0 ||
	    u->clock_seq[0] != 0 ||
	    u->clock_seq[1] != 0 ||
	    !all_zero(u->node, 6)) {
		return False;
	}
	return True;
}

_PUBLIC_ BOOL GUID_equal(const struct GUID *u1, const struct GUID *u2)
{
	if (u1->time_low != u2->time_low ||
	    u1->time_mid != u2->time_mid ||
	    u1->time_hi_and_version != u2->time_hi_and_version ||
	    u1->clock_seq[0] != u2->clock_seq[0] ||
	    u1->clock_seq[1] != u2->clock_seq[1] ||
	    memcmp(u1->node, u2->node, 6) != 0) {
		return False;
	}
	return True;
}

/**
  its useful to be able to display these in debugging messages
*/
_PUBLIC_ char *GUID_string(TALLOC_CTX *mem_ctx, const struct GUID *guid)
{
	return talloc_asprintf(mem_ctx, 
			       "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			       guid->time_low, guid->time_mid,
			       guid->time_hi_and_version,
			       guid->clock_seq[0],
			       guid->clock_seq[1],
			       guid->node[0], guid->node[1],
			       guid->node[2], guid->node[3],
			       guid->node[4], guid->node[5]);
}

_PUBLIC_ char *GUID_string2(TALLOC_CTX *mem_ctx, const struct GUID *guid)
{
	char *ret, *s = GUID_string(mem_ctx, guid);
	ret = talloc_asprintf(mem_ctx, "{%s}", s);
	talloc_free(s);
	return ret;
}

_PUBLIC_ BOOL policy_handle_empty(struct policy_handle *h) 
{
	return (h->handle_type == 0 && GUID_all_zero(&h->uuid));
}
