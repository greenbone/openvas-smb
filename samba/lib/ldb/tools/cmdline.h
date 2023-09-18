/*
 * SPDX-FileCopyrightText: Andrew Tridgell  2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   ldb database library - command line handling for ldb tools

   Copyright (C) Andrew Tridgell  2005

     ** NOTE! The following LGPL license applies to the ldb
     ** library. This does NOT imply that all of Samba is released
     ** under the LGPL
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <popt.h>

struct ldb_cmdline {
	const char *url;
	enum ldb_scope scope;
	const char *basedn;
	int interactive;
	int sorted;
	const char *editor;
	int verbose;
	int recursive;
	int all_records;
	int nosync;
	const char **options;
	int argc;
	const char **argv;
	int num_records;
	int num_searches;
	const char *sasl_mechanism;
	const char *input;
	const char *output;
	char **controls;
};

struct ldb_cmdline *ldb_cmdline_process(struct ldb_context *ldb, int argc, const char **argv,
					void (*usage)(void));


struct ldb_control **parse_controls(void *mem_ctx, char **control_strings);
int handle_controls_reply(struct ldb_control **reply, struct ldb_control **request);
