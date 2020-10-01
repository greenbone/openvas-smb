/*
###############################################################################
# WMI Client lib
# This is a derivative work of the WMI Sample command line client
#
# Authors:
# Chandrashekhar B <bchandra@secpod.com>
#
# Copyright:
# Copyright (c) 2009 Intevation GmbH, http://www.intevation.net
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2
# (or any later version), as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
###############################################################################

*/

/**
 * @file wmirsop.c
 *
 * @brief WMI functions
 *
 * Provides WMI RSOP functionalities.
 */
#include "includes.h"
#include "lib/cmdline/popt_common.h"
#include "librpc/rpc/dcerpc.h"
#include "librpc/gen_ndr/ndr_oxidresolver.h"
#include "librpc/gen_ndr/ndr_oxidresolver_c.h"
#include "librpc/gen_ndr/ndr_dcom.h"
#include "librpc/gen_ndr/ndr_dcom_c.h"
#include "librpc/gen_ndr/ndr_remact_c.h"
#include "librpc/gen_ndr/ndr_epmapper_c.h"
#include "librpc/gen_ndr/com_dcom.h"
#include "param/param.h"

#include "lib/com/dcom/dcom.h"
#include "lib/com/proto.h"
#include "lib/com/dcom/proto.h"

struct WBEMCLASS;
struct WBEMOBJECT;

#include "wmi/proto.h"
#include "wmi/wmi.h"
#include "openvas_wmi_interface.h"


#define WERR_CHECK(msg) if (!W_ERROR_IS_OK(result)) { \
                            DEBUG(2, ("ERROR: %s\n", msg)); \
                            goto error; \
                        } else { \
                            DEBUG(1, ("OK   : %s\n", msg)); \
                        }

struct program_args {
  char *hostname;       // Hostname
};


static int parse_args(int argc, char *argv[], struct program_args *pmyargs)
{
    poptContext pc;
    int opt, i;
    int argc_new;
    char **argv_new;
 
    struct poptOption long_options[] = {
        POPT_AUTOHELP
        POPT_COMMON_SAMBA
        POPT_COMMON_CONNECTION
        POPT_COMMON_CREDENTIALS
        POPT_COMMON_VERSION
        POPT_TABLEEND
    };
 
    pc = poptGetContext("wmic", argc, (const char **) argv,
                long_options, POPT_CONTEXT_KEEP_FIRST);
 
 
    while ((opt = poptGetNextOpt(pc)) != -1) {
          poptFreeContext(pc);
          return 1;
    }
 
    argv_new = discard_const_p(char *, poptGetArgs(pc));
 
    argc_new = argc;
    for (i = 0; i < argc; i++) {
          if (argv_new[i] == NULL) {
            argc_new = i;
            break;
          }
    }
    if (argc_new != 2 || argv_new[1][0] != '/'
        || argv_new[1][1] != '/') {
      poptFreeContext(pc);
          return 1;
    }
 
    pmyargs->hostname = argv_new[1] + 2;
    poptFreeContext(pc);
    return 0;
}


/**
 * @brief Estiablish connection to a WMI RSOP service.
 *
 * @param[in] argc      Number wmic of arguments.
 *
 * @param[in] argv      Array of wmic arguments.
 *
 * @return, WMI_HANDLE on success, NULL on failure.
 */
WMI_HANDLE
wmi_connect_rsop (int argc, char **argv)
{
  /*Works only for domain based systems and not for WORKGROUP */

  struct com_context *ctx = NULL;
  NTSTATUS status;
  struct IWbemServices *pWS = NULL;
  uint32_t *ret_code;

  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  WERROR result;
  union CIMVAR v;
  char *namespace = NULL;
  int ret;
  struct program_args args = {};

  ret = parse_args(argc, argv, &args);
 
  if(ret == 1)
  {
    DEBUG(1, ("ERROR: %s\n", "Invalid input arguments"));
    return NULL;
  }


  dcerpc_init();
  dcerpc_table_init();

  dcom_proxy_IUnknown_init();
  dcom_proxy_IWbemLevel1Login_init();
  dcom_proxy_IWbemServices_init();
  dcom_proxy_IEnumWbemClassObject_init();
  dcom_proxy_IRemUnknown_init();
  dcom_proxy_IWbemFetchSmartEnum_init();
  dcom_proxy_IWbemWCOSmartEnum_init();
  dcom_proxy_IWbemClassObject_init();

  com_init_ctx(&ctx, NULL);
  dcom_client_init(ctx, cmdline_credentials);

  /* Connect to RSOP namespace */
  result = WBEM_ConnectServer(ctx, args.hostname, "root\\rsop", 0, 0, 0, 0, 0, 0, &pWS);
  WERR_CHECK("WBEM_ConnectServer.");

  result = IWbemServices_GetObject(pWS, ctx, "RsopLoggingModeProvider",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, ctx, "RsopCreateSession", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* Flag that ensures creation of temporary RSOP db */
  v.v_uint32 = 4;
  result = IWbemClassObject_Put(in, ctx, "flags", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, ctx, "RsopLoggingModeProvider",
                                    "RsopCreateSession", 0, NULL, in, &out,
                                     NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  if(ret_code){
    result = WbemClassObject_Get(out->object_data, ctx, "nameSpace", 0, &v,
                                 0, 0);
    WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  }

  /* Computer namespace only, user namespace doesn't seem to work */
  namespace = talloc_asprintf_append(v.v_string, "%s", "\\computer");
  result = WBEM_ConnectServer(ctx, args.hostname, namespace, 0, 0, 0, 0, 0, 0, &pWS);

  return pWS;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  return NULL;
}


/**
 * @brief WMI RSOP query.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] query - WQL RSOP query
 *
 * @param[out] res - Result of query as string, or the error.
 *
 * @return, 0 on success, -1 on failure. If fail and res is filled, res must
 *          be free'd.
 */
int wmi_query_rsop(WMI_HANDLE handle, const char *query, char **res)
{
  WERROR result;
  NTSTATUS status;
  struct IWbemServices *pWS = NULL;
  struct IEnumWbemClassObject *pEnum = NULL;
  uint32_t cnt = 5, ret;
  char *class_name = NULL;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_ExecQuery(pWS, pWS->ctx, "WQL", query,
                                   WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_ENSURE_LOCATABLE,
                                   NULL, &pEnum);
  WERR_CHECK("WMI query execute.");

  IEnumWbemClassObject_Reset(pEnum, pWS->ctx);
  WERR_CHECK("Reset result of WMI query.");

  do {
    uint32_t i, j;
    struct WbemClassObject *co[cnt];

    result = IEnumWbemClassObject_SmartNext(pEnum, pWS->ctx, 0xFFFFFFFF,
                                            cnt, co, &ret);
    /* WERR_BADFUNC is OK, it means only that there is less returned
     * objects than requested
     */
    if (!W_ERROR_EQUAL(result, WERR_BADFUNC)
        && W_ERROR_V(result) != WBEM_E_INVALID_CLASS) {
      WERR_CHECK("Retrieve result data.");
    }
    else{
      DEBUG(2, ("OK   : Retrieved less objects than requested (it is normal).\n"));
    }
    if(!ret) break;

    for (i = 0; i < ret; ++i){
      if (!class_name || strcmp(co[i]->obj_class->__CLASS, class_name)){
        if (class_name) talloc_free(class_name);
        class_name = talloc_strdup(pWS->ctx, co[i]->obj_class->__CLASS);

        *res = (char *) talloc_strdup(NULL , "");
        for (j = 0; j < co[i]->obj_class->__PROPERTY_COUNT; ++j)
          *res = (char *) talloc_asprintf_append(*res, "%s%s", j?"|":"",
                                                 co[i]->obj_class->properties[j].name);
        *res = (char *) talloc_asprintf_append(*res, "\n");
      }
      for (j = 0; j < co[i]->obj_class->__PROPERTY_COUNT; ++j){
        char *s;
        s = string_CIMVAR(pWS->ctx, &co[i]->instance->data[j],
                          co[i]->obj_class->properties[j].desc->cimtype & CIM_TYPEMASK);
        *res = (char *) talloc_asprintf_append(*res, "%s%s", j?"|":"", s);
      }
      *res = (char *) talloc_asprintf_append(*res, "\n");
    }
  } while (ret == cnt);
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  *res = strdup (("NTSTATUS: %s", nt_errstr(status)));

  return -1;
}
