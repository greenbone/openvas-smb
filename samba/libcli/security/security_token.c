/*
 * SPDX-FileCopyrightText: Stefan Metzmacher 		2005
 * SPDX-FileCopyrightText: Andrew Tridgell 		2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   security descriptror utility functions

   Copyright (C) Andrew Tridgell 		2004
   Copyright (C) Stefan Metzmacher 		2005
      
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
#include "dsdb/samdb/samdb.h"
#include "libcli/security/security.h"

/*
  return a blank security token
*/
struct security_token *security_token_initialise(TALLOC_CTX *mem_ctx)
{
	struct security_token *st;

	st = talloc(mem_ctx, struct security_token);
	if (!st) {
		return NULL;
	}

	st->user_sid = NULL;
	st->group_sid = NULL;
	st->num_sids = 0;
	st->sids = NULL;
	st->privilege_mask = 0;

	return st;
}

/****************************************************************************
 prints a struct security_token to debug output.
****************************************************************************/
void security_token_debug(int dbg_lev, const struct security_token *token)
{
	TALLOC_CTX *mem_ctx;
	int i;

	if (!token) {
		DEBUG(dbg_lev, ("Security token: (NULL)\n"));
		return;
	}

	mem_ctx = talloc_init("security_token_debug()");
	if (!mem_ctx) {
		return;
	}

	DEBUG(dbg_lev, ("Security token of user %s\n",
				    dom_sid_string(mem_ctx, token->user_sid) ));
	DEBUGADD(dbg_lev, (" SIDs (%lu):\n", 
				       (unsigned long)token->num_sids));
	for (i = 0; i < token->num_sids; i++) {
		DEBUGADD(dbg_lev, ("  SID[%3lu]: %s\n", (unsigned long)i, 
			   dom_sid_string(mem_ctx, token->sids[i])));
	}

	security_token_debug_privileges(dbg_lev, token);

	talloc_free(mem_ctx);
}

/* These really should be cheaper... */

BOOL security_token_is_sid(const struct security_token *token, const struct dom_sid *sid)
{
	if (dom_sid_equal(token->user_sid, sid)) {
		return True;
	}
	return False;
}

BOOL security_token_is_sid_string(const struct security_token *token, const char *sid_string)
{
	BOOL ret;
	struct dom_sid *sid = dom_sid_parse_talloc(NULL, sid_string);
	if (!sid) return False;

	ret = security_token_is_sid(token, sid);

	talloc_free(sid);
	return ret;
}

BOOL security_token_is_system(const struct security_token *token) 
{
	return security_token_is_sid_string(token, SID_NT_SYSTEM);
}

BOOL security_token_is_anonymous(const struct security_token *token) 
{
	return security_token_is_sid_string(token, SID_NT_ANONYMOUS);
}

BOOL security_token_has_sid(const struct security_token *token, const struct dom_sid *sid)
{
	int i;
	for (i = 0; i < token->num_sids; i++) {
		if (dom_sid_equal(token->sids[i], sid)) {
			return True;
		}
	}
	return False;
}

BOOL security_token_has_sid_string(const struct security_token *token, const char *sid_string)
{
	BOOL ret;
	struct dom_sid *sid = dom_sid_parse_talloc(NULL, sid_string);
	if (!sid) return False;

	ret = security_token_has_sid(token, sid);

	talloc_free(sid);
	return ret;
}

BOOL security_token_has_builtin_administrators(const struct security_token *token)
{
	return security_token_has_sid_string(token, SID_BUILTIN_ADMINISTRATORS);
}

BOOL security_token_has_nt_authenticated_users(const struct security_token *token)
{
	return security_token_has_sid_string(token, SID_NT_AUTHENTICATED_USERS);
}
