# SPDX-FileCopyrightText: 2015 Greenbone AG
# SPDX-License-Identifier: GPL-2.0-or-later

# openvas-smb
# $Id$
# Description: Cmake control for the smaba library parts.
#
# Authors:
# Andre Heinecke <aheinecke@greenbone.net>
#
# Copyright:
# Copyright (C) 2015 Greenbone AG
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

# There is a hardcoded 30k command line limit without this tag
#
# From: http://www.cmake.org/gitweb?p=cmake.git;a=blob;f=Source/cmMakefileLibraryTargetGenerator.cxx;h=7ac025687d950f496d49c9bc539a1475a3853674;hb=HEAD#l536
#    // Limit the length of individual object lists to less than the
#    // 32K command line length limit on Windows.  We could make this a
#    // platform file variable but this should work everywhere.
#    archiveCommandLimit = 30000;
# yay
set(CMAKE_START_TEMP_FILE "@")
set(CMAKE_C_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")

include_directories(${GSSAPI_INCLUDE_DIRS})
include_directories(${POPT_INCLUDE_DIRS})
include_directories(${GNUTLS_INCLUDE_DIRS})

set (SMB_INCLUDES
   ${CMAKE_SOURCE_DIR}/samba
   ${CMAKE_SOURCE_DIR}/samba/include
   ${CMAKE_SOURCE_DIR}/samba/lib
   ${CMAKE_SOURCE_DIR}/samba/lib/appweb/ejs
   ${CMAKE_SOURCE_DIR}/samba/lib/crypto
   ${CMAKE_SOURCE_DIR}/samba/lib/ldb/include
   ${CMAKE_SOURCE_DIR}/samba/lib/tdb/include
   ${CMAKE_SOURCE_DIR}/samba/lib/replace
   ${CMAKE_SOURCE_DIR}/samba/ntvfs
   ${CMAKE_SOURCE_DIR}/samba/param
   ${CMAKE_SOURCE_DIR}/samba/lib/talloc
   ${CMAKE_SOURCE_DIR}/samba/librpc/ndr
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/
   ${CMAKE_CURRENT_BINARY_DIR}
)

include_directories(${SMB_INCLUDES})

set (RPC_IDL_FILES
   librpc/idl/policyagent.idl
   librpc/idl/samr.idl
   librpc/idl/dcom.idl
   librpc/idl/nfs4acl.idl
   librpc/idl/epmapper.idl
   librpc/idl/irpc.idl
   librpc/idl/winstation.idl
   librpc/idl/dbgidl.idl
   librpc/idl/notify.idl
   librpc/idl/wzcsvc.idl
   librpc/idl/msgsvc.idl
   librpc/idl/dnsserver.idl
   librpc/idl/trkwks.idl
   librpc/idl/lsa.idl
   librpc/idl/echo.idl
   librpc/idl/drsuapi.idl
   librpc/idl/orpc.idl
   librpc/idl/frsrpc.idl
   librpc/idl/mgmt.idl
   librpc/idl/svcctl.idl
   librpc/idl/netlogon.idl
   librpc/idl/krb5pac.idl
   librpc/idl/winsrepl.idl
   librpc/idl/eventlog.idl
   librpc/idl/security.idl
   librpc/idl/nbt.idl
   librpc/idl/winbind.idl
   librpc/idl/dssetup.idl
   librpc/idl/opendb.idl
   librpc/idl/wkssvc.idl
   librpc/idl/drsblobs.idl
   librpc/idl/srvsvc.idl
   librpc/idl/atsvc.idl
   librpc/idl/unixinfo.idl
   librpc/idl/protected_storage.idl
   librpc/idl/dsbackup.idl
   librpc/idl/w32time.idl
   librpc/idl/dcerpc.idl
   librpc/idl/dfs.idl
   librpc/idl/keysvc.idl
   librpc/idl/spoolss.idl
   librpc/idl/xattr.idl
   librpc/idl/oxidresolver.idl
   librpc/idl/frsapi.idl
   librpc/idl/misc.idl
   librpc/idl/wins.idl
   librpc/idl/schannel.idl
   librpc/idl/audiosrv.idl
   librpc/idl/sasl_helpers.idl
   librpc/idl/initshutdown.idl
   librpc/idl/browser.idl
   librpc/idl/winreg.idl
   librpc/idl/scerpc.idl
   librpc/idl/efs.idl
   librpc/idl/rot.idl
   librpc/idl/ntsvcs.idl
   librpc/idl/remact.idl
)

set (RPC_GEN_SRC
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_atsvc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_audiosrv.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_browser.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dbgidl.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dcerpc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dcom.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dfs.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dnsserver.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_drsblobs.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_drsuapi.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dsbackup.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_dssetup.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_echo.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_efs.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_epmapper.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_epmapper_c.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_eventlog.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_frsapi.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_frsrpc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_initshutdown.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_irpc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_keysvc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_krb5pac.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_lsa.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_mgmt.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_misc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_msgsvc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_nbt.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_netlogon.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_netlogon_c.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_nfs4acl.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_ntsvcs.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_orpc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_oxidresolver.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_oxidresolver_c.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_policyagent.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_protected_storage.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_remact.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_remact_c.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_rot.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_samr.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_scerpc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_schannel.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_security.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_spoolss.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_srvsvc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_svcctl.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_svcctl_c.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_trkwks.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_unixinfo.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_w32time.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_winbind.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_winreg.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_wins.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_winstation.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_wkssvc.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/ndr_wzcsvc.c
)

STRING(REPLACE ".c" ".h" RPC_GEN_HEADERS "${RPC_GEN_SRC}")
STRING(REGEX REPLACE ";[^;]*_c.h;" ";" RPC_GEN_HEADERS "${RPC_GEN_HEADERS}")

set (RPC_GEN_SRC ${RPC_GEN_SRC}
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/dcom_p.c
   ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/tables.c)

# To enable sasl auth add:
#   auth/gensec/cyrus_sasl.c
# and define HAVE_LIBSASL2 in config.h

set (SAMBA_SRC
   auth/auth_anonymous.c
   auth/auth.c
   auth/auth_developer.c
   auth/auth_sam.c
   auth/auth_sam_reply.c
   auth/auth_util.c
   auth/auth_winbind.c
   auth/credentials/credentials.c
   auth/credentials/credentials_files.c
   auth/credentials/credentials_krb5.c
   auth/credentials/credentials_ntlm.c
   auth/gensec/gensec.c
   auth/gensec/gensec_gssapi.c
   auth/gensec/gensec_krb5.c
   auth/gensec/schannel.c
   auth/gensec/schannel_sign.c
   auth/gensec/schannel_state.c
   auth/gensec/socket.c
   auth/gensec/spnego.c
   auth/gensec/spnego_parse.c
   auth/kerberos/clikrb5.c
   auth/kerberos/gssapi_parse.c
   auth/kerberos/kerberos.c
   auth/kerberos/kerberos_heimdal.c
   auth/kerberos/kerberos_pac.c
   auth/kerberos/kerberos_util.c
   auth/kerberos/krb5_init_context.c
   auth/ntlm_check.c
   auth/ntlmssp/ntlmssp.c
   auth/ntlmssp/ntlmssp_client.c
   auth/ntlmssp/ntlmssp_parse.c
   auth/ntlmssp/ntlmssp_server.c
   auth/ntlmssp/ntlmssp_sign.c
   auth/pam_errors.c
   auth/sam.c
   dsdb/common/flag_mapping.c
   dsdb/samdb/cracknames.c
   dsdb/samdb/ldb_modules/entryUUID.c
   dsdb/samdb/ldb_modules/extended_dn.c
   dsdb/samdb/ldb_modules/kludge_acl.c
   dsdb/samdb/ldb_modules/local_password.c
   dsdb/samdb/ldb_modules/objectguid.c
   dsdb/samdb/ldb_modules/partition.c
   dsdb/samdb/ldb_modules/password_hash.c
   dsdb/samdb/ldb_modules/rootdse.c
   dsdb/samdb/ldb_modules/samba3sam.c
   dsdb/samdb/ldb_modules/samldb.c
   dsdb/samdb/ldb_modules/schema.c
   dsdb/samdb/ldb_modules/schema_syntax.c
   dsdb/samdb/samdb.c
   dsdb/samdb/samdb_privilege.c
   lib/charset/charcnv.c
   lib/charset/iconv.c
   lib/charset/util_unistr.c
   libcli/auth/credentials.c
   libcli/auth/smbdes.c
   libcli/auth/smbencrypt.c
   libcli/cliconnect.c
   libcli/clifile.c
   libcli/composite/composite.c
   libcli/ldap/ldap_bind.c
   libcli/ldap/ldap.c
   libcli/ldap/ldap_client.c
   libcli/ldap/ldap_controls.c
   libcli/ldap/ldap_ildap.c
   libcli/ldap/ldap_msg.c
   libcli/ldap/ldap_ndr.c
   libcli/nbt/namequery.c
   libcli/nbt/nbtname.c
   libcli/nbt/nbtsocket.c
   libcli/raw/clisession.c
   libcli/raw/clisocket.c
   libcli/raw/clitransport.c
   libcli/raw/clitree.c
   libcli/raw/rawacl.c
   libcli/raw/rawdate.c
   libcli/raw/raweas.c
   libcli/raw/rawfile.c
   libcli/raw/rawfileinfo.c
   libcli/raw/rawfsinfo.c
   libcli/raw/rawnegotiate.c
   libcli/raw/rawnotify.c
   libcli/raw/rawreadwrite.c
   libcli/raw/rawrequest.c
   libcli/raw/rawsetfileinfo.c
   libcli/raw/rawtrans.c
   libcli/raw/smb_signing.c
   libcli/resolve/bcast.c
   libcli/resolve/host.c
   libcli/resolve/nbtlist.c
   libcli/resolve/resolve.c
   libcli/resolve/wins.c
   libcli/security/dom_sid.c
   libcli/security/privilege.c
   libcli/security/sddl.c
   libcli/security/security_descriptor.c
   libcli/security/security_token.c
   libcli/smb2/cancel.c
   libcli/smb2/close.c
   libcli/smb2/connect.c
   libcli/smb2/create.c
   libcli/smb2/ioctl.c
   libcli/smb2/negprot.c
   libcli/smb2/read.c
   libcli/smb2/request.c
   libcli/smb2/session.c
   libcli/smb2/tcon.c
   libcli/smb2/transport.c
   libcli/smb2/write.c
   libcli/smb_composite/connect.c
   libcli/smb_composite/savefile.c
   libcli/smb_composite/sesssetup.c
   libcli/util/asn1.c
   libcli/util/doserr.c
   libcli/util/errormap.c
   libcli/util/nterr.c
   lib/cmdline/credentials.c
   lib/cmdline/popt_common.c
   lib/cmdline/popt_credentials.c
   lib/com/classes/simple.c
   lib/com/dcom/main.c
   lib/com/dcom/tables.c
   lib/com/main.c
   lib/compression/mszip.c
   lib/com/tables.c
   lib/crypto/arcfour.c
   lib/crypto/crc32.c
   lib/crypto/hmacmd5.c
   lib/crypto/md4.c
   lib/crypto/md5.c
   lib/db_wrap.c
   lib/events/events.c
   lib/events/events_standard.c
   lib/gendb.c
   lib/ldb/common/attrib_handlers.c
   lib/ldb/common/ldb_attributes.c
   lib/ldb/common/ldb.c
   lib/ldb/common/ldb_controls.c
   lib/ldb/common/ldb_debug.c
   lib/ldb/common/ldb_dn.c
   lib/ldb/common/ldb_ldif.c
   lib/ldb/common/ldb_match.c
   lib/ldb/common/ldb_modules.c
   lib/ldb/common/ldb_msg.c
   lib/ldb/common/ldb_parse.c
   lib/ldb/common/ldb_utf8.c
   lib/ldb/common/qsort.c
   lib/ldb/ldb_ildap/ldb_ildap.c
   lib/ldb/ldb_tdb/ldb_cache.c
   lib/ldb/ldb_tdb/ldb_index.c
   lib/ldb/ldb_tdb/ldb_pack.c
   lib/ldb/ldb_tdb/ldb_search.c
   lib/ldb/ldb_tdb/ldb_tdb.c
   lib/ldb/ldb_tdb/ldb_tdb_wrap.c
   lib/ldb/modules/asq.c
   lib/ldb/modules/ldb_map.c
   lib/ldb/modules/ldb_map_inbound.c
   lib/ldb/modules/ldb_map_outbound.c
   lib/ldb/modules/objectclass.c
   lib/ldb/modules/operational.c
   lib/ldb/modules/paged_results.c
   lib/ldb/modules/paged_searches.c
   lib/ldb/modules/rdn_name.c
   lib/ldb/modules/skel.c
   lib/ldb/modules/sort.c
   lib/ldb/samba/ldif_handlers.c
   lib/messaging/messaging.c
   lib/replace/replace.c
   lib/replace/replace.h
   librpc/ndr/libndr.h
   librpc/ndr/ndr_basic.c
   librpc/ndr/ndr.c
   librpc/ndr/ndr_compression.c
   librpc/ndr/ndr_dcom.c
   librpc/ndr/ndr_drsuapi.c
   librpc/ndr/ndr_krb5pac.c
   librpc/ndr/ndr_misc.c
   librpc/ndr/ndr_orpc.c
   librpc/ndr/ndr_sec_helper.c
   librpc/ndr/ndr_spoolss_buf.c
   librpc/ndr/ndr_string.c
   librpc/ndr/uuid.c
   librpc/rpc/dcerpc_auth.c
   librpc/rpc/dcerpc.c
   librpc/rpc/dcerpc_connect.c
   librpc/rpc/dcerpc_error.c
   librpc/rpc/dcerpc.h
   librpc/rpc/dcerpc_schannel.c
   librpc/rpc/dcerpc_smb2.c
   librpc/rpc/dcerpc_smb.c
   librpc/rpc/dcerpc_sock.c
   librpc/rpc/dcerpc_util.c
   librpc/rpc/table.c
   lib/socket/connect.c
   lib/socket/connect_multi.c
   lib/socket/interface.c
   lib/socket/netif.c
   lib/socket/socket.c
   lib/socket/socket_ipv4.c
   lib/socket/socket_ipv6.c
   lib/socket/socket_unix.c
   lib/stream/packet.c
   lib/talloc/talloc.c
   lib/tdb/common/error.c
   lib/tdb/common/freelist.c
   lib/tdb/common/io.c
   lib/tdb/common/lock.c
   lib/tdb/common/open.c
   lib/tdb/common/tdb.c
   lib/tdb/common/transaction.c
   lib/tdb/common/traverse.c
   lib/tls/tls.c
   lib/tls/tlscert.c
   lib/util/data_blob.c
   lib/util/debug.c
   lib/util/fault.c
   lib/util/genrand.c
   lib/util/idtree.c
   lib/util/module.c
   lib/util/ms_fnmatch.c
   lib/util/signal.c
   lib/util/system.c
   lib/util/time.c
   lib/util/unix_privs.c
   lib/util/util.c
   lib/util/util_file.c
   lib/util/util_str.c
   lib/util/util_strlist.c
   lib/util/util_tdb.c
   lib/util/xfile.c
   nbt_server/wins/wins_ldb.c
   nsswitch/wb_common.c
   param/loadparm.c
   param/params.c
   param/secrets.c
   param/util.c
   ${CMAKE_CURRENT_BINARY_DIR}/dynconfig.c
)

configure_file(dynconfig.c.in ${CMAKE_CURRENT_BINARY_DIR}/dynconfig.c @ONLY)

add_definitions(-D_SAMBA_BUILD_=4 -DHAVE_CONFIG_H)

add_custom_command(
   OUTPUT ${RPC_GEN_SRC}
   COMMAND ${PERL} ${CMAKE_CURRENT_SOURCE_DIR}/librpc/tables.pl --output=${CMAKE_CURRENT_BINARY_DIR}/librcp/gen_ndr/tables.c
           ${RPC_GEN_HEADERS} > ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/tables.x &&
           mv -f ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/tables.x ${CMAKE_CURRENT_BINARY_DIR}/librpc/gen_ndr/tables.c
   DEPENDS ${RPC_GEN_HEADERS}
   COMMENT "Generating IDL tables")

# Partial build does not work here for out of source builds.
add_custom_command(
   OUTPUT ${RPC_GEN_HEADERS}
          COMMAND PERL=${PERL} srcdir="${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/build_idl.sh" FULL --includedir ${CMAKE_CURRENT_SOURCE_DIR}/librpc/idl
   DEPENDS ${RPC_IDL_FILES}
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMENT "Generating IDL C-Sources")

add_library(samba-static STATIC ${RPC_GEN_SRC} ${SAMBA_SRC} ${RPC_GEN_HEADERS})
# Link against static heimdal-gssapi ldflags returned by pkg-config
# Link against libhdb as well since it is missing from the list
target_link_libraries (samba-static ${GSSAPI_STATIC_LDFLAGS} ${GNUTLS_STATIC_LDFLAGS} hdb)
set_target_properties (samba-static PROPERTIES CMAKE_POSITION_INDEPENDENT_CODE TRUE)
