# OpenVAS
# $Id$
# Description: CMakefile for the OpenVAS CLI documentation
#
# Authors:
# Matthew Mundell <matthew.mundell@greenbone.net>
#
# Copyright:
# Copyright (C) 2016 Greenbone Networks GmbH
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

# Howto update man pages:
# With "make man" and "make man-html", new files with ".8" and ".8.html" are
# created in the build/doc directory. These files need to be copied to the
# source doc directory and committed as "generated from .8.xml", see log
# history for examples.

## Find xml2man

message (STATUS "Looking for xmltoman...")

find_program (XMLTOMAN_EXECUTABLE xmltoman
  PATHS /bin /usr/bin /usr/local/bin/
  DOC "xmltoman location")

message (STATUS "Looking for xmltoman... ${XMLTOMAN_EXECUTABLE}")

find_program (XMLMANTOHTML_EXECUTABLE xmlmantohtml
  PATHS /bin /usr/bin /usr/local/bin/
  DOC "xmlmantohtml location")

message (STATUS "Looking for xmlmantohtml... ${XMLMANTOHTML_EXECUTABLE}")

# FIX find_program seems to follow symlink
if (XMLMANTOHTML_EXECUTABLE)
  set (XMLMANTOHTML_EXECUTABLE xmlmantohtml)
endif (XMLMANTOHTML_EXECUTABLE)

## build

include (FindDoxygen)

## Emit warning if Doxygen not found.
## Otherwise, create doc and doc-full targets.
if (NOT DOXYGEN_EXECUTABLE)
  message (STATUS "Doxygen is required to build the HTML docs.")
else (NOT DOXYGEN_EXECUTABLE)
  add_custom_target (doc COMMENT "Building documentation..."
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/.built-html ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  add_custom_target (doc-full COMMENT "Building documentation..."
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_full ${CMAKE_CURRENT_BINARY_DIR}/.built-html_full)
add_custom_command (OUTPUT .built-html
  COMMAND sh
  ARGS -c \"${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile && touch ${CMAKE_CURRENT_BINARY_DIR}/.built-html\;\"
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_command (OUTPUT .built-html_full
  COMMAND sh
  ARGS -c \"${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_full && touch ${CMAKE_CURRENT_BINARY_DIR}/.built-html_full\;\"
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_full)
endif (NOT DOXYGEN_EXECUTABLE)

if (XMLTOMAN_EXECUTABLE)
  add_custom_target (man COMMENT "Building manual page..."
    DEPENDS winexe.1 wmic.1)

  add_custom_command (OUTPUT winexe.1
    COMMAND sh
    ARGS -c \"${XMLTOMAN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/winexe.1.xml > ${CMAKE_CURRENT_BINARY_DIR}/winexe.1\;\"
    DEPENDS winexe.1.xml)

  add_custom_command (OUTPUT wmic.1
    COMMAND sh
    ARGS -c \"${XMLTOMAN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wmic.1.xml > ${CMAKE_CURRENT_BINARY_DIR}/wmic.1\;\"
    DEPENDS wmic.1.xml)

else (XMLTOMAN_EXECUTABLE)
  message (STATUS "WARNING: xmltoman is required to generate manpages.")
  add_custom_command (OUTPUT wmiexe.1
    COMMAND echo "[Error: xmltoman required to see manpage here]" > wmiexe.1)
  add_custom_command (OUTPUT wmic.1
    COMMAND echo "[Error: xmltoman required to see manpage here]" >wmic.1)
endif (XMLTOMAN_EXECUTABLE)

if (XMLMANTOHTML_EXECUTABLE)
  add_custom_target (man-html COMMENT "Building manual page HTML..."
    DEPENDS winexe.html wmic.html)

  add_custom_command (OUTPUT winexe.html
    COMMAND sh
    ARGS -c \"${XMLMANTOHTML_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/winexe.1.xml > ${CMAKE_CURRENT_BINARY_DIR}/winexe.html\;\"
    DEPENDS winexe.1.xml)

  add_custom_command (OUTPUT wmic.html
    COMMAND sh
    ARGS -c \"${XMLMANTOHTML_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/wmic.1.xml > ${CMAKE_CURRENT_BINARY_DIR}/wmic.html\;\"
    DEPENDS wmic.1.xml)

else (XMLMANTOHTML_EXECUTABLE)
  message (STATUS "WARNING: xmlmantohtml is required for manpage in HTML docs.")
  add_custom_command (OUTPUT winexe.html
    COMMAND echo "[Error: xmlmantohtml required to see manpage here]" > winexe.html)
  add_custom_command (OUTPUT wmic.html
    COMMAND echo "[Error: xmlmantohtml required to see manpage here]" > wmic.html)
endif (XMLMANTOHTML_EXECUTABLE)

install (FILES winexe.1 wmic.1
  DESTINATION ${DATADIR}/man/man1 )
