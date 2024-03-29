/*
 * SPDX-FileCopyrightText: 2004-2005 Jelmer Vernooij <jelmer@samba.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
   Unix SMB/CIFS implementation.
   Simple class
   Copyright (C) 2004-2005 Jelmer Vernooij <jelmer@samba.org>

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
#include "lib/com/com.h"
#include "librpc/gen_ndr/com_dcom.h"

static struct IClassFactory_vtable simple_classobject_vtable;
static struct IStream_vtable simple_IStream_vtable;

static WERROR simple_IUnknown_QueryInterface (struct IUnknown *d, TALLOC_CTX *mem_ctx, struct GUID *iid, struct IUnknown **iun)
{
	*iun = d;
	return WERR_OK;
}

static uint32_t simple_IUnknown_AddRef (struct IUnknown *d, TALLOC_CTX *mem_ctx)
{
	return 1;
}

static uint32_t simple_IUnknown_Release (struct IUnknown *d, TALLOC_CTX *mem_ctx)
{
	return 1;
}

static WERROR simple_IStream_Read (struct IStream *d, TALLOC_CTX *mem_ctx, uint8_t *pv, uint32_t num_requested, uint32_t *num_readx, uint32_t num_read)
{
	printf("%d bytes are being read\n", num_read);
	return WERR_OK;
}

static WERROR simple_IStream_Write (struct IStream *d, TALLOC_CTX *mem_ctx, uint8_t *data, uint32_t num_requested, uint32_t num_written)
{
	printf("%d bytes are being written\n", num_requested);
	return WERR_OK;
}

static WERROR simpleclass_IUnknown_QueryInterface (struct IUnknown *d, TALLOC_CTX *mem_ctx, struct GUID *iid, struct IUnknown **iun)
{
	/* FIXME: Return WERR_IFACE_NOT_SUPPORTED if IID != IID_IUNKNOWN and IID != IID_CLASSFACTORY */
	*iun = d;
	return WERR_OK;
}

static WERROR simpleclass_IClassFactory_CreateInstance (struct IClassFactory *d, TALLOC_CTX *mem_ctx, struct IUnknown *iunk, struct GUID *iid, struct IUnknown **ppv)
{
	struct IStream *ret;
	/* FIXME: Check whether IID == ISTREAM_IID */
	ret = talloc(mem_ctx, struct IStream);
	ret->ctx = NULL;
	ret->vtable = &simple_IStream_vtable;
	ret->object_data = NULL;

	*ppv = (struct IUnknown *)ret;
	
	return WERR_OK;
}

static uint32_t simpleclass_IUnknown_AddRef (struct IUnknown *d, TALLOC_CTX *mem_ctx)
{
	return 1;
}

static uint32_t simpleclass_IUnknown_Release (struct IUnknown *d, TALLOC_CTX *mem_ctx)
{
	return 1;
}

/* Everything below this line should be autogenerated later on */
static struct IClassFactory_vtable simple_classobject_vtable = {
        { 0, 0, 0, { 0, 0 }, { 0, 0, 0, 0, 0, 0 } },
	simpleclass_IUnknown_QueryInterface, 
	simpleclass_IUnknown_AddRef, 
	simpleclass_IUnknown_Release,
	simpleclass_IClassFactory_CreateInstance,
	NULL,
	NULL,
	NULL
};

static struct IStream_vtable simple_IStream_vtable = {
        { 0, 0, 0, { 0, 0 }, { 0, 0, 0, 0, 0, 0 } },
	simple_IUnknown_QueryInterface,
	simple_IUnknown_AddRef, 
	simple_IUnknown_Release,
	simple_IStream_Read,
	simple_IStream_Write
};

NTSTATUS com_simple_init(void)
{
	struct GUID clsid;
	struct IUnknown *class_object = talloc(talloc_autofree_context(), struct IUnknown);

	class_object->ctx = NULL;
	class_object->object_data = NULL;
	class_object->vtable = (struct IUnknown_vtable *)&simple_classobject_vtable;

	GUID_from_string(CLSID_SIMPLE, &clsid);
	GUID_from_string(COM_ICLASSFACTORY_UUID, &simple_classobject_vtable.iid);
	GUID_from_string(COM_ISTREAM_UUID, &simple_IStream_vtable.iid);

	return com_register_running_class(&clsid, PROGID_SIMPLE, class_object);
}
