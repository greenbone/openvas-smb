/*
 * SPDX-FileCopyrightText: Simo Sorce  2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   ldb database library

   Copyright (C) Simo Sorce  2005

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

/*
 *  Name: ldb
 *
 *  Component: ldb header
 *
 *  Description: defines attribute handlers
 *
 *  Author: Simo Sorce
 */


int ldb_handler_copy(		struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *in, struct ldb_val *out);

int ldb_handler_fold(		struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *in, struct ldb_val *out);

int ldb_canonicalise_Integer(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *in, struct ldb_val *out);

int ldb_comparison_Integer(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_comparison_binary(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_comparison_fold(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_canonicalise_dn(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *in, struct ldb_val *out);

int ldb_comparison_dn(		struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_comparison_objectclass(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_comparison_utctime(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *v1, const struct ldb_val *v2);

int ldb_canonicalise_utctime(	struct ldb_context *ldb, void *mem_ctx,
				const struct ldb_val *in, struct ldb_val *out);

