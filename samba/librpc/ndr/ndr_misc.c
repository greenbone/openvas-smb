/*
 * SPDX-FileCopyrightText: Stefan (metze) Metzmacher         2004.
 * SPDX-FileCopyrightText: Andrew Tridgell                   2003.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   UUID/GUID/policy_handle functions

   Copyright (C) Andrew Tridgell                   2003.
   Copyright (C) Stefan (metze) Metzmacher         2004.
   
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
#include "system/network.h"
#include "librpc/ndr/libndr.h"

_PUBLIC_ void ndr_print_ipv4_addr(struct ndr_print *ndr, const char *name, const struct ipv4_addr *_ip)
{
	struct ipv4_addr ip;

	ip.addr = htonl(_ip->addr);

	ndr->print(ndr, "%-25s: %s", name, sys_inet_ntoa(ip));
}

_PUBLIC_ void ndr_print_GUID(struct ndr_print *ndr, const char *name, const struct GUID *guid)
{
	ndr->print(ndr, "%-25s: %s", name, GUID_string(ndr, guid));
}

