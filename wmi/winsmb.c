/*
###############################################################################
# SMB Client lib
# This is a derivative work of smbclient (clien/client.c)

# Authors:
# Chandrashekhar B <bchandra@secpod.com>
#
# Copyright:
# Copyright (c) 2009 Intevation GmbH, http://www.intevation.net
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
 * @file winsmb.c
 * @brief Implements application specific SMB functions invoking
 * low level Samba API's.
 *
 */

#include "includes.h"
#include "version.h"
#include "libcli/libcli.h"
#include "lib/cmdline/popt_common.h"
#include "libcli/raw/libcliraw.h"
#include "system/dir.h"
#include "system/filesys.h"
#include "lib/util/dlinklist.h"
#include "auth/credentials/credentials.h"
#include "auth/gensec/gensec.h"
#include "system/time.h"
#include "libcli/resolve/resolve.h"
#include "libcli/security/security.h"
#include "lib/smbreadline/smbreadline.h"
#include "librpc/gen_ndr/ndr_nbt.h"
#include "param/param.h"

#include "librpc/gen_ndr/ndr_srvsvc_c.h"
#include "librpc/gen_ndr/ndr_lsa.h"
#include "librpc/gen_ndr/ndr_security.h"

#include "openvas_smb_interface.h"

#define SMB_LIB_VERSION "0.0.1"

TALLOC_CTX *mem_ctx;


/**
 * @brief Returns SMB Client library version
 * @return, SMB client library version
 */

char *
smb_versioninfo()
{
  return SMB_LIB_VERSION;
}


/**
 * @brief Estiablish connection to a SMB service.
 *
 * @param[in] server - The host system to connect to
 *
 * @param[in] share - The file system share.
 *
 * @param[in] username - The username for getting access to SMB service
 *
 * @param[in] password - The password that corresponds to username
 *
 * @param[out] handle - A connection handle in case of success.
 *
 * @return, 0 on success, -1 on failure
 */
int smb_connect(const char *server, const char *share, const char *username, const char *password, SMB_HANDLE *handle)
{
  int opt;
  poptContext pc;
  struct smbcli_state * SmbClientState;
  struct smbcli_options smb_options;
  NTSTATUS status;
  struct cli_credentials *creds;
  char *DelimAt = NULL;
  char *domain = NULL;

  int argc = 1;
  char **argv = NULL;

  struct poptOption long_options[] = {
    POPT_AUTOHELP
    POPT_COMMON_SAMBA
    POPT_COMMON_CONNECTION
    POPT_COMMON_CREDENTIALS
    POPT_COMMON_VERSION
    { NULL }
  };

  pc = poptGetContext("smbcli", argc, (const char **) argv,
                       long_options, 0);

  while ((opt = poptGetNextOpt(pc)) != -1) {
    break;
  }
  poptFreeContext(pc);

  mem_ctx = talloc_init("winsmb.c/smb_connect");
  if (!mem_ctx) {
    fprintf(stderr, "\nclient.c: Not enough memory\n");
    return -1;
  }

  SmbClientState = talloc(mem_ctx, struct smbcli_state);
  gensec_init();

  DelimAt = strpbrk(username, "\\/");
  if (DelimAt)
  {
    domain = talloc_strndup(mem_ctx, username, DelimAt - username);
    username = talloc_strdup(mem_ctx, DelimAt+1);
  }

  creds = cli_credentials_init(mem_ctx);
  cli_credentials_set_conf(creds);
  cli_credentials_set_username(creds, username, CRED_SPECIFIED);
  cli_credentials_set_password(creds, password, CRED_SPECIFIED);
  if (domain)
    cli_credentials_set_domain(creds, domain, CRED_SPECIFIED);
  gensec_set_credentials(mem_ctx, creds);

  status = smbcli_full_connection(mem_ctx, &SmbClientState, server,
                                  share,
                                  NULL, creds,
                                  NULL);


  if (!NT_STATUS_IS_OK(status)) {
    return -1;
  }

  *handle = (SMB_HANDLE) SmbClientState;
  fprintf(stderr, "created: %p\n", SmbClientState);
  return 0;
}


/**
 * @brief Close the connection handle for SMB service.
 *
 * @param[in] handle - SMB connection handle
 *
 * @return, 0 on success, -1 on failure
 */
int smb_close(SMB_HANDLE handle) {
  if(handle){
    fprintf(stderr, "closing: %p\n", handle);
    talloc_free((struct smbcli_state *) handle);
    talloc_free(mem_ctx);
    return 0;
  }
  else
    return -1;
}

/**
 * @brief Obtain Windows file rights in SDDL format
 *
 * @param[in] handle - SMB connection handle
 *
 * @param[in] filename - File system path
 *
 * @return, Security Descriptor in SDDL format on success, NULL on failure.
 */
char *smb_file_SDDL(SMB_HANDLE handle, const char *filename)
{
  union smb_fileinfo query;
  NTSTATUS status;
  int fnum;
  struct smbcli_state *SmbClientState;

  if((!handle) || (!filename))
    return NULL;

  SmbClientState = (struct smbcli_state *) handle;

  fnum = smbcli_nt_create_full(SmbClientState->tree, filename, 0,
                               SEC_STD_READ_CONTROL,
                               0,
                               NTCREATEX_SHARE_ACCESS_DELETE|
                               NTCREATEX_SHARE_ACCESS_READ|
                               NTCREATEX_SHARE_ACCESS_WRITE,
                               NTCREATEX_DISP_OPEN,
                               0, 0);

  if(!fnum)
    return NULL;

  query.query_secdesc.level = RAW_FILEINFO_SEC_DESC;
  query.query_secdesc.in.file.fnum = fnum;
  query.query_secdesc.in.secinfo_flags = 0x7;

  status = smb_raw_fileinfo(SmbClientState->tree, mem_ctx, &query);

  if (!NT_STATUS_IS_OK(status)) {
    fprintf(stderr, "%s\n", nt_errstr(status));
    return NULL;
  }

  return sddl_encode(SmbClientState, query.query_secdesc.out.sd, NULL);
}


/**
 * @brief Obtain the SID of the Owner for a given file/path
 *
 * @param[in] handle - SMB connection handle
 *
 * @param[in] filename - File system path
 *
 * @return, Owner SID string on success, NULL on failure.
 */
char *smb_file_OwnerSID(SMB_HANDLE handle, const char *filename)
{
  union smb_fileinfo query;
  NTSTATUS status;
  int fnum;
  struct smbcli_state *SmbClientState;

  if((!handle) || (!filename))
    return NULL;

  SmbClientState = (struct smbcli_state *) handle;

  fnum = smbcli_nt_create_full(SmbClientState->tree, filename, 0,
                               SEC_STD_READ_CONTROL,
                               0,
                               NTCREATEX_SHARE_ACCESS_DELETE|
                               NTCREATEX_SHARE_ACCESS_READ|
                               NTCREATEX_SHARE_ACCESS_WRITE,
                               NTCREATEX_DISP_OPEN,
                               0, 0);

  if(!fnum)
    return NULL;

  query.query_secdesc.level = RAW_FILEINFO_SEC_DESC;
  query.query_secdesc.in.file.fnum = fnum;
  query.query_secdesc.in.secinfo_flags = 0x7;

  status = smb_raw_fileinfo(SmbClientState->tree, mem_ctx, &query);

  if (!NT_STATUS_IS_OK(status)) {
    fprintf(stderr, "%s\n", nt_errstr(status));
    return NULL;
  }

  return dom_sid_string(NULL, query.query_secdesc.out.sd->owner_sid);
}


/**
 * @brief Obtain the SID of the Group for a given file/path
 *
 * @param[in] handle - SMB connection handle
 *
 * @param[in] filename - File system path
 *
 * @return, Group SID string on success, NULL on failure.
 */
char *smb_file_GroupSID(SMB_HANDLE handle, const char *filename)
{
  union smb_fileinfo query;
  NTSTATUS status;
  int fnum;
  struct smbcli_state *SmbClientState;

  if((!handle) || (!filename))
    return NULL;

  SmbClientState = (struct smbcli_state *) handle;

  fnum = smbcli_nt_create_full(SmbClientState->tree, filename, 0,
                               SEC_STD_READ_CONTROL,
                               0,
                               NTCREATEX_SHARE_ACCESS_DELETE|
                               NTCREATEX_SHARE_ACCESS_READ|
                               NTCREATEX_SHARE_ACCESS_WRITE,
                               NTCREATEX_DISP_OPEN,
                               0, 0);

  if(!fnum)
    return NULL;

  query.query_secdesc.level = RAW_FILEINFO_SEC_DESC;
  query.query_secdesc.in.file.fnum = fnum;
  query.query_secdesc.in.secinfo_flags = 0x7;

  status = smb_raw_fileinfo(SmbClientState->tree, mem_ctx, &query);

  if (!NT_STATUS_IS_OK(status)) {
    fprintf(stderr, "%s\n", nt_errstr(status));
    return NULL;
  }

  return dom_sid_string(NULL, query.query_secdesc.out.sd->group_sid);
}


/**
 * @brief Obtain the Trustee SID and their rights for a given file/path
 *
 * @param[in] handle - SMB connection handle
 *
 * @param[in] filename - File system path
 *
 * @return, Trustee SID:Access_Mask string on success, NULL on failure.
 */
char *smb_file_TrusteeRights(SMB_HANDLE handle, const char *filename)
{

  union smb_fileinfo query;
  NTSTATUS status;
  int fnum;
  struct smbcli_state *SmbClientState;
  int i = 0;
  char *TrusteeSids;

  if((!handle) || (!filename))
    return NULL;

  SmbClientState = (struct smbcli_state *) handle;
  TrusteeSids  = talloc_strdup(SmbClientState, "");

  fnum = smbcli_nt_create_full(SmbClientState->tree, filename, 0,
                               SEC_STD_READ_CONTROL,
                               0,
                               NTCREATEX_SHARE_ACCESS_DELETE|
                               NTCREATEX_SHARE_ACCESS_READ|
                               NTCREATEX_SHARE_ACCESS_WRITE,
                               NTCREATEX_DISP_OPEN,
                               0, 0);

  if(!fnum)
    return NULL;

  query.query_secdesc.level = RAW_FILEINFO_SEC_DESC;
  query.query_secdesc.in.file.fnum = fnum;
  query.query_secdesc.in.secinfo_flags = 0x7;

  status = smb_raw_fileinfo(SmbClientState->tree, mem_ctx, &query);

  if (!NT_STATUS_IS_OK(status)) {
    fprintf(stderr, "%s - %s\n", filename, nt_errstr(status));
    return NULL;
  }

  for (i=0; i< query.query_secdesc.out.sd->dacl->num_aces; i++ ) {
    TrusteeSids = talloc_asprintf_append(TrusteeSids, "%s:%08x,",
                   dom_sid_string(NULL, &query.query_secdesc.out.sd->dacl->aces[i].trustee),
                   query.query_secdesc.out.sd->dacl->aces[i].access_mask);
  }

  return TrusteeSids;
}
