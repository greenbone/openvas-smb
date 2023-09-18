/*
 * SPDX-FileCopyrightText: 2009 Greenbone AG, http://www.greenbone.net
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* OpenVAS
 *
 * $Id$
 * Description: API protos describing the interface of a smb interface
 * implementation.
 *
 * Authors:
 * Chandrashekhar B <bchandra@secpod.com>
 *
 * Copyright:
 * Copyright (c) 2009 Greenbone AG, http://www.greenbone.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * (or any later version), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * @file openvas_smb_interface.h
 * @brief API protos describing the interface of a smb interface implementation.
 *
 * This file contains API protos describing the interface of a smb
 * interface implementation.
 */

#ifndef _NASL_OPENVAS_SMB_INTERFACE_H
#define _NASL_OPENVAS_SMB_INTERFACE_H

typedef long int SMB_HANDLE;

char * smb_versioninfo();
int smb_connect(const char *, const char *, const char *, const char *, SMB_HANDLE *);
int smb_close(SMB_HANDLE);
char * smb_file_SDDL(SMB_HANDLE, const char *);
char * smb_file_OwnerSID(SMB_HANDLE, const char *);
char * smb_file_GroupSID(SMB_HANDLE, const char *);
char * smb_file_TrusteeRights(SMB_HANDLE, const char *);

#endif
