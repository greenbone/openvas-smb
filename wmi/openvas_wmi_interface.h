/*
 * SPDX-FileCopyrightText: 2009 Greenbone AG, http://www.greenbone.net
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* OpenVAS
 *
 * $Id$
 * Description: API protos describing the interface of a wmi interface
 * implementation.
 *
 * Authors:
 * Jan-Oliver Wagner <jan-oliver.wagner@greenbone.net>
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
 * @file openvas_wmi_interface.h
 * @brief API protos describing the interface of a wmi interface implementation.
 *
 * This file contains API protos describing the interface of a wmi
 * interface implementation.
 */

#ifndef _NASL_OPENVAS_WMI_INTERFACE_H
#define _NASL_OPENVAS_WMI_INTERFACE_H

typedef struct IWbemServices * WMI_HANDLE;

char *wmi_versioninfo ();

WMI_HANDLE
wmi_connect (int, char **);

int wmi_close (WMI_HANDLE);
int wmi_query (WMI_HANDLE, const char *, char **);

WMI_HANDLE
wmi_connect_rsop (int, char **);

int wmi_query_rsop (WMI_HANDLE, const char *, char **);

WMI_HANDLE
wmi_connect_reg (int, char **);

int wmi_reg_get_sz (WMI_HANDLE, unsigned int, const char *, const char *,
                    char **);
int wmi_reg_enum_value (WMI_HANDLE, unsigned int, const char *, char **);
int wmi_reg_enum_key (WMI_HANDLE, unsigned int, const char *, char **);
int wmi_reg_get_bin_val (WMI_HANDLE, unsigned int, const char *, const char *,
                         char **);
int wmi_reg_get_dword_val (WMI_HANDLE, unsigned int, const char *, const char *,
                           char **);
int wmi_reg_get_ex_string_val (WMI_HANDLE, unsigned int, const char *,
                               const char *, char **);
int wmi_reg_get_mul_string_val (WMI_HANDLE, unsigned int, const char *,
                                const char *, char **);
int wmi_reg_get_qword_val (WMI_HANDLE, unsigned int, const char *, const char *,
                           char **);

#endif
