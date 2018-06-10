# openvas-smb
# $Id$
# Description: Top-level cmake control for the openvas-smb library.
#
# Authors:
# Andre Heinecke <aheinecke@greenbone.net>
#
# Copyright:
# Copyright (C) 2015-2016 Greenbone Networks GmbH
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

cmake_minimum_required(VERSION 3.0)

project(openvas-smb
  VERSION 1.0.4
  LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Perl REQUIRED)

include (FindPkgConfig)

## Retrieve git revision (at configure time)
find_package (Git)

macro (Git_GET_REVISION dir variable)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                  WORKING_DIRECTORY ${dir}
                  OUTPUT_VARIABLE GIT_BRANCH
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
                  WORKING_DIRECTORY ${dir}
                  OUTPUT_VARIABLE GIT_COMMIT_HASH
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  string (REPLACE "/" "_" GIT_BRANCH ${GIT_BRANCH})
  set (${variable} "${GIT_COMMIT_HASH}-${GIT_BRANCH}")
endmacro (Git_GET_REVISION)

if (NOT CMAKE_BUILD_TYPE MATCHES "Release")
  if (EXISTS "${CMAKE_SOURCE_DIR}/.git/")
    if (GIT_FOUND)
      Git_GET_REVISION(. ProjectRevision)
      set (GIT_REVISION "~git-${ProjectRevision}")
    else (GIT_FOUND)
      set (GIT_REVISION "~git")
    endif (GIT_FOUND)
  endif (EXISTS "${CMAKE_SOURCE_DIR}/.git/")
endif (NOT CMAKE_BUILD_TYPE MATCHES "Release")

# Set beta version if this is a beta release series,
# unset if this is a stable release series.
set (PROJECT_BETA_RELEASE 0)

if (GIT_REVISION)
  set (PROJECT_VERSION_GIT "${GIT_REVISION}")
endif (GIT_REVISION)

# If PROJECT_BETA_RELEASE is set, the version string will be set to:
#   "major.minor+beta${PROJECT_BETA_RELEASE}"
# If PROJECT_BETA_RELEASE is NOT set, the version string will be set to:
#   "major.minor.patch"
if (PROJECT_BETA_RELEASE)
  set (PROJECT_VERSION_SUFFIX "+beta${PROJECT_BETA_RELEASE}")
else (PROJECT_BETA_RELEASE)
  set (PROJECT_VERSION_SUFFIX ".${PROJECT_VERSION_PATCH}")
endif (PROJECT_BETA_RELEASE)

set (PROJECT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}${PROJECT_VERSION_SUFFIX}")

# CPACK settings adapted from openvas-libraries
set (CPACK_CMAKE_GENERATOR "Unix Makefiles")
set (CPACK_GENERATOR "TGZ")
set (CPACK_INSTALL_CMAKE_PROJECTS ".;openvas-smb;ALL;/")
set (CPACK_MODULE_PATH "")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_TOPLEVEL_TAG "")
set (CPACK_SYSTEM_NAME "")
set (CPACK_TOPLEVEL_TAG "")

set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION_STRING}${PROJECT_VERSION_GIT}")

set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_VENDOR "The OpenVAS Project")

# There is a lot of generated code in the build directory.
# Please run CPack only from an out of source build.
set (CPACK_SOURCE_IGNORE_FILES
   "${CMAKE_BINARY_DIR}"
   "/.git/"
   "/.hg/"
   "swp$"
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

if (NOT DATADIR)
  set (DATADIR "${CMAKE_INSTALL_PREFIX}/share")
endif (NOT DATADIR)

if (NOT INCLUDEDIR)
  set (INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
endif (NOT INCLUDEDIR)

message ("-- Install prefix: ${CMAKE_INSTALL_PREFIX}")

pkg_check_modules (GNUTLS REQUIRED gnutls>=3.2.15)
pkg_check_modules (GSSAPI REQUIRED heimdal-gssapi>=1.6.0)
pkg_check_modules (POPT REQUIRED popt)

if(${GSSAPI_VERSION} VERSION_LESS "7.5.0")
	add_definitions(-DOLD_HEIMDAL)
endif()
set (CMAKE_C_FLAGS "-fPIC -O2 -D_FORTIFY_SOURCE=2 -fstack-protector ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "-Wno-unused-result -fno-strict-aliasing ${CMAKE_C_FLAGS}")
set (LINKER_HARDENING_FLAGS     "-Wl,-z,relro -Wl,-z,now")

## Version

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
add_subdirectory(doc)
