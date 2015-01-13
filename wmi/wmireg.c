/*
###############################################################################
# WMI Client lib
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
 * @file wmireg.c
 *
 * @brief WMI Registry functions
 *
 * Provides WMI registry functionalities.
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
#include <inttypes.h>


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
 * @brief Estiablish connection to a WMI Registry service.
 *
 * @param[in] username - The username for getting access to WMI service
 *
 * @param[in] password - The password that corresponds to username
 *
 * @param[in] host - The host system to connect to
 *
 * @param[in] namespace - The WMI namespace of the service.
 *
 * @param[out] handle - A connection handle in case of success.
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_connect_reg(int argc, char **argv, WMI_HANDLE *handle)
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
    return -1;
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

  result = WBEM_ConnectServer(ctx, args.hostname, "root\\default", 0, 0, 0, 0, 0, 0, &pWS);
  WERR_CHECK("Login to remote object.\n");
  *handle = (WMI_HANDLE) pWS;

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Get Registry string value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key name
 *
 * @param[in] key_name - Registry value name.
 *
 * @param[out] res - Result string.
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_sz(WMI_HANDLE handle, const unsigned int hive, const char *key,
                   const char *key_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &wco, NULL);
  WERR_CHECK("GetObject.");


  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetStringValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "GetStringValue",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "sValue", 0, &v, 0, 0);
  if(v.v_string){
    *res = talloc_asprintf(pWS->ctx, "%s", v.v_string);
  }

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Enumerate Registry keys.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_enum_key(WMI_HANDLE handle, const unsigned int hive,
                     const char *key, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "EnumKey", 0, &inc,
                                      &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "EnumKey", 0, NULL,
                                    in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "sNames", 0, &v,
                               0, 0);

  if(v.a_string){
    for (i = 0; i < v.a_string->count; ++i)
      *res = (char *) talloc_asprintf_append(*res, "%s%s", i?"|":"",
                                             v.a_string->item[i]);
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
             get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Enumerate Registry values.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key name
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_enum_value(WMI_HANDLE handle, const unsigned int hive,
                       const char *key, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "EnumValues", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_uint32 = 0;

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "EnumValues", 0,
                                    NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "sNames", 0, &v, 0, 0);
  if(v.a_string){
    for (i = 0; i < v.a_string->count; ++i)
      *res = (char *) talloc_asprintf_append(*res, "%s%s", i?"|":"",
                                             v.a_string->item[i]);
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Get Registry binary value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key containing the value to be queried
 *
 * @param[in] val_name - Registry value to be queried
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_bin_val(WMI_HANDLE handle, const unsigned int hive, const char *key,
                        const char *val_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetBinaryValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "GetBinaryValue",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");
  result = WbemClassObject_Get(out->object_data, pWS->ctx, "uValue", 0, &v, 0, 0);
  if(v.a_uint8){
    for (i = 0; i < v.a_uint8->count; ++i){
      *res = (char *)talloc_asprintf_append(*res, "%0X", v.a_uint8->item[i]);
    }
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return 1;
}


/**
 * @brief Get Registry DWORD value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key containing the value to be queried
 *
 * @param[in] val_name - Registry value to be queried
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_dword_val(WMI_HANDLE handle, const unsigned int hive, const char *key,
                          const char *val_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;
  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetDWORDValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "GetDWORDValue",
                                      0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "uValue", 0,
                               &v, 0, 0);
  if(v.v_uint32){
    *res = talloc_asprintf(pWS->ctx, "%0X", v.v_uint32);
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Get Registry Expanded string value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key containing the value to be queried
 *
 * @param[in] val_name - Registry value to be queried
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_ex_string_val(WMI_HANDLE handle, const unsigned int hive, const char *key,
                              const char *val_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetExpandedStringValue",
                                      0, &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv",
                                    "GetExpandedStringValue", 0,
                                    NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "sValue", 0, &v,
                               0, 0);
  if(v.v_string){
    *res = talloc_asprintf(pWS->ctx, "%s", v.v_string);
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Get Registry multi-valued strings.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key containing the value to be queried
 *
 * @param[in] val_name - Registry value to be queried
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_mul_string_val(WMI_HANDLE handle, const unsigned int hive, const char *key,
                               const char *val_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetMultiStringValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_uint32 = 0;

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv",
                                    "GetMultiStringValue", 0,
                                    NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "sValue", 0,
                               &v, 0, 0);
  if(v.a_string){
    for (i = 0; i < v.a_string->count; ++i)
      *res = (char *) talloc_asprintf_append(*res, "%s%s", i?"|":"",
                                             v.a_string->item[i]);
  }
  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Get Registry QWORD value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] hive - Registry hive
 *
 * @param[in] key - Registry key containing the value to be queried
 *
 * @param[in] val_name - Registry value to be queried
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_get_qword_val(WMI_HANDLE handle, const unsigned int hive, const char *key,
                          const char *val_name, char **res)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "GetQWORDValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  if(hive)
    v.v_uint32 = hive;
  else
    v.v_uint32 = 0x80000002; // Try default, HKEY_LOCAL_MACHINE

  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_uint32 = 0;

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "GetQWORDValue",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  result = WbemClassObject_Get(out->object_data, pWS->ctx, "uValue", 0, &v, 0, 0);
  if(v.v_uint64){
    *res = talloc_asprintf(pWS->ctx, "%"PRIx64, v.v_uint64);
  }

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status), get_friendly_nt_error_msg(status)));
  return -1;
}

/**

 * @brief Set Registry DWORD value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key containing the value to be set
 *
 * @param[in] val_name - Registry value to set
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_set_dword_val(WMI_HANDLE handle, const char *key, const char *val_name, uint32_t val)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "SetDWORDValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  v.v_uint32 = (uint32_t)val;
  result = IWbemClassObject_Put(in, pWS->ctx, "uValue", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "SetDWORDValue",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Set Registry QWORD value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key containing the value to be set
 *
 * @param[in] val_name - Registry value to set
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_set_qword_val(WMI_HANDLE handle, const char *key, const char *val_name, uint64_t val)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "SetQWORDValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

 /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  v.v_uint64 = (uint64_t)val;
  result = IWbemClassObject_Put(in, pWS->ctx, "uValue", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv",
                                    "SetQWORDValue", 0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Set Registry Expanded string value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key containing the value to be set
 *
 * @param[in] val_name - Registry value to set
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_set_ex_string_val(WMI_HANDLE handle, const char *key, const char *val_name, const char *val)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;
  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "SetExpandedStringValue",
                                      0, &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  v.v_string = val;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValue", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv",
                                    "SetExpandedStringValue", 0, NULL, in,
                                                                          
                                    &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}


/**
 * @brief Set Registry string value.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key containing the value to be set
 *
 * @param[in] val_name - Registry value to set
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_set_string_val(WMI_HANDLE handle, const char *key, const char *val_name, const char *val)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "SetStringValue", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = val_name;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValueName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");
  v.v_string = NULL;

  v.v_string = val;
  result = IWbemClassObject_Put(in, pWS->ctx, "sValue", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "SetStringValue",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;
 
error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}

/**
 * @brief Create Registry Key.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key need to be created
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_create_key(WMI_HANDLE handle, const char *key)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "CreateKey", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "CreateKey",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}

/**
 * @brief Delete Registry Key.
 *
 * @param[in] handle - WMI connection handle
 *
 * @param[in] key - Registry key need to be Deleted
 *
 * @return, 0 on success, -1 on failure
 */
int wmi_reg_delete_key(WMI_HANDLE handle, const char *key)
{
  struct IWbemClassObject *wco = NULL;
  struct IWbemClassObject *inc, *outc, *in;
  struct IWbemClassObject *out = NULL;
  int i = 0;
  WERROR result;
  NTSTATUS status;
  union CIMVAR v;
  struct IWbemServices *pWS;

  pWS = (struct IWbemServices *) handle;
  if(pWS->ctx == 0)
    return -1;

  result = IWbemServices_GetObject(pWS, pWS->ctx, "StdRegProv",
                                   WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL,
                                   &wco, NULL);
  WERR_CHECK("GetObject.");

  result = IWbemClassObject_GetMethod(wco, pWS->ctx, "DeleteKey", 0,
                                      &inc, &outc);
  WERR_CHECK("IWbemClassObject_GetMethod.");

  result = IWbemClassObject_SpawnInstance(inc, pWS->ctx, 0, &in);
  WERR_CHECK("IWbemClassObject_SpawnInstance.");

  /* (int)2147483650; 0x80000002; HKEY_LOCAL_MACHINE */
  v.v_uint32 = 0x80000002;
  result = IWbemClassObject_Put(in, pWS->ctx, "hDefKey", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  v.v_string = key;
  result = IWbemClassObject_Put(in, pWS->ctx, "sSubKeyName", 0, &v, 0);
  WERR_CHECK("IWbemClassObject_Put(CommandLine).");

  result = IWbemServices_ExecMethod(pWS, pWS->ctx, "StdRegProv", "DeleteKey",
                                    0, NULL, in, &out, NULL);
  WERR_CHECK("IWbemServices_ExecMethod.");

  return 0;

error:
  status = werror_to_ntstatus(result);
  DEBUG(3, ("NTSTATUS: %s - %s\n", nt_errstr(status),
            get_friendly_nt_error_msg(status)));
  return -1;
}
