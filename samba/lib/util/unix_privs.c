/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   gain/lose root privileges

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
#include "system/filesys.h"

/**
 * @file
 * @brief Gaining/losing root privileges
 */

/*
  there are times when smbd needs to temporarily gain root privileges
  to do some operation. To do this you call root_privileges(), which
  returns a talloc handle. To restore your previous privileges
  talloc_free() this pointer.

  Note that this call is considered successful even if it does not
  manage to gain root privileges, but it will call smb_abort() if it
  fails to restore the privileges afterwards. The logic is that
  failing to gain root access can be caught by whatever operation
  needs to be run as root failing, but failing to lose the root
  privileges is dangerous.

  This also means that this code is safe to be called from completely
  unprivileged processes.
*/

struct saved_state {
	uid_t uid;
};

static int privileges_destructor(struct saved_state *s)
{
	if (geteuid() != s->uid &&
	    seteuid(s->uid) != 0) {
		smb_panic("Failed to restore privileges");
	}
	return 0;
}

/**
 * Obtain root privileges for the current process.
 *
 * The privileges can be dropped by talloc_free()-ing the 
 * token returned by this function
 */
void *root_privileges(void)
{
	struct saved_state *s;
	s = talloc(NULL, struct saved_state);
	if (!s) return NULL;
	s->uid = geteuid();
	if (s->uid != 0) {
		seteuid(0);
	}
	talloc_set_destructor(s, privileges_destructor);
	return s;
}
