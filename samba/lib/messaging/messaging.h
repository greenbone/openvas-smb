/*
 * SPDX-FileCopyrightText: 2001, 2002 by Martin Pool
 * SPDX-FileCopyrightText: Andrew Tridgell 2000
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   messages.c header
   Copyright (C) Andrew Tridgell 2000
   Copyright (C) 2001, 2002 by Martin Pool
   
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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

struct messaging_context;

/* general messages */
#define MSG_DEBUG		1
#define MSG_PING		2
#define MSG_PONG		3
#define MSG_BRL_RETRY		4
#define MSG_PVFS_RETRY_OPEN	5
#define MSG_IRPC                6
#define MSG_PVFS_NOTIFY		7
#define MSG_NTVFS_OPLOCK_BREAK	8

/* temporary messaging endpoints are allocated above this line */
#define MSG_TMP_BASE		1000

#endif
