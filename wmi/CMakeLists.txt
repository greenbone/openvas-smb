# SPDX-FileCopyrightText: 2015 Greenbone AG
# SPDX-License-Identifier: GPL-2.0-or-later

# openvas-smb
# $Id$
# Description: Cmake control for the wmiclient library.
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

set (SMB_INCLUDES
   ${CMAKE_SOURCE_DIR}/samba
   ${CMAKE_SOURCE_DIR}/samba/include
   ${CMAKE_SOURCE_DIR}/samba/lib
   ${CMAKE_SOURCE_DIR}/samba/lib/replace
   ${CMAKE_SOURCE_DIR}/samba/ntvfs
   ${CMAKE_SOURCE_DIR}/samba/param
   ${CMAKE_BINARY_DIR}/samba
)
include_directories(${SMB_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(WMI_SHARED_SRC
   program_args_utils.c
   wmicore.c
   wbemdata.c
)

set(WMICLIENT_SRC
   wmicso.c
   wmireg.c
   wmirsop.c
   winsmb.c
   wmiclient.map
)

set(WMI_HDR
   openvas_smb_interface.h
   openvas_wmi_interface.h
)

add_library(openvas_wmiclient SHARED ${WMICLIENT_SRC} ${WMI_SHARED_SRC})
target_link_libraries(openvas_wmiclient samba-static dl ${GNUTLS_LIBRARIES} ${GSSAPI_LIBRARIES} ${POPT_LIBRARIES} ${LINKER_HARDENING_FLAGS})

set_target_properties (openvas_wmiclient PROPERTIES SOVERSION "${PROJECT_VERSION_MAJOR}")
set_target_properties (openvas_wmiclient PROPERTIES VERSION "${CPACK_PACKAGE_VERSION}")
set_target_properties (openvas_wmiclient PROPERTIES PUBLIC_HEADER "${WMI_HDR}")
set_target_properties (openvas_wmiclient PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/wmiclient.map")

add_executable(wmic wmic.c ${WMI_SHARED_SRC})
target_link_libraries(wmic openvas_wmiclient samba-static ${GNUTLS_LIBRARIES}  ${POPT_LIBRARIES} dl ${LINKER_HARDENING_FLAGS})
install (TARGETS wmic RUNTIME DESTINATION ${BINDIR})

install (TARGETS openvas_wmiclient
         LIBRARY DESTINATION ${LIBDIR}
         ARCHIVE DESTINATION ${LIBDIR}
         PUBLIC_HEADER DESTINATION "${INCLUDEDIR}/openvas/smb")
