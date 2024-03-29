/*
 * SPDX-FileCopyrightText: Stefan Metzmacher            2005
 * SPDX-FileCopyrightText: Jelmer Vernooij              2002
 * SPDX-FileCopyrightText: Andrew Bartlett         2001-2002
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   Generic authentication types
   Copyright (C) Andrew Bartlett         2001-2002
   Copyright (C) Jelmer Vernooij              2002
   Copyright (C) Stefan Metzmacher            2005
   
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
#include "auth/auth.h"
#include "libcli/security/security.h"
#include "librpc/gen_ndr/ndr_samr.h"

static NTSTATUS name_to_ntstatus_want_check(struct auth_method_context *ctx,
			      		    TALLOC_CTX *mem_ctx,
					    const struct auth_usersupplied_info *user_info)
{
	return NT_STATUS_OK;
}

/** 
 * Return an error based on username
 *
 * This function allows the testing of obsure errors, as well as the generation
 * of NT_STATUS -> DOS error mapping tables.
 *
 * This module is of no value to end-users.
 *
 * The password is ignored.
 *
 * @return An NTSTATUS value based on the username
 **/

static NTSTATUS name_to_ntstatus_check_password(struct auth_method_context *ctx,
			      		        TALLOC_CTX *mem_ctx,
					        const struct auth_usersupplied_info *user_info, 
					        struct auth_serversupplied_info **_server_info)
{
	NTSTATUS nt_status;
	struct auth_serversupplied_info *server_info;
	uint32_t error_num;
	const char *user;

	user = user_info->client.account_name;

	if (strncasecmp("NT_STATUS", user, strlen("NT_STATUS")) == 0) {
		nt_status = nt_status_string_to_code(user);
	} else {
		error_num = strtoul(user, NULL, 16);
		DEBUG(5,("name_to_ntstatus_check_password: Error for user %s was 0x%08X\n", user, error_num));
		nt_status = NT_STATUS(error_num);
	}
	NT_STATUS_NOT_OK_RETURN(nt_status);

	server_info = talloc(mem_ctx, struct auth_serversupplied_info);
	NT_STATUS_HAVE_NO_MEMORY(server_info);

	server_info->account_sid = dom_sid_parse_talloc(server_info, SID_NT_ANONYMOUS);
	NT_STATUS_HAVE_NO_MEMORY(server_info->account_sid);

	/* is this correct? */
	server_info->primary_group_sid = dom_sid_parse_talloc(server_info, SID_BUILTIN_GUESTS);
	NT_STATUS_HAVE_NO_MEMORY(server_info->primary_group_sid);

	server_info->n_domain_groups = 0;
	server_info->domain_groups = NULL;

	/* annoying, but the Anonymous really does have a session key, 
	   and it is all zeros! */
	server_info->user_session_key = data_blob_talloc(server_info, NULL, 16);
	NT_STATUS_HAVE_NO_MEMORY(server_info->user_session_key.data);

	server_info->lm_session_key = data_blob_talloc(server_info, NULL, 16);
	NT_STATUS_HAVE_NO_MEMORY(server_info->lm_session_key.data);

	data_blob_clear(&server_info->user_session_key);
	data_blob_clear(&server_info->lm_session_key);

	server_info->account_name = talloc_asprintf(server_info, "NAME TO NTSTATUS %s ANONYMOUS LOGON", user);
	NT_STATUS_HAVE_NO_MEMORY(server_info->account_name);

	server_info->domain_name = talloc_strdup(server_info, "NT AUTHORITY");
	NT_STATUS_HAVE_NO_MEMORY(server_info->domain_name);

	server_info->full_name = talloc_asprintf(server_info, "NAME TO NTSTATUS %s Anonymous Logon", user);
	NT_STATUS_HAVE_NO_MEMORY(server_info->full_name);

	server_info->logon_script = talloc_strdup(server_info, "");
	NT_STATUS_HAVE_NO_MEMORY(server_info->logon_script);

	server_info->profile_path = talloc_strdup(server_info, "");
	NT_STATUS_HAVE_NO_MEMORY(server_info->profile_path);

	server_info->home_directory = talloc_strdup(server_info, "");
	NT_STATUS_HAVE_NO_MEMORY(server_info->home_directory);

	server_info->home_drive = talloc_strdup(server_info, "");
	NT_STATUS_HAVE_NO_MEMORY(server_info->home_drive);

	server_info->last_logon = 0;
	server_info->last_logoff = 0;
	server_info->acct_expiry = 0;
	server_info->last_password_change = 0;
	server_info->allow_password_change = 0;
	server_info->force_password_change = 0;

	server_info->logon_count = 0;
	server_info->bad_password_count = 0;

	server_info->acct_flags = ACB_NORMAL;

	server_info->authenticated = False;

	*_server_info = server_info;

	return nt_status;
}

static struct auth_operations name_to_ntstatus_auth_ops = {
	.name		= "name_to_ntstatus",
	.get_challenge	= auth_get_challenge_not_implemented,
	.want_check	= name_to_ntstatus_want_check,
	.check_password	= name_to_ntstatus_check_password
};

/** 
 * Return a 'fixed' challenge instead of a variable one.
 *
 * The idea of this function is to make packet snifs consistant
 * with a fixed challenge, so as to aid debugging.
 *
 * This module is of no value to end-users.
 *
 * This module does not actually authenticate the user, but
 * just pretenteds to need a specified challenge.  
 * This module removes *all* security from the challenge-response system
 *
 * @return NT_STATUS_UNSUCCESSFUL
 **/
static NTSTATUS fixed_challenge_get_challenge(struct auth_method_context *ctx, TALLOC_CTX *mem_ctx, DATA_BLOB *_blob)
{
	DATA_BLOB blob;
	const char *challenge = "I am a teapot";

	blob = data_blob_talloc(mem_ctx, challenge, 8);
	NT_STATUS_HAVE_NO_MEMORY(blob.data);

	*_blob = blob;
	return NT_STATUS_OK;
}

static NTSTATUS fixed_challenge_want_check(struct auth_method_context *ctx,
			      		   TALLOC_CTX *mem_ctx,
					   const struct auth_usersupplied_info *user_info)
{
	/* don't handle any users */
	return NT_STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS fixed_challenge_check_password(struct auth_method_context *ctx,
			      		       TALLOC_CTX *mem_ctx,
					       const struct auth_usersupplied_info *user_info,
					       struct auth_serversupplied_info **_server_info)
{
	/* don't handle any users */
	return NT_STATUS_NO_SUCH_USER;
}

static struct auth_operations fixed_challenge_auth_ops = {
	.name		= "fixed_challenge",
	.get_challenge	= fixed_challenge_get_challenge,
	.want_check	= fixed_challenge_want_check,
	.check_password	= fixed_challenge_check_password
};

NTSTATUS auth_developer_init(void)
{
	NTSTATUS ret;

	ret = auth_register(&name_to_ntstatus_auth_ops);
	if (!NT_STATUS_IS_OK(ret)) {
		DEBUG(0,("Failed to register 'name_to_ntstatus' auth backend!\n"));
		return ret;
	}

	ret = auth_register(&fixed_challenge_auth_ops);
	if (!NT_STATUS_IS_OK(ret)) {
		DEBUG(0,("Failed to register 'fixed_challenge' auth backend!\n"));
		return ret;
	}

	return ret;
}
