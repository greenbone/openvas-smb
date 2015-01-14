# openvas-smb
# $Id$
# Description: Top-level cmake control for the openvas-smb library.
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

cmake_minimum_required(VERSION 2.8.8)
project(openvas-smb)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Perl REQUIRED)

include (FindPkgConfig REQUIRED)

## Retrieve svn revision (at configure time)
#  Not using Subversion_WC_INFO, as it would have to connect to the repo
find_program (SVN_EXECUTABLE svn DOC "subversion command line client")

macro (Subversion_GET_REVISION dir variable)
  execute_process (COMMAND ${SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}/${dir}
    OUTPUT_VARIABLE ${variable}
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string (REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*"
    "\\2" ${variable} "${${variable}}")
endmacro (Subversion_GET_REVISION)

if (NOT CMAKE_BUILD_TYPE MATCHES "Release")
  if (EXISTS "${CMAKE_SOURCE_DIR}/.svn/")
    if (SVN_EXECUTABLE)
      Subversion_GET_REVISION(. ProjectRevision)
      set (SVN_REVISION ".SVN.r${ProjectRevision}")
    else (SVN_EXECUTABLE)
      set (SVN_REVISION ".SVN")
    endif (SVN_EXECUTABLE)
  endif (EXISTS "${CMAKE_SOURCE_DIR}/.svn/")
endif (NOT CMAKE_BUILD_TYPE MATCHES "Release")

# CPACK settings adapted from openvas-libraries
set (CPACK_CMAKE_GENERATOR "Unix Makefiles")
set (CPACK_GENERATOR "TGZ")
set (CPACK_INSTALL_CMAKE_PROJECTS ".;openvas-smb;ALL;/")
set (CPACK_MODULE_PATH "")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set (CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/README")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_TOPLEVEL_TAG "")
set (CPACK_SYSTEM_NAME "")
set (CPACK_TOPLEVEL_TAG "")

set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "+beta2${SVN_REVISION}")
set (CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}${CPACK_PACKAGE_VERSION_PATCH}")

set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_VENDOR "The OpenVAS Project")

# There is a lot of generated code in the build directory.
# Please run CPack only from an out of source build.
set (CPACK_SOURCE_IGNORE_FILES
   "/.svn/"
   "/.hg/"
   "/_CPack_Packages/"
   "/CMakeFiles/"
   "/build/"
   "swp$"
   "swo$"
   ".tar.gz"
)

include (CPack)

# Path settings below are taken from openvas-libaries

if (NOT EXEC_PREFIX)
  set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif (NOT EXEC_PREFIX)

if (NOT BINDIR)
  set (BINDIR "${EXEC_PREFIX}/bin")
endif (NOT BINDIR)

if (NOT LIBDIR)
  set (_DEFAULT_LIBRARY_INSTALL_DIR lib)
  if (EXISTS "${EXEC_PREFIX}/lib32/" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    set (_DEFAULT_LIBRARY_INSTALL_DIR lib32)
  endif (EXISTS "${EXEC_PREFIX}/lib32/" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  if (EXISTS "${CMAKE_INSTALL_PREFIX}/lib64/" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (_DEFAULT_LIBRARY_INSTALL_DIR lib64)
  endif (EXISTS "${CMAKE_INSTALL_PREFIX}/lib64/" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  set( LIBRARY_INSTALL_DIR "${_DEFAULT_LIBRARY_INSTALL_DIR}")
  set (LIBDIR "${EXEC_PREFIX}/${LIBRARY_INSTALL_DIR}")
endif (NOT LIBDIR)

message ("-- Install prefix: ${CMAKE_INSTALL_PREFIX}")

pkg_check_modules (GNUTLS REQUIRED gnutls>=2.12)

set (CMAKE_C_FLAGS "-s -fPIC -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -Wl,-z,relro -Wl,-z,now ${CMAKE_C_FLAGS}")

## Version

string (REPLACE "
" "" OPENVASLIB_VERSION ${CPACK_PACKAGE_VERSION})

string(SUBSTRING ${CPACK_PACKAGE_VERSION_PATCH} 0 1 BETA_INDICATOR)
if (BETA_INDICATOR MATCHES "^\\+")
  set (LIBOPENVASCONFIG_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.0")
else (BETA_INDICATOR MATCHES "^\\+")
  set (LIBOPENVASCONFIG_VERSION "${OPENVASLIB_VERSION}")
endif (BETA_INDICATOR MATCHES "^\\+")

configure_file (VERSION.in ${CMAKE_BINARY_DIR}/VERSION @ONLY)
configure_file (libopenvas_wmiclient.pc.in ${CMAKE_BINARY_DIR}/libopenvas_wmiclient.pc @ONLY)
configure_file (libopenvas_wincmd.pc.in ${CMAKE_BINARY_DIR}/libopenvas_wincmd.pc @ONLY)

install (FILES ${CMAKE_BINARY_DIR}/libopenvas_wmiclient.pc
         DESTINATION ${LIBDIR}/pkgconfig)

install (FILES ${CMAKE_BINARY_DIR}/libopenvas_wincmd.pc
         DESTINATION ${LIBDIR}/pkgconfig)

add_subdirectory(samba)
add_subdirectory(winexe)
add_subdirectory(wmi)
