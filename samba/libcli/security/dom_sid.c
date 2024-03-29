/*
 * SPDX-FileCopyrightText: Jeremy Allison  		1999
 * SPDX-FileCopyrightText: Andrew Tridgell 		1992-2004
 * SPDX-FileCopyrightText: Stefan (metze) Metzmacher 	2002-2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   Samba utility functions

   Copyright (C) Stefan (metze) Metzmacher 	2002-2004
   Copyright (C) Andrew Tridgell 		1992-2004
   Copyright (C) Jeremy Allison  		1999
      
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
#include "librpc/gen_ndr/security.h"

/*****************************************************************
 Compare the auth portion of two sids.
*****************************************************************/  

static int dom_sid_compare_auth(const struct dom_sid *sid1, const struct dom_sid *sid2)
{
	int i;

	if (sid1 == sid2)
		return 0;
	if (!sid1)
		return -1;
	if (!sid2)
		return 1;

	if (sid1->sid_rev_num != sid2->sid_rev_num)
		return sid1->sid_rev_num - sid2->sid_rev_num;

	for (i = 0; i < 6; i++)
		if (sid1->id_auth[i] != sid2->id_auth[i])
			return sid1->id_auth[i] - sid2->id_auth[i];

	return 0;
}

/*****************************************************************
 Compare two sids.
*****************************************************************/  

static int dom_sid_compare(const struct dom_sid *sid1, const struct dom_sid *sid2)
{
	int i;

	if (sid1 == sid2)
		return 0;
	if (!sid1)
		return -1;
	if (!sid2)
		return 1;

	/* Compare most likely different rids, first: i.e start at end */
	if (sid1->num_auths != sid2->num_auths)
		return sid1->num_auths - sid2->num_auths;

	for (i = sid1->num_auths-1; i >= 0; --i)
		if (sid1->sub_auths[i] != sid2->sub_auths[i])
			return sid1->sub_auths[i] - sid2->sub_auths[i];

	return dom_sid_compare_auth(sid1, sid2);
}

/*****************************************************************
 Compare two sids.
*****************************************************************/  

BOOL dom_sid_equal(const struct dom_sid *sid1, const struct dom_sid *sid2)
{
	return dom_sid_compare(sid1, sid2) == 0;
}


/*
  convert a string to a dom_sid, returning a talloc'd dom_sid
*/
struct dom_sid *dom_sid_parse_talloc(TALLOC_CTX *mem_ctx, const char *sidstr)
{
	struct dom_sid *ret;
	uint_t rev, ia, num_sub_auths, i;
	char *p;
  
	if (strncasecmp(sidstr, "S-", 2)) {
		return NULL;
	}

	sidstr += 2;

	rev = strtol(sidstr, &p, 10);
	if (*p != '-') {
		return NULL;
	}
	sidstr = p+1;

	ia = strtol(sidstr, &p, 10);
	if (p == sidstr) {
		return NULL;
	}
	sidstr = p;

	num_sub_auths = 0;
	for (i=0;sidstr[i];i++) {
		if (sidstr[i] == '-') num_sub_auths++;
	}

	ret = talloc(mem_ctx, struct dom_sid);
	if (!ret) {
		return NULL;
	}

	ret->sub_auths = talloc_array(ret, uint32_t, num_sub_auths);
	if (!ret->sub_auths) {
		return NULL;
	}

	ret->sid_rev_num = rev;
	ret->id_auth[0] = 0;
	ret->id_auth[1] = 0;
	ret->id_auth[2] = ia >> 24;
	ret->id_auth[3] = ia >> 16;
	ret->id_auth[4] = ia >> 8;
	ret->id_auth[5] = ia;
	ret->num_auths = num_sub_auths;

	for (i=0;i<num_sub_auths;i++) {
		if (sidstr[0] != '-') {
			return NULL;
		}
		sidstr++;
		ret->sub_auths[i] = strtoul(sidstr, &p, 10);
		if (p == sidstr) {
			return NULL;
		}
		sidstr = p;
	}

	return ret;
}

/*
  copy a dom_sid structure
*/
struct dom_sid *dom_sid_dup(TALLOC_CTX *mem_ctx, const struct dom_sid *dom_sid)
{
	struct dom_sid *ret;
	int i;
	
	if (!dom_sid) {
		return NULL;
	}

	ret = talloc(mem_ctx, struct dom_sid);
	if (!ret) {
		return NULL;
	}

	ret->sub_auths = talloc_array(ret, uint32_t, dom_sid->num_auths);
	if (!ret->sub_auths) {
		return NULL;
	}

	ret->sid_rev_num = dom_sid->sid_rev_num;
	ret->id_auth[0] = dom_sid->id_auth[0];
	ret->id_auth[1] = dom_sid->id_auth[1];
	ret->id_auth[2] = dom_sid->id_auth[2];
	ret->id_auth[3] = dom_sid->id_auth[3];
	ret->id_auth[4] = dom_sid->id_auth[4];
	ret->id_auth[5] = dom_sid->id_auth[5];
	ret->num_auths = dom_sid->num_auths;

	for (i=0;i<dom_sid->num_auths;i++) {
		ret->sub_auths[i] = dom_sid->sub_auths[i];
	}

	return ret;
}

/*
  add a rid to a domain dom_sid to make a full dom_sid. This function
  returns a new sid in the suppplied memory context
*/
struct dom_sid *dom_sid_add_rid(TALLOC_CTX *mem_ctx, 
				const struct dom_sid *domain_sid, 
				uint32_t rid)
{
	struct dom_sid *sid;

	sid = talloc(mem_ctx, struct dom_sid);
	if (!sid) return NULL;

	*sid = *domain_sid;

	sid->sub_auths = talloc_array(sid, uint32_t, sid->num_auths+1);
	if (!sid->sub_auths) {
		return NULL;
	}
	memcpy(sid->sub_auths, domain_sid->sub_auths, sid->num_auths*sizeof(uint32_t));
	sid->sub_auths[sid->num_auths] = rid;
	sid->num_auths++;

	return sid;
}

/*
  Split up a SID into its domain and RID part
*/
NTSTATUS dom_sid_split_rid(TALLOC_CTX *mem_ctx, const struct dom_sid *sid,
			   struct dom_sid **domain, uint32_t *rid)
{
	if (sid->num_auths == 0) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (!(*domain = dom_sid_dup(mem_ctx, sid))) {
		return NT_STATUS_NO_MEMORY;
	}

	(*domain)->num_auths -= 1;
	*rid = (*domain)->sub_auths[(*domain)->num_auths];
	return NT_STATUS_OK;
}

/*
  return True if the 2nd sid is in the domain given by the first sid
*/
BOOL dom_sid_in_domain(const struct dom_sid *domain_sid, 
		       const struct dom_sid *sid)
{
	int i;

	if (!domain_sid || !sid) {
		return False;
	}

	if (domain_sid->num_auths > sid->num_auths) {
		return False;
	}

	for (i = domain_sid->num_auths-1; i >= 0; --i) {
		if (domain_sid->sub_auths[i] != sid->sub_auths[i]) {
			return False;
		}
	}

	return dom_sid_compare_auth(domain_sid, sid) == 0;
}

/*
  convert a dom_sid to a string
*/
char *dom_sid_string(TALLOC_CTX *mem_ctx, const struct dom_sid *sid)
{
	int i, ofs, maxlen;
	uint32_t ia;
	char *ret;
	
	if (!sid) {
		return talloc_strdup(mem_ctx, "(NULL SID)");
	}

	maxlen = sid->num_auths * 11 + 25;
	ret = talloc_size(mem_ctx, maxlen);
	if (!ret) return talloc_strdup(mem_ctx, "(SID ERR)");

	ia = (sid->id_auth[5]) +
		(sid->id_auth[4] << 8 ) +
		(sid->id_auth[3] << 16) +
		(sid->id_auth[2] << 24);

	ofs = snprintf(ret, maxlen, "S-%u-%lu", 
		       (unsigned int)sid->sid_rev_num, (unsigned long)ia);
	if (ofs < 0 || ofs >= maxlen) {
			talloc_free(ret);
			return talloc_strdup(mem_ctx, "(SID ERR)");
	}


	for (i = 0; i < sid->num_auths; i++) {
		int n = snprintf(ret + ofs, maxlen - ofs, "-%lu", (unsigned long)sid->sub_auths[i]);
		if (n < 0 || n >= maxlen - ofs) {
			talloc_free(ret);
			return talloc_strdup(mem_ctx, "(SID ERR)");
		}
		ofs += n;
	}
	
	return ret;
}
