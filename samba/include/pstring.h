/*
 * SPDX-FileCopyrightText: Martin Pool		      2002
 * SPDX-FileCopyrightText: Paul Ashton                  1998-2000
 * SPDX-FileCopyrightText: Luke Kenneth Casson Leighton 1996-2000
 * SPDX-FileCopyrightText: John H Terpstra              1996-2000
 * SPDX-FileCopyrightText: Andrew Tridgell              1992-2000
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   samba -- Unix SMB/CIFS implementation.

   ugly string types from Samba3. Will be removed 
   with glee when we finally don't use them.
   
   Copyright (C) Andrew Tridgell              1992-2000
   Copyright (C) John H Terpstra              1996-2000
   Copyright (C) Luke Kenneth Casson Leighton 1996-2000
   Copyright (C) Paul Ashton                  1998-2000
   Copyright (C) Martin Pool		      2002
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _PSTRING
#define _PSTRING

#define PSTRING_LEN 1024
#define FSTRING_LEN 256

typedef char pstring[PSTRING_LEN];
typedef char fstring[FSTRING_LEN];

#define pstrcpy(d,s) safe_strcpy((d), (s),sizeof(pstring)-1)
#define pstrcat(d,s) safe_strcat((d), (s),sizeof(pstring)-1)
#define fstrcpy(d,s) safe_strcpy((d),(s),sizeof(fstring)-1)
#define fstrcat(d,s) safe_strcat((d),(s),sizeof(fstring)-1)

#endif
