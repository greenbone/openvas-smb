/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   module to store/fetch session keys for the schannel server

   Copyright (C) Andrew Tridgell 2004
   
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
#include "lib/ldb/include/ldb.h"
#include "lib/ldb/include/ldb_errors.h"
#include "dsdb/samdb/samdb.h"
#include "db_wrap.h"
#include "libcli/auth/libcli_auth.h"
#include "auth/auth.h"

/**
  connect to the schannel ldb
*/
struct ldb_context *schannel_db_connect(TALLOC_CTX *mem_ctx)
{
	char *path;
	struct ldb_context *ldb;
	BOOL existed;
	const char *init_ldif = 
		"dn: @ATTRIBUTES\n" \
		"computerName: CASE_INSENSITIVE\n" \
		"flatname: CASE_INSENSITIVE\n";

	path = smbd_tmp_path(mem_ctx, "schannel.ldb");
	if (!path) {
		return NULL;
	}

	existed = file_exist(path);
	
	ldb = ldb_wrap_connect(mem_ctx, path, system_session(mem_ctx), 
			       NULL, LDB_FLG_NOSYNC, NULL);
	talloc_free(path);
	if (!ldb) {
		return NULL;
	}
	
	if (!existed) {
		gendb_add_ldif(ldb, init_ldif);
	}

	return ldb;
}

/*
  remember an established session key for a netr server authentication
  use a simple ldb structure
*/
NTSTATUS schannel_store_session_key_ldb(TALLOC_CTX *mem_ctx,
					struct ldb_context *ldb,
					struct creds_CredentialState *creds)
{
	struct ldb_message *msg;
	struct ldb_val val, seed, client_state, server_state;
	char *f;
	char *sct;
	int ret;

	f = talloc_asprintf(mem_ctx, "%u", (unsigned int)creds->negotiate_flags);

	if (f == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	sct = talloc_asprintf(mem_ctx, "%u", (unsigned int)creds->secure_channel_type);

	if (sct == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	msg = ldb_msg_new(ldb);
	if (msg == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	msg->dn = ldb_dn_new_fmt(msg, ldb, "computerName=%s", creds->computer_name);
	if ( ! msg->dn) {
		return NT_STATUS_NO_MEMORY;
	}

	val.data = creds->session_key;
	val.length = sizeof(creds->session_key);

	seed.data = creds->seed.data;
	seed.length = sizeof(creds->seed.data);

	client_state.data = creds->client.data;
	client_state.length = sizeof(creds->client.data);
	server_state.data = creds->server.data;
	server_state.length = sizeof(creds->server.data);

	ldb_msg_add_string(msg, "objectClass", "schannelState");
	ldb_msg_add_value(msg, "sessionKey", &val, NULL);
	ldb_msg_add_value(msg, "seed", &seed, NULL);
	ldb_msg_add_value(msg, "clientState", &client_state, NULL);
	ldb_msg_add_value(msg, "serverState", &server_state, NULL);
	ldb_msg_add_string(msg, "negotiateFlags", f);
	ldb_msg_add_string(msg, "secureChannelType", sct);
	ldb_msg_add_string(msg, "accountName", creds->account_name);
	ldb_msg_add_string(msg, "computerName", creds->computer_name);
	ldb_msg_add_string(msg, "flatname", creds->domain);
	samdb_msg_add_dom_sid(ldb, mem_ctx, msg, "objectSid", creds->sid);

	ldb_delete(ldb, msg->dn);

	ret = ldb_add(ldb, msg);

	if (ret != 0) {
		DEBUG(0,("Unable to add %s to session key db - %s\n", 
			 ldb_dn_get_linearized(msg->dn), ldb_errstring(ldb)));
		return NT_STATUS_INTERNAL_DB_CORRUPTION;
	}

	return NT_STATUS_OK;
}

NTSTATUS schannel_store_session_key(TALLOC_CTX *mem_ctx,
				    struct creds_CredentialState *creds)
{
	struct ldb_context *ldb;
	NTSTATUS nt_status;
	int ret;
		
	ldb = schannel_db_connect(mem_ctx);
	if (!ldb) {
		return NT_STATUS_ACCESS_DENIED;
	}

	ret = ldb_transaction_start(ldb);
	if (ret != 0) {
		talloc_free(ldb);
		return NT_STATUS_INTERNAL_DB_CORRUPTION;
	}

	nt_status = schannel_store_session_key_ldb(mem_ctx, ldb, creds);

	if (NT_STATUS_IS_OK(nt_status)) {
		ret = ldb_transaction_commit(ldb);
	} else {
		ret = ldb_transaction_cancel(ldb);
	}

	if (ret != 0) {
		DEBUG(0,("Unable to commit adding credentials for %s to schannel key db - %s\n", 
			 creds->computer_name, ldb_errstring(ldb)));
		talloc_free(ldb);
		return NT_STATUS_INTERNAL_DB_CORRUPTION;
	}

	talloc_free(ldb);
	return nt_status;
}

/*
  read back a credentials back for a computer
*/
NTSTATUS schannel_fetch_session_key_ldb(TALLOC_CTX *mem_ctx,
					struct ldb_context *ldb,
					const char *computer_name, 
					const char *domain,
					struct creds_CredentialState **creds)
{
	struct ldb_result *res;
	int ret;
	const struct ldb_val *val;

	*creds = talloc_zero(mem_ctx, struct creds_CredentialState);
	if (!*creds) {
		return NT_STATUS_NO_MEMORY;
	}

	ret = ldb_search_exp_fmt(ldb, mem_ctx, &res,
				 NULL, LDB_SCOPE_SUBTREE, NULL,
				"(&(computerName=%s)(flatname=%s))", computer_name, domain);
	if (ret != LDB_SUCCESS) {
		DEBUG(3,("schannel: Failed to find a record for client %s: %s\n", computer_name, ldb_errstring(ldb)));
		return NT_STATUS_INVALID_HANDLE;
	}
	if (res->count != 1) {
		DEBUG(3,("schannel: Failed to find a record for client: %s (found %d records)\n", computer_name, res->count));
		talloc_free(res);
		return NT_STATUS_INVALID_HANDLE;
	}

	val = ldb_msg_find_ldb_val(res->msgs[0], "sessionKey");
	if (val == NULL || val->length != 16) {
		DEBUG(1,("schannel: record in schannel DB must contain a sessionKey of length 16, when searching for client: %s\n", computer_name));
		talloc_free(res);
		return NT_STATUS_INTERNAL_ERROR;
	}

	memcpy((*creds)->session_key, val->data, 16);

	val = ldb_msg_find_ldb_val(res->msgs[0], "seed");
	if (val == NULL || val->length != 8) {
		DEBUG(1,("schannel: record in schannel DB must contain a vaid seed of length 8, when searching for client: %s\n", computer_name));
		talloc_free(res);
		return NT_STATUS_INTERNAL_ERROR;
	}

	memcpy((*creds)->seed.data, val->data, 8);

	val = ldb_msg_find_ldb_val(res->msgs[0], "clientState");
	if (val == NULL || val->length != 8) {
		DEBUG(1,("schannel: record in schannel DB must contain a vaid clientState of length 8, when searching for client: %s\n", computer_name));
		talloc_free(res);
		return NT_STATUS_INTERNAL_ERROR;
	}
	memcpy((*creds)->client.data, val->data, 8);

	val = ldb_msg_find_ldb_val(res->msgs[0], "serverState");
	if (val == NULL || val->length != 8) {
		DEBUG(1,("schannel: record in schannel DB must contain a vaid serverState of length 8, when searching for client: %s\n", computer_name));
		talloc_free(res);
		return NT_STATUS_INTERNAL_ERROR;
	}
	memcpy((*creds)->server.data, val->data, 8);

	(*creds)->negotiate_flags = ldb_msg_find_attr_as_int(res->msgs[0], "negotiateFlags", 0);

	(*creds)->secure_channel_type = ldb_msg_find_attr_as_int(res->msgs[0], "secureChannelType", 0);

	(*creds)->account_name = talloc_strdup(*creds, ldb_msg_find_attr_as_string(res->msgs[0], "accountName", NULL));
	if ((*creds)->account_name == NULL) {
		talloc_free(res);
		return NT_STATUS_NO_MEMORY;
	}

	(*creds)->computer_name = talloc_strdup(*creds, ldb_msg_find_attr_as_string(res->msgs[0], "computerName", NULL));
	if ((*creds)->computer_name == NULL) {
		talloc_free(res);
		return NT_STATUS_NO_MEMORY;
	}

	(*creds)->domain = talloc_strdup(*creds, ldb_msg_find_attr_as_string(res->msgs[0], "flatname", NULL));
	if ((*creds)->domain == NULL) {
		talloc_free(res);
		return NT_STATUS_NO_MEMORY;
	}

	(*creds)->sid = samdb_result_dom_sid(*creds, res->msgs[0], "objectSid");

	talloc_free(res);
	return NT_STATUS_OK;
}

NTSTATUS schannel_fetch_session_key(TALLOC_CTX *mem_ctx,
					const char *computer_name, 
					const char *domain, 
					struct creds_CredentialState **creds)
{
	NTSTATUS nt_status;
	struct ldb_context *ldb;

	ldb = schannel_db_connect(mem_ctx);
	if (!ldb) {
		return NT_STATUS_ACCESS_DENIED;
	}

	nt_status = schannel_fetch_session_key_ldb(mem_ctx, ldb,
						   computer_name, domain, 
						   creds);
	talloc_free(ldb);
	return nt_status;
}
