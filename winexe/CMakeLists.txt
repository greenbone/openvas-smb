# openvas-smb
# $Id$
# Description: Top-level cmake control for the winexe tool.
#
# Authors:
# Andre Heinecke <aheinecke@greenbone.net>
#
# Copyright:
# Copyright (C) 2015 Greenbone Networks GmbH
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

set (SHARED_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/service.c
   ${CMAKE_CURRENT_SOURCE_DIR}/async.c
   ${CMAKE_CURRENT_BINARY_DIR}/winexesvc_exe.c
)

set (WINEXE_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/winexe.c
)

set (WINCMD_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/wincmd.c
)

set (SMB_INCLUDES
   ${CMAKE_SOURCE_DIR}/samba
   ${CMAKE_SOURCE_DIR}/samba/include
   ${CMAKE_SOURCE_DIR}/samba/lib
   ${CMAKE_SOURCE_DIR}/samba/lib/popt
   ${CMAKE_SOURCE_DIR}/samba/lib/replace
   ${CMAKE_SOURCE_DIR}/samba/ntvfs
   ${CMAKE_SOURCE_DIR}/samba/param
   ${CMAKE_SOURCE_DIR}/samba/torture
   ${CMAKE_SOURCE_DIR}/samba/winexe
   ${CMAKE_BINARY_DIR}/samba
)
include_directories(${SMB_INCLUDES})

find_program(MINGW_GCC i586-mingw32msvc-gcc)

if (NOT MINGW_GCC)
  message(SEND_ERROR "i586-mingw32msvc-gcc not found in PATH.\nIt is needed to build the Windows Service for winexe.")
endif(NOT MINGW_GCC)

set (WINEXESVC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/winexesvc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/winservice.c
)

set (BINTOC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.c
)

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/winexesvc.exe
   COMMAND ${MINGW_GCC} ${WINEXESVC_SOURCES} -o winexesvc.exe
   DEPENDS ${WINEXESVC_SOURCES})

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/winexesvc_exe.c
   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin2c ${CMAKE_CURRENT_SOURCE_DIR}/winexesvc_exe
           ${CMAKE_CURRENT_BINARY_DIR}/winexesvc.exe > ${CMAKE_CURRENT_BINARY_DIR}/winexesvc_exe.c
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/winexesvc.exe ${CMAKE_CURRENT_BINARY_DIR}/bin2c)

add_executable(bin2c ${BINTOC_SOURCES})
add_executable(winexe ${WINEXE_SOURCES} ${SHARED_SOURCES})

add_library(openvas_wincmd SHARED ${WINCMD_SOURCES} ${SHARED_SOURCES})
target_link_libraries(openvas_wincmd samba-static ${GSSAPI_LIBRARIES})

target_link_libraries(winexe samba-static dl gnutls resolv ${GSSAPI_LIBRARIES})

install (TARGETS winexe RUNTIME DESTINATION ${BINDIR})

set_target_properties (openvas_wincmd PROPERTIES SOVERSION "${CPACK_PACKAGE_VERSION_MAJOR}")
set_target_properties (openvas_wincmd PROPERTIES VERSION "${CPACK_PACKAGE_VERSION}")

install (TARGETS openvas_wincmd
         LIBRARY DESTINATION ${LIBDIR}
         ARCHIVE DESTINATION ${LIBDIR})
