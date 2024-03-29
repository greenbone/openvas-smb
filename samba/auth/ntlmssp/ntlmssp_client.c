/*
 * SPDX-FileCopyrightText: Stefan Metzmacher 2005
 * SPDX-FileCopyrightText: Andrew Bartlett <abartlet@samba.org> 2001-2005
 * SPDX-FileCopyrightText: Andrew Tridgell      2001
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/Netbios implementation.
   Version 3.0
   handle NLTMSSP, client server side parsing

   Copyright (C) Andrew Tridgell      2001
   Copyright (C) Andrew Bartlett <abartlet@samba.org> 2001-2005
   Copyright (C) Stefan Metzmacher 2005

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
#include "auth/ntlmssp/ntlmssp.h"
#include "auth/ntlmssp/msrpc_parse.h"
#include "lib/crypto/crypto.h"
#include "libcli/auth/libcli_auth.h"
#include "auth/credentials/credentials.h"
#include "auth/gensec/gensec.h"

/*********************************************************************
 Client side NTLMSSP
*********************************************************************/

/**
 * Next state function for the Initial packet
 * 
 * @param ntlmssp_state NTLMSSP State
 * @param out_mem_ctx The DATA_BLOB *out will be allocated on this context
 * @param in A NULL data blob (input ignored)
 * @param out The initial negotiate request to the server, as an talloc()ed DATA_BLOB, on out_mem_ctx
 * @return Errors or NT_STATUS_OK. 
 */

NTSTATUS ntlmssp_client_initial(struct gensec_security *gensec_security, 
				TALLOC_CTX *out_mem_ctx, 
				DATA_BLOB in, DATA_BLOB *out) 
{
	struct gensec_ntlmssp_state *gensec_ntlmssp_state = gensec_security->private_data;

	if (gensec_ntlmssp_state->unicode) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_UNICODE;
	} else {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_OEM;
	}
	
	if (gensec_ntlmssp_state->use_ntlmv2) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_NTLM2;
	}

	/* generate the ntlmssp negotiate packet */
	msrpc_gen(out_mem_ctx, 
		  out, "CddAA",
		  "NTLMSSP",
		  NTLMSSP_NEGOTIATE,
		  gensec_ntlmssp_state->neg_flags,
		  gensec_ntlmssp_state->get_domain(), 
		  cli_credentials_get_workstation(gensec_security->credentials));

	gensec_ntlmssp_state->expected_state = NTLMSSP_CHALLENGE;

	return NT_STATUS_MORE_PROCESSING_REQUIRED;
}

/**
 * Next state function for the Challenge Packet.  Generate an auth packet.
 * 
 * @param gensec_security GENSEC state
 * @param out_mem_ctx Memory context for *out
 * @param in The server challnege, as a DATA_BLOB.  reply.data must be NULL
 * @param out The next request (auth packet) to the server, as an allocated DATA_BLOB, on the out_mem_ctx context
 * @return Errors or NT_STATUS_OK. 
 */

NTSTATUS ntlmssp_client_challenge(struct gensec_security *gensec_security, 
				  TALLOC_CTX *out_mem_ctx,
				  const DATA_BLOB in, DATA_BLOB *out) 
{
	struct gensec_ntlmssp_state *gensec_ntlmssp_state = gensec_security->private_data;
	uint32_t chal_flags, ntlmssp_command, unkn1, unkn2;
	DATA_BLOB server_domain_blob;
	DATA_BLOB challenge_blob;
	DATA_BLOB target_info = data_blob(NULL, 0);
	char *server_domain;
	const char *chal_parse_string;
	const char *auth_gen_string;
	DATA_BLOB lm_response = data_blob(NULL, 0);
	DATA_BLOB nt_response = data_blob(NULL, 0);
	DATA_BLOB session_key = data_blob(NULL, 0);
	DATA_BLOB lm_session_key = data_blob(NULL, 0);
	DATA_BLOB encrypted_session_key = data_blob(NULL, 0);
	NTSTATUS nt_status;
	int flags = 0;
	const char *user, *domain;

	TALLOC_CTX *mem_ctx = talloc_new(out_mem_ctx);
	if (!mem_ctx) {
		return NT_STATUS_NO_MEMORY;
	}

	if (!msrpc_parse(mem_ctx,
			 &in, "CdBd",
			 "NTLMSSP",
			 &ntlmssp_command, 
			 &server_domain_blob,
			 &chal_flags)) {
		DEBUG(1, ("Failed to parse the NTLMSSP Challenge: (#1)\n"));
		dump_data(2, in.data, in.length);
		talloc_free(mem_ctx);

		return NT_STATUS_INVALID_PARAMETER;
	}
	
	data_blob_free(&server_domain_blob);

	DEBUG(3, ("Got challenge flags:\n"));
	debug_ntlmssp_flags(chal_flags);

	ntlmssp_handle_neg_flags(gensec_ntlmssp_state, chal_flags, gensec_ntlmssp_state->allow_lm_key);

	if (gensec_ntlmssp_state->unicode) {
		if (chal_flags & NTLMSSP_CHAL_TARGET_INFO) {
			chal_parse_string = "CdUdbddB";
		} else {
			chal_parse_string = "CdUdbdd";
		}
		auth_gen_string = "CdBBUUUBd";
	} else {
		if (chal_flags & NTLMSSP_CHAL_TARGET_INFO) {
			chal_parse_string = "CdAdbddB";
		} else {
			chal_parse_string = "CdAdbdd";
		}

		auth_gen_string = "CdBBAAABd";
	}

	if (!msrpc_parse(mem_ctx,
			 &in, chal_parse_string,
			 "NTLMSSP",
			 &ntlmssp_command, 
			 &server_domain,
			 &chal_flags,
			 &challenge_blob, 8,
			 &unkn1, &unkn2,
			 &target_info)) {
		DEBUG(1, ("Failed to parse the NTLMSSP Challenge: (#2)\n"));
		dump_data(2, in.data, in.length);
		talloc_free(mem_ctx);
		return NT_STATUS_INVALID_PARAMETER;
	}

	gensec_ntlmssp_state->server_domain = server_domain;

	if (challenge_blob.length != 8) {
		talloc_free(mem_ctx);
		return NT_STATUS_INVALID_PARAMETER;
	}

	cli_credentials_get_ntlm_username_domain(gensec_security->credentials, mem_ctx, 
						 &user, &domain);

	if (gensec_ntlmssp_state->neg_flags & NTLMSSP_NEGOTIATE_NTLM2) {
		flags |= CLI_CRED_NTLM2;
	}
	if (gensec_ntlmssp_state->use_ntlmv2) {
		flags |= CLI_CRED_NTLMv2_AUTH;
	}
	if (gensec_ntlmssp_state->use_nt_response) {
		flags |= CLI_CRED_NTLM_AUTH;
	}
	if (lp_client_lanman_auth()) {
		flags |= CLI_CRED_LANMAN_AUTH;
	}

	nt_status = cli_credentials_get_ntlm_response(gensec_security->credentials, mem_ctx, 
						      &flags, challenge_blob, target_info,
						      &lm_response, &nt_response, 
						      &lm_session_key, &session_key);

	if (!NT_STATUS_IS_OK(nt_status)) {
		return nt_status;
	}
	
	if (!(flags & CLI_CRED_LANMAN_AUTH)) {
		/* LM Key is still possible, just silly.  Fortunetly
		 * we require command line options to end up here */
		/* gensec_ntlmssp_state->neg_flags &= ~NTLMSSP_NEGOTIATE_LM_KEY; */
	}

	if (!(flags & CLI_CRED_NTLM2)) {
		/* NTLM2 is incompatible... */
		gensec_ntlmssp_state->neg_flags &= ~NTLMSSP_NEGOTIATE_NTLM2;
	}
	
	if ((gensec_ntlmssp_state->neg_flags & NTLMSSP_NEGOTIATE_LM_KEY) 
	    && lp_client_lanman_auth() && lm_session_key.length == 16) {
		DATA_BLOB new_session_key = data_blob_talloc(mem_ctx, NULL, 16);
		if (lm_response.length == 24) {
			SMBsesskeygen_lm_sess_key(lm_session_key.data, lm_response.data, 
						  new_session_key.data);
		} else {
			static const uint8_t zeros[24];
			SMBsesskeygen_lm_sess_key(lm_session_key.data, zeros,
						  new_session_key.data);
		}
		session_key = new_session_key;
		dump_data_pw("LM session key\n", session_key.data, session_key.length);
	}


	/* Key exchange encryptes a new client-generated session key with
	   the password-derived key */
	if (gensec_ntlmssp_state->neg_flags & NTLMSSP_NEGOTIATE_KEY_EXCH) {
		/* Make up a new session key */
		uint8_t client_session_key[16];
		generate_random_buffer(client_session_key, sizeof(client_session_key));

		/* Encrypt the new session key with the old one */
		encrypted_session_key = data_blob_talloc(gensec_ntlmssp_state, 
							 client_session_key, sizeof(client_session_key));
		dump_data_pw("KEY_EXCH session key:\n", encrypted_session_key.data, encrypted_session_key.length);
		arcfour_crypt(encrypted_session_key.data, session_key.data, encrypted_session_key.length);
		dump_data_pw("KEY_EXCH session key (enc):\n", encrypted_session_key.data, encrypted_session_key.length);

		/* Mark the new session key as the 'real' session key */
		session_key = data_blob_talloc(mem_ctx, client_session_key, sizeof(client_session_key));
	}

	DEBUG(3, ("NTLMSSP: Set final flags:\n"));
	debug_ntlmssp_flags(gensec_ntlmssp_state->neg_flags);

	/* this generates the actual auth packet */
	if (!msrpc_gen(mem_ctx, 
		       out, auth_gen_string, 
		       "NTLMSSP", 
		       NTLMSSP_AUTH, 
		       lm_response.data, lm_response.length,
		       nt_response.data, nt_response.length,
		       domain, 
		       user, 
		       cli_credentials_get_workstation(gensec_security->credentials),
		       encrypted_session_key.data, encrypted_session_key.length,
		       gensec_ntlmssp_state->neg_flags)) {
		talloc_free(mem_ctx);
		return NT_STATUS_NO_MEMORY;
	}

	gensec_ntlmssp_state->session_key = session_key;
	talloc_steal(gensec_ntlmssp_state, session_key.data);

	talloc_steal(out_mem_ctx, out->data);

	gensec_ntlmssp_state->chal = challenge_blob;
	gensec_ntlmssp_state->lm_resp = lm_response;
	talloc_steal(gensec_ntlmssp_state->lm_resp.data, lm_response.data);
	gensec_ntlmssp_state->nt_resp = nt_response;
	talloc_steal(gensec_ntlmssp_state->nt_resp.data, nt_response.data);

	gensec_ntlmssp_state->expected_state = NTLMSSP_DONE;

	if (gensec_security->want_features & (GENSEC_FEATURE_SIGN|GENSEC_FEATURE_SEAL)) {
		nt_status = ntlmssp_sign_init(gensec_ntlmssp_state);
		if (!NT_STATUS_IS_OK(nt_status)) {
			DEBUG(1, ("Could not setup NTLMSSP signing/sealing system (error was: %s)\n", 
				  nt_errstr(nt_status)));
			talloc_free(mem_ctx);
			return nt_status;
		}
	}

	talloc_free(mem_ctx);
	return NT_STATUS_OK;
}

NTSTATUS gensec_ntlmssp_client_start(struct gensec_security *gensec_security)
{
	struct gensec_ntlmssp_state *gensec_ntlmssp_state;
	NTSTATUS nt_status;

	nt_status = gensec_ntlmssp_start(gensec_security);
	NT_STATUS_NOT_OK_RETURN(nt_status);

	gensec_ntlmssp_state = gensec_security->private_data;

	gensec_ntlmssp_state->role = NTLMSSP_CLIENT;

	gensec_ntlmssp_state->get_domain = lp_workgroup;

	gensec_ntlmssp_state->unicode = lp_parm_bool(-1, "ntlmssp_client", "unicode", True);

	gensec_ntlmssp_state->use_nt_response = lp_parm_bool(-1, "ntlmssp_client", "send_nt_reponse", True);

	gensec_ntlmssp_state->allow_lm_key = (lp_client_lanman_auth() 
					      && (lp_parm_bool(-1, "ntlmssp_client", "allow_lm_key", False)
						  || lp_parm_bool(-1, "ntlmssp_client", "lm_key", False)));

	gensec_ntlmssp_state->use_ntlmv2 = lp_client_ntlmv2_auth();

	gensec_ntlmssp_state->expected_state = NTLMSSP_INITIAL;

	gensec_ntlmssp_state->neg_flags = 
		NTLMSSP_NEGOTIATE_NTLM |
		NTLMSSP_REQUEST_TARGET;

	if (lp_parm_bool(-1, "ntlmssp_client", "128bit", True)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_128;		
	}

	if (lp_parm_bool(-1, "ntlmssp_client", "56bit", False)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_56;		
	}

	if (lp_parm_bool(-1, "ntlmssp_client", "lm_key", False)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_LM_KEY;
	}

	if (lp_parm_bool(-1, "ntlmssp_client", "keyexchange", True)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_KEY_EXCH;		
	}

	if (lp_parm_bool(-1, "ntlmssp_client", "alwayssign", True)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_ALWAYS_SIGN;		
	}

	if (lp_parm_bool(-1, "ntlmssp_client", "ntlm2", True)) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_NTLM2;		
	} else {
		/* apparently we can't do ntlmv2 if we don't do ntlm2 */
		gensec_ntlmssp_state->use_ntlmv2 = False;
	}

	if (gensec_security->want_features & GENSEC_FEATURE_SESSION_KEY) {
		/*
		 * We need to set this to allow a later SetPassword
		 * via the SAMR pipe to succeed. Strange.... We could
		 * also add  NTLMSSP_NEGOTIATE_SEAL here. JRA.
		 * 
		 * Without this, Windows will not create the master key
		 * that it thinks is only used for NTLMSSP signing and 
		 * sealing.  (It is actually pulled out and used directly) 
		 */
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_SIGN;
	}
	if (gensec_security->want_features & GENSEC_FEATURE_SIGN) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_SIGN;
	}
	if (gensec_security->want_features & GENSEC_FEATURE_SEAL) {
		gensec_ntlmssp_state->neg_flags |= NTLMSSP_NEGOTIATE_SEAL;
	}

	gensec_security->private_data = gensec_ntlmssp_state;

	return NT_STATUS_OK;
}

