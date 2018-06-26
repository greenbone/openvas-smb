/*
###############################################################################
# WMI Client lib
# This is a derivative work of the WMI Sample command line client
#
# Authors:
# Chandrashekhar B <bchandra@secpod.com>
#
# Copyright:
# Copyright (c) 2009 SecPod, http://www.secpod.com
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

/*
   WMI Sample client
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

/**
 * @file wmicso.c
 *
 * @brief WMI functions
 *
 * Provides WMI functionalities.
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
#include "librpc/rpc/dcerpc_table.h"
#include "param/param.h"

#include "lib/com/dcom/dcom.h"
#include "lib/com/proto.h"
#include "lib/com/dcom/proto.h"

struct WBEMCLASS;
struct WBEMOBJECT;

#include "wmi/proto.h"
#include "wmi/wmi.h"

#include "openvas_wmi_interface.h"

#define WMI_LIB_VERSION "0.0.1"

#define WERR_CHECK(msg) if (!W_ERROR_IS_OK(result)) { \
                            DEBUG(2, ("ERROR: %s\n", msg)); \
                            goto error; \
                        } else { \
                            DEBUG(1, ("OK   : %s\n", msg)); \
                        }


struct program_args {
  char *hostname;       // Hostname
  char *ns;             // WMI namspace, ex: root\cimv2
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
    if (argc_new != 3 || argv_new[1][0] != '/'
        || argv_new[1][1] != '/') {
      poptFreeContext(pc);
          return 1;
    }
 
    pmyargs->hostname = argv_new[1] + 2;
    pmyargs->ns = argv_new[2];
    poptFreeContext(pc);
    return 0;
}



/*
 * @brief Decode the values based on data type
 */
char *string_CIMVAR(TALLOC_CTX *mem_ctx, union CIMVAR *v, enum CIMTYPE_ENUMERATION cimtype)
{
  int i;
  char *s = NULL;

  switch (cimtype) {
    case CIM_SINT8: return talloc_asprintf(mem_ctx, "%d", v->v_sint8);
    case CIM_UINT8: return talloc_asprintf(mem_ctx, "%u", v->v_uint8);
    case CIM_SINT16: return talloc_asprintf(mem_ctx, "%d", v->v_sint16);
    case CIM_UINT16: return talloc_asprintf(mem_ctx, "%u", v->v_uint16);
    case CIM_SINT32: return talloc_asprintf(mem_ctx, "%d", v->v_sint32);
    case CIM_UINT32: return talloc_asprintf(mem_ctx, "%u", v->v_uint32);
    case CIM_SINT64: return talloc_asprintf(mem_ctx, "%lld", v->v_sint64);
    case CIM_UINT64: return talloc_asprintf(mem_ctx, "%llu", v->v_sint64);
    case CIM_REAL32: return talloc_asprintf(mem_ctx, "Unsupported");
    case CIM_REAL64: return talloc_asprintf(mem_ctx, "Unsupported");
    case CIM_BOOLEAN: return talloc_asprintf(mem_ctx, "%s", v->v_boolean?"True":"False");
    case CIM_STRING:
    case CIM_DATETIME:
    case CIM_REFERENCE: return talloc_asprintf(mem_ctx, "%s", v->v_string);
    case CIM_CHAR16: return talloc_asprintf(mem_ctx, "Unsupported");
    case CIM_OBJECT: return talloc_asprintf(mem_ctx, "Unsupported");
    case CIM_ARR_STRING:
      if(v->a_string){
        for (i = 0; i < v->a_uint8->count; ++i){
          s = (char *) talloc_asprintf_append(s, "%s%s", i?"|":"", v->a_string->item[i]);
        }
      }
      return s;
    default: return talloc_asprintf(mem_ctx, "Unsupported");
  }
}


/**
 * @brief Returns WMI Client library version
 * @return, WMI client library version
 */
char *
wmi_versioninfo()
{
  return WMI_LIB_VERSION;
}


/**
 * @brief Estiablish connection to a WMI service.
 *
 * @param[in] argc      Number wmic of arguments.
 *
 * @param[in] argv      Array of wmic arguments.
 *
 * @return, WMI_HANDLE on success, NULL on failure.
 */
WMI_HANDLE
wmi_connect (int argc, char **argv)
{

  WERROR result;
  NTSTATUS status;
  struct IWbemServices *pWS = NULL;
  struct com_context *ctx;
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

  com_init_ctx(&ctx, NULL);
  dcom_client_init(ctx, cmdline_credentials);

  result = WBEM_ConnectServer(ctx, args.hostname, args.ns, 0, 0, 0, 0, 0, 0, &pWS);
  WERR_CHECK("Login to remote object.\n");
  return pWS;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  return NULL;
}


/**
 * @brief Query WMI service using a WQL query
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] query - The WQL query string
 *
 * @param[out] result - Result of query as string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_query(WMI_HANDLE handle, const char *query, char **res)
{
  uint32_t cnt = 5, ret = 0;
  char *class_name = NULL;
  WERROR result;
  NTSTATUS status;
  struct IWbemServices *pWS = NULL;
  struct IEnumWbemClassObject *pEnum = NULL;

  pWS = (struct IWbemServices *)handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_ExecQuery(pWS, pWS->ctx, "WQL", query, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_ENSURE_LOCATABLE, NULL, &pEnum);
  WERR_CHECK("WMI query execute.");

  IEnumWbemClassObject_Reset(pEnum, pWS->ctx);
  WERR_CHECK("Reset result of WMI query.");

  do {
    uint32_t i, j;
    struct WbemClassObject *co[cnt];

    result = IEnumWbemClassObject_SmartNext(pEnum, pWS->ctx, 0xFFFFFFFF, cnt, co, &ret);
    /* WERR_BADFUNC is OK, it means only that there is less returned objects than requested */
    if (!W_ERROR_EQUAL(result, WERR_BADFUNC)) {
      WERR_CHECK("Retrieve result data.");
    }
    else {
      DEBUG(2, ("OK   : Retrieved less objects than requested (it is normal).\n"));
    }
    if(!ret)
      break;

    for (i = 0; i < ret; ++i){
      if (!class_name || strcmp(co[i]->obj_class->__CLASS, class_name)){
        if (class_name) talloc_free(class_name);
          class_name = talloc_strdup(pWS->ctx, co[i]->obj_class->__CLASS);
          *res = (char *) talloc_strdup(NULL , "");
          for (j = 0; j < co[i]->obj_class->__PROPERTY_COUNT; ++j)
            *res = (char *) talloc_asprintf_append(*res, "%s%s", j?"|":"", co[i]->obj_class->properties[j].name);
          *res = (char *) talloc_asprintf_append(*res, "\n");
      }
      for (j = 0; j < co[i]->obj_class->__PROPERTY_COUNT; ++j){
        char *s;
        s = string_CIMVAR(pWS->ctx, &co[i]->instance->data[j], co[i]->obj_class->properties[j].desc->cimtype & CIM_TYPEMASK);
        *res = (char *) talloc_asprintf_append(*res, "%s%s", j?"|":"", s);
      }
      *res = (char *) talloc_asprintf_append(*res, "\n");
    }
  } while (ret == cnt);

  return 0;

  error:
    status = werror_to_ntstatus(result);
    DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
    return -1;
}


/**
 * @brief Close the connection handle for a WMI service.
 *
 * @param[in] handle - WMI service connection handle
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_close(WMI_HANDLE handle)
{
  struct IWbemServices *pWS = NULL;
  pWS = (struct IWbemServices *) handle;
  if(pWS)
  {
    talloc_free(pWS->ctx);
    pWS->ctx = NULL;
  }
  return 0;
}

