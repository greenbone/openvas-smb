/*
 * SPDX-FileCopyrightText: Zenoss, Inc. 2008
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
   WMI Core Functionality
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
#include "includes.h"
#include "auth/credentials/credentials.h"
#include "librpc/gen_ndr/com_dcom.h"
#include "lib/com/dcom/dcom.h"
#include "libcli/composite/composite.h"
#include "wmi/wmi.h"

/*
 * Structure used to maintain state across successive asynchronous calls when
 * connecting to a WBEM server via DCOM.
 */
struct wbem_connect_context
{
    struct com_context *com_context;
    struct IWbemLevel1Login *login;
    struct IWbemServices *services;
    const char *wszNetworkResource;
    const char *wszPreferredLocale;
    uint32_t lFlags;
    struct IWbemContext *pCtx;
};

/*
 * Finish off the WBEM connect host request by receiving the results of the
 * IUnknown:Release call of the IWbemLevel1Login interface pointer and then
 * letting the parent composite continue.
 */
static void wbem_release_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    /* receive the results, but we don't really care what they are */
    (void)IUnknown_Release_recv(ctx);

    composite_done(c);
}

/*
 * Continue the WBEM connect server request by processing the results of the
 * NTLMLogin request. If successful, we will be provided with an IWbemServices
 * interface pointer that is our ultimate goal.
 */
static void wbem_login_server_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct wbem_connect_context *s = NULL;
    struct IWbemServices *services = NULL;
    WERROR result;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    s = talloc_get_type(c->private_data, struct wbem_connect_context);

    /* receive the results of the NTLMLogin request */
    result = IWbemLevel1Login_NTLMLogin_recv(ctx, &services);
    if (!W_ERROR_IS_OK(result))
    {
        composite_error(c, werror_to_ntstatus(result));
    }
    else
    {
        struct composite_context *release_ctx = NULL;

        release_ctx = IUnknown_Release_send((struct IUnknown *)s->login, c);
        if (composite_nomem(release_ctx, c)) return;

        composite_continue(c, release_ctx, wbem_release_continue, c);

        s->services = services;
    }
}

/*
 * Continue the connect server request by receiving the results of the DCOM
 * activation request. If successful, then we can continue by issuing the
 * NTLMLogin call.
 */
static void wbem_connect_server_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct wbem_connect_context *s = NULL;
    struct IUnknown **interfaces = NULL;
    struct composite_context *login_ctx = NULL;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    s = talloc_get_type(c->private_data, struct wbem_connect_context);

    /*
     * receive the results of our activation request - hopefully a nice
     * array of IUnknown interface pointers
     */
    c->status = dcom_activate_recv(ctx, c, &interfaces);
    if (!composite_is_ok(c)) return;

    s->login = (struct IWbemLevel1Login *)interfaces[0];

    /* we don't need the actual array memory any longer, so let it go */
    talloc_free(interfaces);

    /* send off the NTLMLogin request and then setup continuation from there */
    login_ctx = IWbemLevel1Login_NTLMLogin_send(s->login,
            c, s->wszNetworkResource, s->wszPreferredLocale, s->lFlags,
            s->pCtx);
    if (composite_nomem(login_ctx, c)) return;

    composite_continue(c, login_ctx, wbem_login_server_continue, c);
}

/*
 * Complete an asynchronous WBEM connect server request by retrieving the
 * status and the resulting IWbemServices interface pointer, if available.
 */
WERROR WBEM_ConnectServer_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct IWbemServices **services)
{
    WERROR result;

    NTSTATUS status = composite_wait(c);
    if (!NT_STATUS_IS_OK(status))
    {
        result = ntstatus_to_werror(status);
    }
    else
    {
        struct wbem_connect_context *s = talloc_get_type(c->private_data,
                struct wbem_connect_context);

        talloc_steal(parent_ctx, s->services);
        *services = s->services;

        result = WERR_OK;
    }

    talloc_free(c);
    return result;
}

/*
 * Asynchronously connect to a remote DCOM server and activate the IWbemServices
 * interface for WBEM work.
 */
struct composite_context *WBEM_ConnectServer_send(struct com_context *com_ctx,
        TALLOC_CTX *parent_ctx, const char *server, const char *nspace,
        const char *user, const char *password, const char *locale,
        uint32_t flags, const char *authority, struct IWbemContext* wbem_ctx)
{
    struct composite_context *c = NULL;
    struct wbem_connect_context *s = NULL;
    struct composite_context *new_ctx = NULL;
    struct GUID clsid;
    struct GUID iid;

    c = composite_create(parent_ctx, com_ctx->event_ctx);
    if (c == NULL) return NULL;

    /*
     * if credentials were passed directly, rather than grabbed from
     * command-line processing, build a temporary credentials structure and
     * add it to the DCOM credentials cache
     */
    if (user)
    {
        char *cred = talloc_asprintf(c, "%s%%%s", user, password);
        struct cli_credentials *cc = cli_credentials_init(cred);
        cli_credentials_set_conf(cc);
        cli_credentials_parse_string(cc, cred, CRED_SPECIFIED);
        dcom_set_server_credentials(com_ctx, server, cc);
        talloc_free(cred);
    }

    /* cache all of the needed parameters for later continuation processing */
    s = talloc_zero(c, struct wbem_connect_context);
    if (composite_nomem(s, c)) return c;

    c->private_data = s;
    s->com_context = com_ctx;

    s->wszNetworkResource = talloc_strdup(s, nspace);
    if (composite_nomem(s->wszNetworkResource, c)) return c;

    if (locale != NULL)
    {
        s->wszPreferredLocale = talloc_strdup(s, locale);
        if (composite_nomem(s->wszPreferredLocale, c)) return c;
    }

    s->lFlags = flags;
    s->pCtx = wbem_ctx;

    /*
     * Create the parameters needed for the activation call: we need the CLSID
     * and IID for the IWbemLevel1Login interface we're after.
     */
    GUID_from_string(CLSID_WBEMLEVEL1LOGIN, &clsid);
    GUID_from_string(COM_IWBEMLEVEL1LOGIN_UUID, &iid);

    new_ctx = dcom_activate_send(c, &clsid, server, 1, &iid, com_ctx);
    if (composite_nomem(new_ctx, c)) return c;

    composite_continue(c, new_ctx, wbem_connect_server_continue, c);
    return c;
}

/*
 * Synchronously connect to a remote DCOM server and activate the IWbemServices
 * interface for WBEM work.
 */
WERROR WBEM_ConnectServer(struct com_context *ctx, const char *server,
        const char *nspace, const char *user, const char *password,
        const char *locale, uint32_t flags, const char *authority,
        struct IWbemContext* wbem_ctx, struct IWbemServices** services)
{
    struct composite_context *c = WBEM_ConnectServer_send(ctx,
            NULL,
            server,
            nspace,
            user,
            password,
            locale,
            flags,
            authority,
            wbem_ctx);
    return WBEM_ConnectServer_recv(c, NULL, services);
}

struct werror_code_struct {
        const char *dos_errstr;
        WERROR werror;
};

static const struct werror_code_struct wmi_errs[] =
{
	{ "RPC_S_CALL_FAILED", W_ERROR(RPC_S_CALL_FAILED) },

        { "WBEM_NO_ERROR", W_ERROR(WBEM_NO_ERROR) },
        { "WBEM_S_NO_ERROR", W_ERROR(WBEM_S_NO_ERROR) },
        { "WBEM_S_SAME", W_ERROR(WBEM_S_SAME) },
        { "WBEM_S_FALSE", W_ERROR(WBEM_S_FALSE) },
        { "WBEM_S_ALREADY_EXISTS", W_ERROR(WBEM_S_ALREADY_EXISTS) },
        { "WBEM_S_RESET_TO_DEFAULT", W_ERROR(WBEM_S_RESET_TO_DEFAULT) },
        { "WBEM_S_DIFFERENT", W_ERROR(WBEM_S_DIFFERENT) },
        { "WBEM_S_TIMEDOUT", W_ERROR(WBEM_S_TIMEDOUT) },
        { "WBEM_S_NO_MORE_DATA", W_ERROR(WBEM_S_NO_MORE_DATA) },
        { "WBEM_S_OPERATION_CANCELLED", W_ERROR(WBEM_S_OPERATION_CANCELLED) },
        { "WBEM_S_PENDING", W_ERROR(WBEM_S_PENDING) },
        { "WBEM_S_DUPLICATE_OBJECTS", W_ERROR(WBEM_S_DUPLICATE_OBJECTS) },
        { "WBEM_S_ACCESS_DENIED", W_ERROR(WBEM_S_ACCESS_DENIED) },
        { "WBEM_S_PARTIAL_RESULTS", W_ERROR(WBEM_S_PARTIAL_RESULTS) },
        { "WBEM_S_NO_POSTHOOK", W_ERROR(WBEM_S_NO_POSTHOOK) },
        { "WBEM_S_POSTHOOK_WITH_BOTH", W_ERROR(WBEM_S_POSTHOOK_WITH_BOTH) },
        { "WBEM_S_POSTHOOK_WITH_NEW", W_ERROR(WBEM_S_POSTHOOK_WITH_NEW) },
        { "WBEM_S_POSTHOOK_WITH_STATUS", W_ERROR(WBEM_S_POSTHOOK_WITH_STATUS) },
        { "WBEM_S_POSTHOOK_WITH_OLD", W_ERROR(WBEM_S_POSTHOOK_WITH_OLD) },
        { "WBEM_S_REDO_PREHOOK_WITH_ORIGINAL_OBJECT", W_ERROR(WBEM_S_REDO_PREHOOK_WITH_ORIGINAL_OBJECT) },
        { "WBEM_S_SOURCE_NOT_AVAILABLE", W_ERROR(WBEM_S_SOURCE_NOT_AVAILABLE) },
        { "WBEM_E_FAILED", W_ERROR(WBEM_E_FAILED) },
        { "WBEM_E_NOT_FOUND", W_ERROR(WBEM_E_NOT_FOUND) },
        { "WBEM_E_ACCESS_DENIED", W_ERROR(WBEM_E_ACCESS_DENIED) },
        { "WBEM_E_PROVIDER_FAILURE", W_ERROR(WBEM_E_PROVIDER_FAILURE) },
        { "WBEM_E_TYPE_MISMATCH", W_ERROR(WBEM_E_TYPE_MISMATCH) },
        { "WBEM_E_OUT_OF_MEMORY", W_ERROR(WBEM_E_OUT_OF_MEMORY) },
        { "WBEM_E_INVALID_CONTEXT", W_ERROR(WBEM_E_INVALID_CONTEXT) },
        { "WBEM_E_INVALID_PARAMETER", W_ERROR(WBEM_E_INVALID_PARAMETER) },
        { "WBEM_E_NOT_AVAILABLE", W_ERROR(WBEM_E_NOT_AVAILABLE) },
        { "WBEM_E_CRITICAL_ERROR", W_ERROR(WBEM_E_CRITICAL_ERROR) },
        { "WBEM_E_INVALID_STREAM", W_ERROR(WBEM_E_INVALID_STREAM) },
        { "WBEM_E_NOT_SUPPORTED", W_ERROR(WBEM_E_NOT_SUPPORTED) },
        { "WBEM_E_INVALID_SUPERCLASS", W_ERROR(WBEM_E_INVALID_SUPERCLASS) },
        { "WBEM_E_INVALID_NAMESPACE", W_ERROR(WBEM_E_INVALID_NAMESPACE) },
        { "WBEM_E_INVALID_OBJECT", W_ERROR(WBEM_E_INVALID_OBJECT) },
        { "WBEM_E_INVALID_CLASS", W_ERROR(WBEM_E_INVALID_CLASS) },
        { "WBEM_E_PROVIDER_NOT_FOUND", W_ERROR(WBEM_E_PROVIDER_NOT_FOUND) },
        { "WBEM_E_INVALID_PROVIDER_REGISTRATION", W_ERROR(WBEM_E_INVALID_PROVIDER_REGISTRATION) },
        { "WBEM_E_PROVIDER_LOAD_FAILURE", W_ERROR(WBEM_E_PROVIDER_LOAD_FAILURE) },
        { "WBEM_E_INITIALIZATION_FAILURE", W_ERROR(WBEM_E_INITIALIZATION_FAILURE) },
        { "WBEM_E_TRANSPORT_FAILURE", W_ERROR(WBEM_E_TRANSPORT_FAILURE) },
        { "WBEM_E_INVALID_OPERATION", W_ERROR(WBEM_E_INVALID_OPERATION) },
        { "WBEM_E_INVALID_QUERY", W_ERROR(WBEM_E_INVALID_QUERY) },
        { "WBEM_E_INVALID_QUERY_TYPE", W_ERROR(WBEM_E_INVALID_QUERY_TYPE) },
        { "WBEM_E_ALREADY_EXISTS", W_ERROR(WBEM_E_ALREADY_EXISTS) },
        { "WBEM_E_OVERRIDE_NOT_ALLOWED", W_ERROR(WBEM_E_OVERRIDE_NOT_ALLOWED) },
        { "WBEM_E_PROPAGATED_QUALIFIER", W_ERROR(WBEM_E_PROPAGATED_QUALIFIER) },
        { "WBEM_E_PROPAGATED_PROPERTY", W_ERROR(WBEM_E_PROPAGATED_PROPERTY) },
        { "WBEM_E_UNEXPECTED", W_ERROR(WBEM_E_UNEXPECTED) },
        { "WBEM_E_ILLEGAL_OPERATION", W_ERROR(WBEM_E_ILLEGAL_OPERATION) },
        { "WBEM_E_CANNOT_BE_KEY", W_ERROR(WBEM_E_CANNOT_BE_KEY) },
        { "WBEM_E_INCOMPLETE_CLASS", W_ERROR(WBEM_E_INCOMPLETE_CLASS) },
        { "WBEM_E_INVALID_SYNTAX", W_ERROR(WBEM_E_INVALID_SYNTAX) },
        { "WBEM_E_NONDECORATED_OBJECT", W_ERROR(WBEM_E_NONDECORATED_OBJECT) },
        { "WBEM_E_READ_ONLY", W_ERROR(WBEM_E_READ_ONLY) },
        { "WBEM_E_PROVIDER_NOT_CAPABLE", W_ERROR(WBEM_E_PROVIDER_NOT_CAPABLE) },
        { "WBEM_E_CLASS_HAS_CHILDREN", W_ERROR(WBEM_E_CLASS_HAS_CHILDREN) },
        { "WBEM_E_CLASS_HAS_INSTANCES", W_ERROR(WBEM_E_CLASS_HAS_INSTANCES) },
        { "WBEM_E_QUERY_NOT_IMPLEMENTED", W_ERROR(WBEM_E_QUERY_NOT_IMPLEMENTED) },
        { "WBEM_E_ILLEGAL_NULL", W_ERROR(WBEM_E_ILLEGAL_NULL) },
        { "WBEM_E_INVALID_QUALIFIER_TYPE", W_ERROR(WBEM_E_INVALID_QUALIFIER_TYPE) },
        { "WBEM_E_INVALID_PROPERTY_TYPE", W_ERROR(WBEM_E_INVALID_PROPERTY_TYPE) },
        { "WBEM_E_VALUE_OUT_OF_RANGE", W_ERROR(WBEM_E_VALUE_OUT_OF_RANGE) },
        { "WBEM_E_CANNOT_BE_SINGLETON", W_ERROR(WBEM_E_CANNOT_BE_SINGLETON) },
        { "WBEM_E_INVALID_CIM_TYPE", W_ERROR(WBEM_E_INVALID_CIM_TYPE) },
        { "WBEM_E_INVALID_METHOD", W_ERROR(WBEM_E_INVALID_METHOD) },
        { "WBEM_E_INVALID_METHOD_PARAMETERS", W_ERROR(WBEM_E_INVALID_METHOD_PARAMETERS) },
        { "WBEM_E_SYSTEM_PROPERTY", W_ERROR(WBEM_E_SYSTEM_PROPERTY) },
        { "WBEM_E_INVALID_PROPERTY", W_ERROR(WBEM_E_INVALID_PROPERTY) },
        { "WBEM_E_CALL_CANCELLED", W_ERROR(WBEM_E_CALL_CANCELLED) },
        { "WBEM_E_SHUTTING_DOWN", W_ERROR(WBEM_E_SHUTTING_DOWN) },
        { "WBEM_E_PROPAGATED_METHOD", W_ERROR(WBEM_E_PROPAGATED_METHOD) },
        { "WBEM_E_UNSUPPORTED_PARAMETER", W_ERROR(WBEM_E_UNSUPPORTED_PARAMETER) },
        { "WBEM_E_MISSING_PARAMETER_ID", W_ERROR(WBEM_E_MISSING_PARAMETER_ID) },
        { "WBEM_E_INVALID_PARAMETER_ID", W_ERROR(WBEM_E_INVALID_PARAMETER_ID) },
        { "WBEM_E_NONCONSECUTIVE_PARAMETER_IDS", W_ERROR(WBEM_E_NONCONSECUTIVE_PARAMETER_IDS) },
        { "WBEM_E_PARAMETER_ID_ON_RETVAL", W_ERROR(WBEM_E_PARAMETER_ID_ON_RETVAL) },
        { "WBEM_E_INVALID_OBJECT_PATH", W_ERROR(WBEM_E_INVALID_OBJECT_PATH) },
        { "WBEM_E_OUT_OF_DISK_SPACE", W_ERROR(WBEM_E_OUT_OF_DISK_SPACE) },
        { "WBEM_E_BUFFER_TOO_SMALL", W_ERROR(WBEM_E_BUFFER_TOO_SMALL) },
        { "WBEM_E_UNSUPPORTED_PUT_EXTENSION", W_ERROR(WBEM_E_UNSUPPORTED_PUT_EXTENSION) },
        { "WBEM_E_UNKNOWN_OBJECT_TYPE", W_ERROR(WBEM_E_UNKNOWN_OBJECT_TYPE) },
        { "WBEM_E_UNKNOWN_PACKET_TYPE", W_ERROR(WBEM_E_UNKNOWN_PACKET_TYPE) },
        { "WBEM_E_MARSHAL_VERSION_MISMATCH", W_ERROR(WBEM_E_MARSHAL_VERSION_MISMATCH) },
        { "WBEM_E_MARSHAL_INVALID_SIGNATURE", W_ERROR(WBEM_E_MARSHAL_INVALID_SIGNATURE) },
        { "WBEM_E_INVALID_QUALIFIER", W_ERROR(WBEM_E_INVALID_QUALIFIER) },
        { "WBEM_E_INVALID_DUPLICATE_PARAMETER", W_ERROR(WBEM_E_INVALID_DUPLICATE_PARAMETER) },
        { "WBEM_E_TOO_MUCH_DATA", W_ERROR(WBEM_E_TOO_MUCH_DATA) },
        { "WBEM_E_SERVER_TOO_BUSY", W_ERROR(WBEM_E_SERVER_TOO_BUSY) },
        { "WBEM_E_INVALID_FLAVOR", W_ERROR(WBEM_E_INVALID_FLAVOR) },
        { "WBEM_E_CIRCULAR_REFERENCE", W_ERROR(WBEM_E_CIRCULAR_REFERENCE) },
        { "WBEM_E_UNSUPPORTED_CLASS_UPDATE", W_ERROR(WBEM_E_UNSUPPORTED_CLASS_UPDATE) },
        { "WBEM_E_CANNOT_CHANGE_KEY_INHERITANCE", W_ERROR(WBEM_E_CANNOT_CHANGE_KEY_INHERITANCE) },
        { "WBEM_E_CANNOT_CHANGE_INDEX_INHERITANCE", W_ERROR(WBEM_E_CANNOT_CHANGE_INDEX_INHERITANCE) },
        { "WBEM_E_TOO_MANY_PROPERTIES", W_ERROR(WBEM_E_TOO_MANY_PROPERTIES) },
        { "WBEM_E_UPDATE_TYPE_MISMATCH", W_ERROR(WBEM_E_UPDATE_TYPE_MISMATCH) },
        { "WBEM_E_UPDATE_OVERRIDE_NOT_ALLOWED", W_ERROR(WBEM_E_UPDATE_OVERRIDE_NOT_ALLOWED) },
        { "WBEM_E_UPDATE_PROPAGATED_METHOD", W_ERROR(WBEM_E_UPDATE_PROPAGATED_METHOD) },
        { "WBEM_E_METHOD_NOT_IMPLEMENTED", W_ERROR(WBEM_E_METHOD_NOT_IMPLEMENTED) },
        { "WBEM_E_METHOD_DISABLED", W_ERROR(WBEM_E_METHOD_DISABLED) },
        { "WBEM_E_REFRESHER_BUSY", W_ERROR(WBEM_E_REFRESHER_BUSY) },
        { "WBEM_E_UNPARSABLE_QUERY", W_ERROR(WBEM_E_UNPARSABLE_QUERY) },
        { "WBEM_E_NOT_EVENT_CLASS", W_ERROR(WBEM_E_NOT_EVENT_CLASS) },
        { "WBEM_E_MISSING_GROUP_WITHIN", W_ERROR(WBEM_E_MISSING_GROUP_WITHIN) },
        { "WBEM_E_MISSING_AGGREGATION_LIST", W_ERROR(WBEM_E_MISSING_AGGREGATION_LIST) },
        { "WBEM_E_PROPERTY_NOT_AN_OBJECT", W_ERROR(WBEM_E_PROPERTY_NOT_AN_OBJECT) },
        { "WBEM_E_AGGREGATING_BY_OBJECT", W_ERROR(WBEM_E_AGGREGATING_BY_OBJECT) },
        { "WBEM_E_UNINTERPRETABLE_PROVIDER_QUERY", W_ERROR(WBEM_E_UNINTERPRETABLE_PROVIDER_QUERY) },
        { "WBEM_E_BACKUP_RESTORE_WINMGMT_RUNNING", W_ERROR(WBEM_E_BACKUP_RESTORE_WINMGMT_RUNNING) },
        { "WBEM_E_QUEUE_OVERFLOW", W_ERROR(WBEM_E_QUEUE_OVERFLOW) },
        { "WBEM_E_PRIVILEGE_NOT_HELD", W_ERROR(WBEM_E_PRIVILEGE_NOT_HELD) },
        { "WBEM_E_INVALID_OPERATOR", W_ERROR(WBEM_E_INVALID_OPERATOR) },
        { "WBEM_E_LOCAL_CREDENTIALS", W_ERROR(WBEM_E_LOCAL_CREDENTIALS) },
        { "WBEM_E_CANNOT_BE_ABSTRACT", W_ERROR(WBEM_E_CANNOT_BE_ABSTRACT) },
        { "WBEM_E_AMENDED_OBJECT", W_ERROR(WBEM_E_AMENDED_OBJECT) },
        { "WBEM_E_CLIENT_TOO_SLOW", W_ERROR(WBEM_E_CLIENT_TOO_SLOW) },
        { "WBEM_E_NULL_SECURITY_DESCRIPTOR", W_ERROR(WBEM_E_NULL_SECURITY_DESCRIPTOR) },
        { "WBEM_E_TIMED_OUT", W_ERROR(WBEM_E_TIMED_OUT) },
        { "WBEM_E_INVALID_ASSOCIATION", W_ERROR(WBEM_E_INVALID_ASSOCIATION) },
        { "WBEM_E_AMBIGUOUS_OPERATION", W_ERROR(WBEM_E_AMBIGUOUS_OPERATION) },
        { "WBEM_E_QUOTA_VIOLATION", W_ERROR(WBEM_E_QUOTA_VIOLATION) },
        { "WBEM_E_RESERVED_001", W_ERROR(WBEM_E_RESERVED_001) },
        { "WBEM_E_RESERVED_002", W_ERROR(WBEM_E_RESERVED_002) },
        { "WBEM_E_UNSUPPORTED_LOCALE", W_ERROR(WBEM_E_UNSUPPORTED_LOCALE) },
        { "WBEM_E_HANDLE_OUT_OF_DATE", W_ERROR(WBEM_E_HANDLE_OUT_OF_DATE) },
        { "WBEM_E_CONNECTION_FAILED", W_ERROR(WBEM_E_CONNECTION_FAILED) },
        { "WBEM_E_INVALID_HANDLE_REQUEST", W_ERROR(WBEM_E_INVALID_HANDLE_REQUEST) },
        { "WBEM_E_PROPERTY_NAME_TOO_WIDE", W_ERROR(WBEM_E_PROPERTY_NAME_TOO_WIDE) },
        { "WBEM_E_CLASS_NAME_TOO_WIDE", W_ERROR(WBEM_E_CLASS_NAME_TOO_WIDE) },
        { "WBEM_E_METHOD_NAME_TOO_WIDE", W_ERROR(WBEM_E_METHOD_NAME_TOO_WIDE) },
        { "WBEM_E_QUALIFIER_NAME_TOO_WIDE", W_ERROR(WBEM_E_QUALIFIER_NAME_TOO_WIDE) },
        { "WBEM_E_RERUN_COMMAND", W_ERROR(WBEM_E_RERUN_COMMAND) },
        { "WBEM_E_DATABASE_VER_MISMATCH", W_ERROR(WBEM_E_DATABASE_VER_MISMATCH) },
        { "WBEM_E_VETO_DELETE", W_ERROR(WBEM_E_VETO_DELETE) },
        { "WBEM_E_VETO_PUT", W_ERROR(WBEM_E_VETO_PUT) },
        { "WBEM_E_INVALID_LOCALE", W_ERROR(WBEM_E_INVALID_LOCALE) },
        { "WBEM_E_PROVIDER_SUSPENDED", W_ERROR(WBEM_E_PROVIDER_SUSPENDED) },
        { "WBEM_E_SYNCHRONIZATION_REQUIRED", W_ERROR(WBEM_E_SYNCHRONIZATION_REQUIRED) },
        { "WBEM_E_NO_SCHEMA", W_ERROR(WBEM_E_NO_SCHEMA) },
        { "WBEM_E_PROVIDER_ALREADY_REGISTERED", W_ERROR(WBEM_E_PROVIDER_ALREADY_REGISTERED) },
        { "WBEM_E_PROVIDER_NOT_REGISTERED", W_ERROR(WBEM_E_PROVIDER_NOT_REGISTERED) },
        { "WBEM_E_FATAL_TRANSPORT_ERROR", W_ERROR(WBEM_E_FATAL_TRANSPORT_ERROR) },
        { "WBEM_E_ENCRYPTED_CONNECTION_REQUIRED", W_ERROR(WBEM_E_ENCRYPTED_CONNECTION_REQUIRED) },
        { "WBEM_E_PROVIDER_TIMED_OUT", W_ERROR(WBEM_E_PROVIDER_TIMED_OUT) },
        { "WBEM_E_NO_KEY", W_ERROR(WBEM_E_NO_KEY) },
        { "WBEM_E_PROVIDER_DISABLED", W_ERROR(WBEM_E_PROVIDER_DISABLED) },
        { NULL, W_ERROR(0) }
};

const char *wmi_errstr(WERROR werror)
{
        int idx = 0;

        while (wmi_errs[idx].dos_errstr != NULL) {
                if (W_ERROR_V(wmi_errs[idx].werror) ==
                    W_ERROR_V(werror))
                        return wmi_errs[idx].dos_errstr;
                idx++;
        }

        return win_errstr(werror);
}
