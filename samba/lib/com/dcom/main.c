/*
 * SPDX-FileCopyrightText: 2006 Andrzej Hajda <andrzej.hajda@wp.pl>
 * SPDX-FileCopyrightText: 2004 Jelmer Vernooij <jelmer@samba.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
 Unix SMB/CIFS implementation.
 Main DCOM functionality
 Copyright (C) 2004 Jelmer Vernooij <jelmer@samba.org>
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

#include "includes.h"
#include "system/filesys.h"
#include "librpc/gen_ndr/epmapper.h"
#include "librpc/gen_ndr/ndr_remact_c.h"
#include "librpc/gen_ndr/com_dcom.h"
#include "librpc/gen_ndr/dcom.h"
#include "librpc/gen_ndr/ndr_oxidresolver.h"
#include "librpc/gen_ndr/ndr_oxidresolver_c.h"
#include "librpc/gen_ndr/ndr_remact.h"
#include "librpc/gen_ndr/ndr_remact_c.h"
#include "lib/com/dcom/dcom.h"
#include "librpc/rpc/dcerpc_table.h"
#include "lib/util/dlinklist.h"
#include "auth/credentials/credentials.h"
#include "libcli/composite/composite.h"

#include <ctype.h>

#define DCOM_NEGOTIATED_PROTOCOLS { EPM_PROTOCOL_TCP, EPM_PROTOCOL_SMB, EPM_PROTOCOL_NCALRPC }

/*
 * Structure used to keep track of the state of an asynchronous DCOM object
 * activation request.
 */
struct dcom_activation_state
{
    struct composite_context *parent_ctx; /* parent composite context */
    struct com_context *com_ctx; /* the COM context */
    struct GUID clsid; /* the requested class ID */
    struct GUID iid; /* the requester interface ID */
    int num_ifaces; /* the requested # of interfaces */
    const char *server; /* the server binding string */
    struct dcerpc_binding *binding; /* the resolved server binding */
    struct COMVERSION negotiated_version; /* the COM version negotiated */

    struct IUnknown **ip; /* receives requested interfaces */
    WERROR coresult; /* receives the COM result */
};

static NTSTATUS dcerpc_binding_from_STRINGBINDING(TALLOC_CTX *mem_ctx,
        struct dcerpc_binding **b_out, struct STRINGBINDING *bd)
{
    char *host, *endpoint;
    struct dcerpc_binding *b;

    b = talloc_zero(mem_ctx, struct dcerpc_binding);
    if (!b)
    {
        return NT_STATUS_NO_MEMORY;
    }

    b->transport = dcerpc_transport_by_endpoint_protocol(bd->wTowerId);

    if (b->transport == -1)
    {
        DEBUG(1, ("Can't find transport match endpoint protocol %d\n", bd->wTowerId));
        talloc_free(b);
        return NT_STATUS_NOT_SUPPORTED;
    }

    host = talloc_strdup(b, bd->NetworkAddr);
    endpoint = strchr(host, '[');

    if (endpoint)
    {
        *endpoint = '\0';
        endpoint++;

        endpoint[strlen(endpoint) - 1] = '\0';
    }

    b->host = host;
    b->endpoint = talloc_strdup(b, endpoint);

    *b_out = b;
    return NT_STATUS_OK;
}

struct cli_credentials *dcom_get_server_credentials(struct com_context *ctx,
        const char *server)
{
    struct dcom_server_credentials *c;
    struct cli_credentials *d;

    d = NULL;
    for (c = ctx->dcom->credentials; c; c = c->next)
    {
        if (c->server == NULL)
        {
            d = c->credentials;
            continue;
        }
        if (server && !strcmp(c->server, server))
            return c->credentials;
    }
    return d;
}

void dcom_set_server_credentials(struct com_context *ctx, const char *server,
        struct cli_credentials *credentials)
{
    struct dcom_server_credentials *c;

    for (c = ctx->dcom->credentials; c; c = c->next)
    {
        if ((server == NULL && c->server == NULL) || (server && c->server
                && !strcmp(c->server, server)))
        {
            if (c->credentials && c->credentials != credentials)
            {
                talloc_unlink(c, c->credentials);
                c->credentials = credentials;
                if (talloc_find_parent_bytype(c->credentials, struct dcom_server_credentials))
                    (void) talloc_reference(c, c->credentials);
                else
                    talloc_steal(c, c->credentials);
            }
            return;
        }
    }
    c = talloc(ctx->event_ctx, struct dcom_server_credentials);
    c->server = talloc_strdup(c, server);
    c->credentials = credentials;
    if (talloc_find_parent_bytype(c->credentials, struct dcom_server_credentials))
        (void) talloc_reference(c, c->credentials);
    else
        talloc_steal(c, c->credentials);
    DLIST_ADD(ctx->dcom->credentials, c);
}

void dcom_update_credentials_for_aliases(struct com_context *ctx,
        const char *server, struct DUALSTRINGARRAY *pds)
{
    struct cli_credentials *cc;
    struct dcerpc_binding *b;
    uint32_t i;
    NTSTATUS status;

    cc = dcom_get_server_credentials(ctx, server);
    for (i = 0; pds->stringbindings[i]; ++i)
    {
        if (pds->stringbindings[i]->wTowerId != EPM_PROTOCOL_TCP)
            continue;
        status = dcerpc_binding_from_STRINGBINDING(ctx, &b,
                pds->stringbindings[i]);
        if (!NT_STATUS_IS_OK(status))
            continue;
        dcom_set_server_credentials(ctx, b->host, cc);
        talloc_free(b);
    }
}

struct dcom_client_context *dcom_client_init(struct com_context *ctx,
        struct cli_credentials *credentials)
{
    ctx->dcom = talloc_zero(ctx, struct dcom_client_context);
    if (!credentials)
    {
        credentials = cli_credentials_init(ctx);
        cli_credentials_set_conf(credentials);
        cli_credentials_parse_string(credentials, "%", CRED_SPECIFIED);
    }
    dcom_set_server_credentials(ctx, NULL, credentials);
    return ctx->dcom;
}

static void dcom_update_server(struct dcom_object_exporter *m,
        const char *server)
{
    char *c = strchr(server, '[');

    if (m->host)
        talloc_free(m->host);

    m->host = c ? talloc_strndup(m, server, c - server)
            : talloc_strdup(m, server);
}

/*
 * Complete a remote activation request by receiving the final results and
 * updating the updating the local object exporter as needed.
 */
static void remote_activation_complete(struct rpc_request *rpc_req)
{
    struct composite_context *c = NULL;
    struct RemoteActivation* r = NULL;
    struct dcom_activation_state *s = NULL;
    struct dcom_object_exporter *m = NULL;
    struct IUnknown *ru_template = NULL;
    WERROR result;
    int i;

    /* retrieve the parent composite context */
    c = talloc_get_type(rpc_req->async.private, struct composite_context);
    if (!NT_STATUS_IS_OK(rpc_req->status))
    {
        composite_error(c, rpc_req->status);
        return;
    }

    /* retrieve pointers to the RemoteActivation output and our state data */
    r = talloc_get_type(rpc_req->ndr.struct_ptr, struct RemoteActivation);
    s = talloc_get_type(c->private_data, struct dcom_activation_state);

    /* receive the results of the RPC request and display if requested */
    c->status = dcerpc_ndr_request_recv(rpc_req);
    if (!composite_is_ok(c)) return;

    if (DEBUGLVL(9))
        NDR_PRINT_OUT_DEBUG(RemoteActivation, r);

    /* determine if the remote activation request was successful */
    result = *(r->out.hr);
    if (!W_ERROR_IS_OK(result))
    {
        composite_error(c, werror_to_ntstatus(result));
        return;
    }

    m = object_exporter_update_oxid(s->com_ctx,
            *(r->out.pOxid), *(r->out.pdsaOxidBindings));

    /* build the return interface pointers from the results. */
    s->ip = talloc_array(c, struct IUnknown *, s->num_ifaces);
    if (composite_nomem(s->ip, c)) return;
    for (i = 0; i < s->num_ifaces; i++)
    {
        s->ip[i] = NULL;

        /*
         * determine if each individual interface was successfully returned; in
         * some cases the overall request may be successful but certain
         * interfaces may not be available.
         */
        if (W_ERROR_IS_OK(r->out.results[i]))
        {
            NTSTATUS status = dcom_IUnknown_from_OBJREF(s->com_ctx,
                    &(s->ip[i]), &(r->out.ifaces[i]->obj));
            if (!NT_STATUS_IS_OK(status))
            {
                r->out.results[i] = ntstatus_to_werror(status);
            }
            else if (!ru_template)
            {
                ru_template = s->ip[i];
            }
        }
    }

    /*
     * do something with the object exporter?! this whole section of code
     * desperately needs a rewrite and clarity.
     */
    /* TODO:avg check when exactly oxid should be updated,its lifetime etc */
    if (m->rem_unknown && memcmp(
            &m->rem_unknown->obj.u_objref.u_standard.std.ipid,
            r->out.ipidRemUnknown, sizeof(*(r->out.ipidRemUnknown))))
    {
        talloc_free(m->rem_unknown);
        m->rem_unknown = NULL;
    }
    if (!m->rem_unknown)
    {
        if (!ru_template)
        {
            DEBUG(1,("dcom_activate: Cannot Create IRemUnknown - template interface not available\n"));
            result = WERR_GENERAL_FAILURE;
            // TODO: notice how this failure doesn't get sent back to the caller?
        }
        m->rem_unknown = talloc_zero(m, struct IRemUnknown);
        memcpy(m->rem_unknown, ru_template, sizeof(struct IUnknown));
        GUID_from_string(COM_IREMUNKNOWN_UUID, &m->rem_unknown->obj.iid);
        m->rem_unknown->obj.u_objref.u_standard.std.ipid
                = *(r->out.ipidRemUnknown);
        m->rem_unknown->vtable
                = (struct IRemUnknown_vtable *) dcom_proxy_vtable_by_iid(
                        &m->rem_unknown->obj.iid);
        /* TODO:avg copy stringbindigs?? */
    }

    dcom_update_credentials_for_aliases(s->com_ctx, s->server,
            *(r->out.pdsaOxidBindings));
    dcom_update_server(m, s->server);

    composite_done(c);
}

/*
 * Continue the remote activation request following a connect by issuing the
 * RemoteActivation call after all the preliminary negotiation has been
 * completed.
 */
static void remote_activation_continue(struct composite_context *ctx)
{
    static uint16_t protseq[] = DCOM_NEGOTIATED_PROTOCOLS;

    struct composite_context *c = NULL;
    struct dcom_activation_state *s = NULL;
    struct dcerpc_pipe *p = NULL;
    struct RemoteActivation *r = NULL;
    struct rpc_request *rpc_req = NULL;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);
    if (!composite_is_ok(c)) return;

    /* retrieve the activation state data */
    s = talloc_get_type(c->private_data, struct dcom_activation_state);

    /*
     * complete the pipe connect and receive a pointer to the dcerpc_pipe
     * structure
     */
    c->status = dcerpc_pipe_connect_b_recv(ctx, c, &p);
    if (!composite_is_ok(c)) return;

    /*
     * Prepare arguments for the RemoteActivation call. Refer to 3.1.4.1.1.2
     * "Issuing the Activation Request" in [MS-DCOM] for more information.
     */
    r = talloc_zero(c, struct RemoteActivation);
    if (composite_nomem(r, c)) return;

    r->in.this.version = s->negotiated_version;
    r->in.this.cid = GUID_random(); /* generate a new causality identifier */
    r->in.ClientImpLevel = RPC_C_IMP_LEVEL_IDENTIFY;
    r->in.Clsid = s->clsid;
    r->in.Mode = 0; /* we do not require an interface to the class factory */
    r->in.protseq = protseq;
    r->in.num_protseqs = ARRAY_SIZE(protseq);
    r->in.Interfaces = s->num_ifaces;
    r->in.pIIDs = &s->iid;

    r->out.that = talloc_zero(r, struct ORPCTHAT);
    if (composite_nomem(r->out.that, c)) return;

    r->out.pOxid = talloc_zero(r, uint64_t);
    if (composite_nomem(r->out.pOxid, c)) return;

    r->out.pdsaOxidBindings = talloc_zero(r, struct DUALSTRINGARRAY *);
    if (composite_nomem(r->out.pdsaOxidBindings, c)) return;

    r->out.ipidRemUnknown = talloc_zero(r, struct GUID);
    if (composite_nomem(r->out.ipidRemUnknown, c)) return;

    r->out.AuthnHint = talloc_zero(r, uint32_t);
    if (composite_nomem(r->out.AuthnHint, c)) return;

    r->out.ServerVersion = talloc_zero(r, struct COMVERSION);
    if (composite_nomem(r->out.ServerVersion, c)) return;

    r->out.hr = talloc_zero(r, WERROR);
    if (composite_nomem(r->out.hr, c)) return;

    r->out.ifaces = talloc_array(r, struct MInterfacePointer *, s->num_ifaces);
    if (composite_nomem(r->out.ifaces, c)) return;

    r->out.results = &s->coresult;

    /*
     * Send the asynchronous RemoteActivation request and setup the
     * RPC continuation callback.
     */
    rpc_req = dcerpc_RemoteActivation_send(p, c, r);
    if (composite_nomem(rpc_req, c)) return;

    composite_continue_rpc(c, rpc_req, remote_activation_complete, c);
}

/**
 * Continue the RPC IOXIDResolver:ServerAlive call by receiving the
 * response and processing the results.
 */
static void determine_rpc_binding_continue2(struct rpc_request *rpc_ctx)
{
    struct composite_context *c = NULL;
    struct dcom_activation_state *s = NULL;
    struct ServerAlive* r = NULL;
    struct cli_credentials *creds = NULL;
    struct composite_context *pipe_conn_req = NULL;
    NTSTATUS status;

    /* retrieve the parent composite context */
    c = talloc_get_type(rpc_ctx->async.private, struct composite_context);

    /* retrieve a pointer to the results structure and our state data */
    s = talloc_get_type(c->private_data, struct dcom_activation_state);
    r = talloc_get_type(rpc_ctx->ndr.struct_ptr, struct ServerAlive);

    /* retrieve the results of the RPC request */
    c->status = dcerpc_ndr_request_recv(rpc_ctx);
    DEBUG(3, ("dcerpc_ndr_request_recv returned %s\n", nt_errstr(c->status)));
    if (!composite_is_ok(c)) return;

    status = werror_to_ntstatus(r->out.result);
    DEBUG(3, ("IObjectExporter::ServerAlive returned %s\n", nt_errstr(status)));
    if (!W_ERROR_IS_OK(r->out.result))
    {
        composite_error(c, status);
    }
    if (!composite_is_ok(c)) return;

    // lowest common denominator here; ideally we'd just call ServerAlive2
    // and do better negotiation on our end...
    s->negotiated_version.MajorVersion = COM_MAJOR_VERSION;
    s->negotiated_version.MinorVersion = COM_MINOR_VERSION;

    /*
     * Proceed with the activating request using the same binding that was
     * successful for the ServerAlive call and try to retrieve a pipe to
     * the IRemoteActivation interface.
     */
    creds = dcom_get_server_credentials(s->com_ctx, s->server);
    pipe_conn_req = dcerpc_pipe_connect_b_send(c,
            s->binding, &dcerpc_table_IRemoteActivation, creds, c->event_ctx);

    composite_continue(c, pipe_conn_req, remote_activation_continue, c);
}

/*
 * Continue the RPC connect after a successful socket open to the server by
 * receiving the results and then attempting to send a
 * IOXIDResolver::ServerAlive call if the connect was successful.
 */
static void determine_rpc_binding_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct dcerpc_pipe* p = NULL;
    struct ServerAlive* r = NULL;
    struct rpc_request* rpc_ctx = NULL;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    /*
     * complete the pipe connect and receive a pointer to the dcerpc_pipe
     * structure. a failure here is most likely that no RPC server is listening
     * on the specified host & port.
     */
    c->status = dcerpc_pipe_connect_b_recv(ctx, c, &p);
    if (!composite_is_ok(c)) return;

    /* prepare arguments for the ServerAlive call and send it off */
    r = talloc_zero(c, struct ServerAlive);
    if (composite_nomem(r, c)) return;

    rpc_ctx = dcerpc_ServerAlive_send(p, c, r);
    if (composite_nomem(rpc_ctx, c)) return;

    composite_continue_rpc(c, rpc_ctx, determine_rpc_binding_continue2, c);
}

/*
 * 3.1.4.1.1.1   Determining RPC Binding Information for Activation
 * The client MUST call the IObjectExporter::ServerAlive2 method using the first
 * RPC protocol sequence listed in section 3.2.2.3, unless the client COMVERSION
 * is less than 5.6, in which case the client MUST call the
 * IObjectExporter::ServerAlive method. The client MUST specify the RPC endpoint
 * information to contain the remote server name on which the application wants
 * to activate the object and the well-known endpoint of the object resolver.
 *
 * In our case, we're just assuming the lowest common denominator and acting
 * like client version 5.1, so we just call ServerAlive instead of ServerAlive2.
 */
static struct composite_context *dcom_determine_rpc_binding(
        struct composite_context *parent_ctx, const char* server,
        struct dcom_activation_state *activation_state, void *private_data)
{
    struct composite_context *c = NULL;
    struct cli_credentials* creds = NULL;
    struct composite_context *pipe_conn_req = NULL;
    NTSTATUS status;

    /* composite context allocation and setup */
    c = composite_create(parent_ctx, parent_ctx->event_ctx);
    if (c == NULL) return NULL;
    c->private_data = activation_state;

    /*
     * determine if the caller specified a server binding, and if not use
     * the default
     */
    status = dcerpc_parse_binding(c, server, &activation_state->binding);
    if (!NT_STATUS_IS_OK(status))
    {
        /* build a binding string using NCACN_IP_TCP */
        char *bindstr = talloc_asprintf(c, "ncacn_ip_tcp:%s[sign,seal]", server);
        if (composite_nomem(bindstr, c)) return c;

        status = dcerpc_parse_binding(c, bindstr, &activation_state->binding);
        if (!NT_STATUS_IS_OK(status))
        {
            DEBUG(0, ("Failed to parse dcerpc binding '%s'\n", bindstr));
            composite_error(c, status);
            return c;
        }
    }
    else if (activation_state->binding->transport != NCACN_IP_TCP)
    {
        DEBUG(0, ("Invalid transport specified, only NCACN_IP_TCP "
                        "supported for DCOM object activation\n"));
        composite_error(c, NT_STATUS_RPC_PROTSEQ_NOT_SUPPORTED);
        return c;
    }

    DEBUG(3, ("Using binding %s\n", dcerpc_binding_string(c,
                            activation_state->binding)));

    /*
     * create an empty set of credentials since an
     * IOXIDResolver:ServerAlive call should never have any authentication
     * specified
     */
    creds = talloc_zero(c, struct cli_credentials);
    if (composite_nomem(creds, c)) return c;
    creds->username = "";
    creds->password = "";

    /*
     * start connecting to a rpc pipe after binding structure
     * is established
     */
    pipe_conn_req = dcerpc_pipe_connect_b_send(c,
            activation_state->binding, &dcerpc_table_IOXIDResolver, creds,
            parent_ctx->event_ctx);
    if (composite_nomem(pipe_conn_req, c)) return c;

    composite_continue(c, pipe_conn_req, determine_rpc_binding_continue, c);
    return c;
}

/*
 * Complete an activation request and continue the composite chain.
 */
static void complete_activation(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct dcom_activation_state *s = NULL;

    c = talloc_get_type(ctx->async.private_data, struct composite_context);
    c->status = ctx->status;
    if (!composite_is_ok(c)) return;

    s = talloc_get_type(ctx->private_data, struct dcom_activation_state);

    DEBUG(3, ("Negotiated COM version: %d.%d using binding %s\n",
            s->negotiated_version.MajorVersion,
            s->negotiated_version.MinorVersion,
            dcerpc_binding_string(c, s->binding)));

    composite_done(c);
}

/*
 * Begin an asynchronous DCOM object activation request.
 *
 * parent_ctx       the parent memory context for the request
 * clsid            the class ID requested
 * server           the binding string or server name
 * num_ifaces       the number of interfaces requested
 * iid              the interface ID requested
 * com_ctx          the COM context to use
 */
struct composite_context *dcom_activate_send(TALLOC_CTX *parent_ctx,
        const struct GUID *clsid, const char *server, int num_ifaces,
        const struct GUID *iid, struct com_context *com_ctx)
{
    struct composite_context *c = NULL;
    struct dcom_activation_state *s = NULL;
    struct composite_context *binding_ctx = NULL;

    /* composite context allocation and setup */
    c = composite_create(parent_ctx, com_ctx->event_ctx);
    if (c == NULL) return NULL;

    s = talloc_zero(c, struct dcom_activation_state);
    if (composite_nomem(s, c)) return c;

    c->private_data = s;
    s->parent_ctx = parent_ctx;
    s->com_ctx = com_ctx;
    s->server = server;
    s->clsid = *clsid;
    s->iid = *iid;
    s->num_ifaces = num_ifaces;

    /*
     * Begin the DCOM object activation by first attempting to determine the
     * correct RPC binding to use and what COM version should be used. This
     * requires sending a non-authenticated IOXIDResolver:ServerAlive RPC call
     * to the requested server and handling the results. See section 3.1.4.1.1
     * of [MS-DCOM] for further information.
     */
    binding_ctx = dcom_determine_rpc_binding(c, server, s, c);
    if (composite_nomem(binding_ctx, c)) return c;

    composite_continue(c, binding_ctx, complete_activation, c);
    return c;
}

/*
 * Complete an asynchronous DCOM activation request by receiving the results
 * and providing the IUknown interface pointers back to the caller.
 */
NTSTATUS dcom_activate_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct IUnknown ***interfaces)
{
    NTSTATUS status = composite_wait(c);
    if (NT_STATUS_IS_OK(status))
    {
        struct dcom_activation_state *s = talloc_get_type(c->private_data,
                struct dcom_activation_state);

        talloc_steal(parent_ctx, s->ip);
        *interfaces = s->ip;

        status = werror_to_ntstatus(s->coresult);
    }

    talloc_free(c);

    return status;
}

/*
 * Synchronous DCOM activation request.
 */
NTSTATUS dcom_activate(struct com_context *com_ctx, TALLOC_CTX *parent_ctx,
        const char* server, struct GUID *clsid, struct GUID *iid,
        int num_ifaces, struct IUnknown ***interfaces)
{
    struct composite_context *c = dcom_activate_send(parent_ctx, clsid, server,
            num_ifaces, iid, com_ctx);
    return dcom_activate_recv(c, parent_ctx, interfaces);
}

struct dcom_object_exporter *object_exporter_by_oxid(struct com_context *ctx,
        uint64_t oxid)
{
    struct dcom_object_exporter *ox;
    for (ox = ctx->dcom->object_exporters; ox; ox = ox->next)
    {
        if (ox->oxid == oxid)
        {
            return ox;
        }
    }

    return NULL;
}

struct dcom_object_exporter *object_exporter_update_oxid(
        struct com_context *ctx, uint64_t oxid,
        struct DUALSTRINGARRAY *bindings)
{
    struct dcom_object_exporter *ox;
    ox = object_exporter_by_oxid(ctx, oxid);
    if (!ox)
    {
        ox = talloc_zero(ctx, struct dcom_object_exporter);
        DLIST_ADD(ctx->dcom->object_exporters, ox);
        ox->oxid = oxid;
    }
    else
    {
        talloc_free(ox->bindings);
    }
    ox->bindings = bindings;
    talloc_steal(ox, bindings);
    return ox;
}

struct query_interface_state
{
    struct dcom_object_exporter *ox;
    struct REMQIRESULT *rqir;
    uint16_t cIids;
    struct GUID *iids;
    struct IUnknown **ips;
    WERROR result;
};

struct dcom_object_exporter *object_exporter_by_ip(struct com_context *ctx,
        struct IUnknown *ip)
{
    return object_exporter_by_oxid(ctx, ip->obj.u_objref.u_standard.std.oxid);
}

static void query_interface_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct query_interface_state *s = NULL;
    WERROR result;

    /* retrieve the parent composite context */
    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    s = talloc_get_type(c->private_data, struct query_interface_state);

    result = IRemUnknown_RemQueryInterface_recv(ctx, &s->rqir);
    if (W_ERROR_IS_OK(result))
    {
        int i;
        struct IUnknown ru;
        NTSTATUS status;

        s->ips = talloc_array(c, struct IUnknown *, s->cIids);
        if (composite_nomem(s->ips, c)) return;

        ru = *(struct IUnknown *) s->ox->rem_unknown;
        for (i = 0; i < s->cIids; i++)
        {
            s->ips[i] = NULL;
            if (W_ERROR_IS_OK(s->rqir[i].hResult))
            {
                ru.obj.iid = s->iids[i];
                ru.obj.u_objref.u_standard.std = s->rqir[i].std;
                status = dcom_IUnknown_from_OBJREF(s->ox->rem_unknown->ctx,
                        &(s->ips[i]), &ru.obj);
                if (!NT_STATUS_IS_OK(status))
                {
                    /*
                     * TODO: the old code used an array of status result values
                     * separately from the overall status. Do we really need
                     * such a distinction?
                     */
                    result = ntstatus_to_werror(status);
                }
            }
        }
    }

    s->result = result;
    c->status = werror_to_ntstatus(result);
    composite_done(c);
}

NTSTATUS dcom_query_interface_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct IUnknown ***interfaces)
{
    NTSTATUS status = composite_wait(c);
    if (NT_STATUS_IS_OK(status))
    {
        struct query_interface_state *s = NULL;

        s = talloc_get_type(c->private_data, struct query_interface_state);

        talloc_steal(parent_ctx, s->ips);
        *interfaces = s->ips;

        status = werror_to_ntstatus(s->result);
    }

    talloc_free(c);

    return status;
}

struct composite_context *dcom_query_interface_send(struct IUnknown *d,
        TALLOC_CTX *parent_ctx, uint32_t cRefs, uint16_t cIids,
        struct GUID *iids)
{
    struct composite_context *c;
    struct composite_context *new_ctx;
    struct query_interface_state *s;

    c = composite_create(parent_ctx, d->ctx->event_ctx);
    if (c == NULL) return NULL;

    s = talloc_zero(c, struct query_interface_state);
    if (composite_nomem(s, c)) return c;
    c->private_data = s;

    s->ox = object_exporter_by_ip(d->ctx, d);
    s->cIids = cIids;
    s->iids = talloc_memdup(s, iids, sizeof(struct GUID) * cIids);
    if (composite_nomem(s->iids, c)) return c;

    new_ctx = IRemUnknown_RemQueryInterface_send(s->ox->rem_unknown, c,
            &IUnknown_ipid(d), cRefs, cIids, s->iids);
    if (composite_nomem(new_ctx, c)) return c;

    composite_continue(c, new_ctx, query_interface_continue, c);
    return c;
}

NTSTATUS dcom_query_interface(struct IUnknown *d, TALLOC_CTX *parent_ctx,
        uint32_t cRefs, uint16_t cIids, struct GUID *iids,
        struct IUnknown ***interfaces)
{
    struct composite_context *c = dcom_query_interface_send(d, parent_ctx,
            cRefs, cIids, iids);
    return dcom_query_interface_recv(c, parent_ctx, interfaces);
}

/*
 * Returns true if the provided string is an numeric IP address binding string.
 */
static int is_ip_binding(const char* s)
{
    while (*s && (*s != '['))
    {
        if (isdigit(*s) || *s == '.')
            ++s;
        else
            return 0;
    }
    return 1;
}

/*
 * Attempt to find a STRINGBINDING that is similar to the specified host name.
 */
static int find_similar_binding(struct STRINGBINDING **sb, const char *host)
{
    const size_t host_len = strlen(host);
    int i;

    for (i = 0; sb[i] != NULL; ++i)
    {
        /* only look @ TCP/IP bindings */
        if (sb[i]->wTowerId == EPM_PROTOCOL_TCP)
        {
            const char *s = strchr(sb[i]->NetworkAddr, '[');
            if (s != NULL)
            {
                /*
                 * see if the hostnames match, but use the shorter of the two
                 * names in case the user provided a FQDN but the STRINGBINDING
                 * is only the hostname (the common case)
                 */
                const size_t addr_len = s - sb[i]->NetworkAddr;
                const size_t n = (addr_len < host_len) ? addr_len : host_len;
                if (strncasecmp(host, sb[i]->NetworkAddr, n) == 0) break;
            }
        }
    }

    return i;
}

NTSTATUS dcom_OBJREF_from_IUnknown(struct OBJREF *o, struct IUnknown *p)
{
    /* FIXME: Cache generated objref objects? */
    ZERO_STRUCTP(o);

    if (!p)
    {
        o->signature = OBJREF_SIGNATURE;
        o->flags = OBJREF_NULL;
    }
    else
    {
        *o = p->obj;
        switch (o->flags)
        {
        case OBJREF_CUSTOM:
        {
            marshal_fn marshal;

            marshal = dcom_marshal_by_clsid(&o->u_objref.u_custom.clsid);
            if (marshal)
            {
                return marshal(p, o);
            }
            else
            {
                return NT_STATUS_NOT_SUPPORTED;
            }
        }
        }
    }

    return NT_STATUS_OK;
}

NTSTATUS dcom_IUnknown_from_OBJREF(struct com_context *ctx,
        struct IUnknown **_p, struct OBJREF *o)
{
    struct IUnknown *p;
    struct dcom_object_exporter *ox;
    unmarshal_fn unmarshal;

    switch (o->flags)
    {
    case OBJREF_NULL:
        *_p = NULL;
        return NT_STATUS_OK;

    case OBJREF_STANDARD:
        p = talloc_zero(ctx, struct IUnknown);
        p->ctx = ctx;
        p->obj = *o;
        p->vtable = dcom_proxy_vtable_by_iid(&o->iid);

        if (!p->vtable)
        {
            DEBUG(0, ("Unable to find proxy class for interface with IID %s\n", GUID_string(ctx, &o->iid)));
            return NT_STATUS_NOT_SUPPORTED;
        }

        p->vtable->Release_send = dcom_release_send;

        ox = object_exporter_by_oxid(ctx, o->u_objref.u_standard.std.oxid);
        /* FIXME: Add object to list of objects to ping */
        *_p = p;
        return NT_STATUS_OK;

    case OBJREF_HANDLER:
        p = talloc_zero(ctx, struct IUnknown);
        p->ctx = ctx;
        p->obj = *o;
        ox = object_exporter_by_oxid(ctx, o->u_objref.u_handler.std.oxid);
        /* FIXME: Add object to list of objects to ping */
        /*FIXME		p->vtable = dcom_vtable_by_clsid(&o->u_objref.u_handler.clsid);*/
        /* FIXME: Do the custom unmarshaling call */

        *_p = p;
        return NT_STATUS_NOT_SUPPORTED;

    case OBJREF_CUSTOM:
        p = talloc_zero(ctx, struct IUnknown);
        p->ctx = ctx;
        p->vtable = NULL;
        p->obj = *o;
        unmarshal = dcom_unmarshal_by_clsid(&o->u_objref.u_custom.clsid);
        *_p = p;
        if (unmarshal)
        {
            return unmarshal(o, _p);
        }
        else
        {
            return NT_STATUS_NOT_SUPPORTED;
        }
    }

    return NT_STATUS_NOT_SUPPORTED;
}

uint64_t dcom_get_current_oxid(void)
{
    return getpid();
}

/*
 * State structure used for asynchronous dcom_get_pipe calls.
 */
struct dcom_get_pipe_state
{
    struct IUnknown *iface;             /* the requested interface */
    struct dcom_object_exporter *ox;    /* the object exporter for it */
    struct dcerpc_pipe *p;              /* the final pipe */

    int similar_binding_index;
    int current_binding_index;
    int current_binding_offset;
};

/*
 * Complete the alter_context PDU request for an existing pipe so it can be
 * used again for the requested interface.
 */
static void reuse_existing_pipe_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;

    c = talloc_get_type(ctx->async.private_data, struct composite_context);

    /* nothing to do here except pass along the status of the alter context */
    c->status = dcerpc_alter_context_recv(ctx);
    if (!composite_is_ok(c)) return;

    composite_done(c);
}

/*
 * Reuse an existing pipe already on the object exporter. If the pipe isn't
 * presently bound to the GUID for the requested interface then an alter_context
 * PDU request is sent off, otherwise the pipe is used as-is.
 */
static void reuse_existing_pipe(struct composite_context *c)
{
    struct composite_context *new_ctx = NULL;
    struct dcom_get_pipe_state *s = NULL;
    struct dcerpc_pipe *p = NULL;

    s = talloc_get_type(c->private_data, struct dcom_get_pipe_state);
    p = s->p = s->ox->pipe;

    if (!GUID_equal(&p->syntax.uuid, &s->iface->vtable->iid))
    {
        s->ox->pipe->syntax.uuid = s->iface->vtable->iid;

        new_ctx = dcerpc_alter_context_send(p, c,
                &idl_iface_by_uuid(&s->iface->vtable->iid)->syntax_id,
                &p->transfer_syntax);
        if (composite_nomem(new_ctx, c)) return;

        composite_continue(c, new_ctx, reuse_existing_pipe_continue, c);
    }
    else
    {
        c->status = NT_STATUS_OK;
        composite_done(c);
    }
}

/* forward reference */
static void try_next_binding(struct composite_context *c,
        struct dcom_get_pipe_state *s);

/*
 * Continues a new pipe binding request by determining if the specific binding
 * attempt was successful.
 */
static void bind_new_pipe_continue(struct composite_context *ctx)
{
    struct composite_context *c = NULL;
    struct dcom_get_pipe_state *s = NULL;
    struct dcerpc_pipe *p = NULL;
    const struct STRINGBINDING *sb = NULL;
    NTSTATUS status;

    c = talloc_get_type(ctx->async.private_data, struct composite_context);
    s = talloc_get_type(c->private_data, struct dcom_get_pipe_state);
    sb = s->ox->bindings->stringbindings[s->current_binding_offset];

    status = dcerpc_pipe_connect_b_recv(ctx, c, &p);
    if (!NT_STATUS_IS_OK(status))
    {
        DEBUG(9, ("Unable to bind to %s: %s\n", sb->NetworkAddr,
                nt_errstr(status)));
        try_next_binding(c, s);
    }
    else
    {
        DEBUG(9, ("bind_new_pipe_continue: successfully bound to %s\n",
                dcerpc_binding_string(c, p->binding)));
        s->p = p;
        composite_done(c);
    }
}

/*
 * Try to bind to the next available STRINGBINDING until we finally run out of
 * them.
 */
static void try_next_binding(struct composite_context *c,
        struct dcom_get_pipe_state *s)
{
    struct STRINGBINDING **bindings = s->ox->bindings->stringbindings;
    struct STRINGBINDING *sb = NULL;
    int try_index = -1;

    /* repeat our loop until we reach the end OR we find a binding to try */
    while (bindings[s->current_binding_index++] != NULL)
    {
        /* wrap the offset around if we've reached the end of the bindings */
        if (bindings[++s->current_binding_offset] == NULL)
            s->current_binding_offset = 0;

        sb = bindings[s->current_binding_offset];

        /* let's try the similar binding as soon as we see it */
        if (s->current_binding_offset == s->similar_binding_index)
        {
            try_index = s->current_binding_offset;
            break;
        }

        /*
         * otherwise, if we've got a non-TCP/IP binding let's just skip it
         */
        if (sb->wTowerId != EPM_PROTOCOL_TCP)
        {
            DEBUG(3, ("dcom_get_pipe: Skipping binding %s\n", sb->NetworkAddr));
            continue;
        }

        /* hey, a binding to try! */
        try_index = s->current_binding_offset;
        break;
    }

    if (try_index != -1)
    {
        struct composite_context *new_ctx = NULL;
        struct dcerpc_binding *binding = NULL;
        NTSTATUS status;

        DEBUG(9, ("dcom_get_pipe: Trying binding %s\n", sb->NetworkAddr));
        status = dcerpc_binding_from_STRINGBINDING(s->iface->ctx, &binding, sb);
        if (!NT_STATUS_IS_OK(status))
        {
            DEBUG(1, ("Error parsing string binding %s: %s\n", sb->NetworkAddr,
                    nt_errstr(status)));
            try_next_binding(c, s);
        }
        else
        {
            binding->flags |= DCERPC_AUTH_NTLM | DCERPC_SIGN;
            if (DEBUGLVL(9)) binding->flags |= DCERPC_DEBUG_PRINT_BOTH;

            new_ctx = dcerpc_pipe_connect_b_send(c, binding,
                    idl_iface_by_uuid(&s->iface->obj.iid),
                    dcom_get_server_credentials(s->iface->ctx, binding->host),
                    s->iface->ctx->event_ctx);

            if (!composite_nomem(new_ctx, c))
                composite_continue(c, new_ctx, bind_new_pipe_continue, c);
        }
    }
    else
    {
        /* no more bindings left and we never connected, so error time */
        /* NOTE: this should effectively never happen */
        composite_error(c, NT_STATUS_INVALID_ADDRESS);
    }
}

/*
 * Allocate a new DCE/RPC pipe by finding the best binding that matches our
 * previous connection attempts.
 */
static void bind_new_pipe(struct composite_context *c)
{
    struct dcom_get_pipe_state *s = NULL;
    struct dcerpc_binding *binding = NULL;
    const char *host = NULL;
    int similar_index = -1;
    NTSTATUS status;

    s = talloc_get_type(c->private_data, struct dcom_get_pipe_state);

    /*
     * First, try and find a similar STRINGBINDING by comparing the specified
     * hostname with that the server has provided us. We parse the hostname as
     * a binding string first to make sure we have a consistent hostname format
     * without any binding prefix.
     */
    host = s->ox->host;
    status = dcerpc_parse_binding(c, host, &binding);
    if (NT_STATUS_IS_OK(status))
    {
        host = talloc_strdup(c, binding->host);
        talloc_free(binding);
    }
    similar_index = find_similar_binding(s->ox->bindings->stringbindings, host);
    DEBUG(1, (__location__": dcom_get_pipe: host=%s, similar=%s\n", host,
            s->ox->bindings->stringbindings[similar_index] ? s->ox->bindings->stringbindings[similar_index]->NetworkAddr : "None"));

    /*
     * Second, set the initial values for the binding offset and index and begin
     * the search for a binding that answers. The current_binding_index
     * represents an index from 0 until we hit the maximum string binding, but
     * the current_binding_offset starts at the similar_index first and then
     * wraps as needed. This approach gives us one pass through the
     * stringbindings but starts where we think is best.
     */
    s->similar_binding_index = similar_index;
    s->current_binding_offset = similar_index - 1;
    s->current_binding_index = 0;

    try_next_binding(c, s);
}

/*
 * Asynchronously request a DCERPC pipe for the specified interface pointer.
 */
struct composite_context *dcom_get_pipe_send(struct IUnknown *d,
        TALLOC_CTX *parent_ctx)
{
    struct composite_context *c = NULL;
    struct dcom_get_pipe_state *s = NULL;

    /* create a new composite to use for this call sequence */
    c = composite_create(0, d->ctx->event_ctx);
    if (c == NULL) return NULL;

    s = talloc_zero(c, struct dcom_get_pipe_state);
    if (composite_nomem(s, c)) return c;
    c->private_data = s;
    s->iface = d;

    /* get the local object exporter for this IUnknown */
    s->ox = object_exporter_by_oxid(s->iface->ctx,
            s->iface->obj.u_objref.u_standard.std.oxid);
    if (s->ox == NULL)
    {
        DEBUG(0, ("dcom_get_pipe: OXID not found\n"));
        composite_error(c, NT_STATUS_NOT_SUPPORTED);
        return c;
    }

    /* release the existing pipe if it is invalid */
    if (s->ox->pipe != NULL && s->ox->pipe->last_fault_code)
    {
        DEBUG(1, ("dcom_get_pipe: pipe's last_fault_code was %08x, freeing\n",
                s->ox->pipe->last_fault_code));
        talloc_free(s->ox->pipe);
        s->ox->pipe = NULL;
    }

    /* if the object exporter has a valid pipe then reuse it, otherwise... */
    if (s->ox->pipe != NULL)
    {
        reuse_existing_pipe(c);
    }
    else
    {
        bind_new_pipe(c);
    }

    return c;
}

/*
 * Receive the results of an asynchronous request to get a DCERPC pipe.
 */
NTSTATUS dcom_get_pipe_recv(struct composite_context *c,
        TALLOC_CTX *parent_ctx, struct dcerpc_pipe **pp)
{
    struct dcom_get_pipe_state *s = NULL;
    NTSTATUS status;

    *pp = NULL;

    status = composite_wait(c);
    if (NT_STATUS_IS_OK(status))
    {
        s = talloc_get_type(c->private_data, struct dcom_get_pipe_state);

        /*
         * let the caller own it, but also let the object exporter own a
         * reference as well
         */
        talloc_steal(parent_ctx, s->p);
        *pp = s->p;

        talloc_reference(s->ox, s->p);
        s->ox->pipe = s->p;
    }

    talloc_free(c);
    return status;
}

/* FIXME:avg put IUnknown_Release_out into header */
struct IUnknown_Release_out
{
    uint32_t result;
};

void dcom_release_continue(struct composite_context *cr)
{
    struct composite_context *c;
    struct IUnknown *d;
    struct IUnknown_Release_out *out;
    WERROR r;

    c = talloc_get_type(cr->async.private_data, struct composite_context);
    d = c->private_data;
    r = IRemUnknown_RemRelease_recv(cr);
    talloc_free(d);
    out = talloc_zero(c, struct IUnknown_Release_out);
    out->result = W_ERROR_V(r);
    c->private_data = out;
    composite_done(c);
}

struct composite_context *dcom_release_send(struct IUnknown *d,
        TALLOC_CTX *mem_ctx)
{
    struct composite_context *c, *cr;
    struct REMINTERFACEREF iref;
    struct dcom_object_exporter *ox;

    c = composite_create(d->ctx, d->ctx->event_ctx);
    if (c == NULL)
        return NULL;
    c->private_data = d;

    ox = object_exporter_by_ip(d->ctx, d);
    iref.ipid = IUnknown_ipid(d);
    iref.cPublicRefs = 5;
    iref.cPrivateRefs = 0;
    cr = IRemUnknown_RemRelease_send(ox->rem_unknown, mem_ctx, 1, &iref);

    composite_continue(c, cr, dcom_release_continue, c);
    return c;
}

uint32_t dcom_release_recv(struct composite_context *c)
{
    NTSTATUS status;
    WERROR r;

    status = composite_wait(c);
    if (!NT_STATUS_IS_OK(status))
        r = ntstatus_to_werror(status);
    else
        W_ERROR_V(r) = ((struct IUnknown_Release_out *) c->private_data)->result;
    talloc_free(c);
    return W_ERROR_IS_OK(r) ? 0 : W_ERROR_V(r);
}

uint32_t dcom_release(void *interface, TALLOC_CTX *mem_ctx)
{
    struct composite_context *c;

    c = dcom_release_send(interface, mem_ctx);
    return dcom_release_recv(c);
}

void dcom_proxy_async_call_recv_pipe_send_rpc(struct composite_context *c_pipe)
{
    struct composite_context *c;
    struct dcom_proxy_async_call_state *s;
    struct dcerpc_pipe *p;
    struct rpc_request *req;
    NTSTATUS status;

    c = c_pipe->async.private_data;
    s = talloc_get_type(c->private_data, struct dcom_proxy_async_call_state);

    status = dcom_get_pipe_recv(c_pipe, c, &p);
    if (!NT_STATUS_IS_OK(status))
    {
        composite_error(c, NT_STATUS_RPC_NT_CALL_FAILED);
        return;
    }

    req = dcerpc_ndr_request_send(p, &s->d->obj.u_objref.u_standard.std.ipid,
            s->table, s->opnum, s, s->r);
    composite_continue_rpc(c, req, s->continuation, c);
}

