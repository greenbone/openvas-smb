/*
 * SPDX-FileCopyrightText: Zenoss, Inc. 2008
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef WMI_H_
#define WMI_H_
/*
   WMI core functions

   Copyright (C) Zenoss, Inc. 2008

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

extern WERROR WBEM_ConnectServer(struct com_context *ctx, const char *server,
        const char *nspace, const char *user, const char *password,
        const char *locale, uint32_t flags, const char *authority,
        struct IWbemContext* wbem_ctx, struct IWbemServices** services);

extern WERROR WBEM_ConnectServer_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct IWbemServices **services);

extern struct composite_context *WBEM_ConnectServer_send(struct com_context *ctx,
        TALLOC_CTX *parent_ctx, const char *server, const char *nspace,
        const char *user, const char *password, const char *locale,
        uint32_t flags, const char *authority, struct IWbemContext* wbem_ctx);

extern struct composite_context *IEnumWbemClassObject_SmartNext_send(
        struct IEnumWbemClassObject *d, TALLOC_CTX *parent_ctx,
        int32_t lTimeout, uint32_t uCount);

extern WERROR IEnumWbemClassObject_SmartNext_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct WbemClassObject **apObjects,
        uint32_t *puReturned);

extern WERROR IEnumWbemClassObject_SmartNext(struct IEnumWbemClassObject *d,
        TALLOC_CTX *mem_ctx, int32_t lTimeout, uint32_t uCount,
        struct WbemClassObject **apObjects, uint32_t *puReturned);

extern const char *wmi_errstr(WERROR werror);

extern WERROR IWbemClassObject_GetMethod(struct IWbemClassObject *d,
        TALLOC_CTX *mem_ctx, const char *name, uint32_t flags,
        struct IWbemClassObject **in, struct IWbemClassObject **out);

extern WERROR IWbemClassObject_SpawnInstance(struct IWbemClassObject *d,
        TALLOC_CTX *mem_ctx, uint32_t flags,
        struct IWbemClassObject **instance);

extern WERROR IWbemClassObject_Put(struct IWbemClassObject *d,
        TALLOC_CTX *mem_ctx, const char *name, uint32_t flags,
        union CIMVAR *val, enum CIMTYPE_ENUMERATION cimtype);

extern WERROR WbemClassObject_Get(struct WbemClassObject *d,
        TALLOC_CTX *mem_ctx, const char *name, uint32_t flags,
        union CIMVAR *val, enum CIMTYPE_ENUMERATION *cimtype,
        uint32_t *flavor);

#endif /* WMI_H_ */
