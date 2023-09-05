/*
 * SPDX-FileCopyrightText: 2006 Andrzej Hajda <andrzej.hajda@wp.pl>
 * SPDX-FileCopyrightText: Jelmer Vernooij					  2004-2005.
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   COM standard objects
   Copyright (C) Jelmer Vernooij					  2004-2005.
   Copyright (C) 2006 Andrzej Hajda <andrzej.hajda@wp.pl>
   
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

#ifndef _COM_H /* _COM_H */
#define _COM_H 

struct IUnknown_vtable;

enum {
        COM_EXT_WMI_CLASS_CACHE = 1
};

struct com_context 
{
	struct dcom_client_context *dcom;
	struct event_context *event_ctx;
        struct com_extension {
                uint32_t id;
                void *data;
                struct com_extension *prev, *next;
        } *extensions;
};

typedef struct IUnknown *(*get_class_object_function) (const struct GUID *clsid);


#include "lib/com/proto.h"

#endif /* _COM_H */
