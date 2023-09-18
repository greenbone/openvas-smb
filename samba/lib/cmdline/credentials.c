/*
 * SPDX-FileCopyrightText: Jelmer Vernooij 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   Copyright (C) Jelmer Vernooij 2005

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
#include "system/filesys.h"
#include "auth/credentials/credentials.h"

static const char *cmdline_get_userpassword(struct cli_credentials *credentials)
{
	char *ret;
	TALLOC_CTX *mem_ctx = talloc_new(NULL);

	const char *prompt_name = cli_credentials_get_unparsed_name(credentials, mem_ctx);
	const char *prompt;

	prompt = talloc_asprintf(mem_ctx, "Password for [%s]:", 
				 prompt_name);

	ret = getpass(prompt);

	talloc_free(mem_ctx);
	return ret;
}

void cli_credentials_set_cmdline_callbacks(struct cli_credentials *cred)
{
	cli_credentials_set_password_callback(cred, cmdline_get_userpassword);
}
