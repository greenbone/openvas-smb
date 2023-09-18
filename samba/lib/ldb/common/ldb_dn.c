/*
 * SPDX-FileCopyrightText: Simo Sorce 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   ldb database library

   Copyright (C) Simo Sorce 2005

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
 *  Component: ldb dn creation and manipulation utility functions
 *
 *  Description: - explode a dn into it's own basic elements
 *                 and put them in a structure (only if necessary)
 *               - manipulate ldb_dn structures
 *
 *  Author: Simo Sorce
 */

#include "includes.h"
#include <ctype.h>
#include "ldb/include/includes.h"

#define LDB_DN_NULL_FAILED(x) if (!(x)) goto failed

#define LDB_FREE(x) do { talloc_free(x); x = NULL; } while(0)

/**
   internal ldb exploded dn structures
*/
struct ldb_dn_component {

	char *name;
	struct ldb_val value;

	char *cf_name;
	struct ldb_val cf_value;
};

struct ldb_dn {

	struct ldb_context *ldb;

	/* Special DNs are always linearized */
	bool special;
	bool invalid;

	bool valid_case;

	char *linearized;
	char *casefold;

	unsigned int comp_num;
	struct ldb_dn_component *components;

};

/* strdn may be NULL */
struct ldb_dn *ldb_dn_new(void *mem_ctx, struct ldb_context *ldb, const char *strdn)
{
	struct ldb_dn *dn;

	if ( (! mem_ctx) || (! ldb)) return NULL;

	dn = talloc_zero(mem_ctx, struct ldb_dn);
	LDB_DN_NULL_FAILED(dn);

	dn->ldb = ldb;

	if (strdn) {
		if (strdn[0] == '@') {
			dn->special = true;
		}
		if (strncasecmp(strdn, "<GUID=", 6) == 0) {
			/* this is special DN returned when the
			 * exploded_dn control is used */
			dn->special = true;
			/* FIXME: add a GUID string to ldb_dn structure */
		} else if (strncasecmp(strdn, "<SID=", 8) == 0) {
			/* this is special DN returned when the
			 * exploded_dn control is used */
			dn->special = true;
			/* FIXME: add a SID string to ldb_dn structure */
		} else if (strncasecmp(strdn, "<WKGUID=", 8) == 0) {
			/* this is special DN returned when the
			 * exploded_dn control is used */
			dn->special = true;
			/* FIXME: add a WKGUID string to ldb_dn structure */
		}
		dn->linearized = talloc_strdup(dn, strdn);
	} else {
		dn->linearized = talloc_strdup(dn, "");
	}
	LDB_DN_NULL_FAILED(dn->linearized);

	return dn;

failed:
	talloc_free(dn);
	return NULL;
}

struct ldb_dn *ldb_dn_new_fmt(void *mem_ctx, struct ldb_context *ldb, const char *new_fmt, ...)
{
	struct ldb_dn *dn;
	char *strdn;
	va_list ap;

	if ( (! mem_ctx) || (! ldb)) return NULL;

	dn = talloc_zero(mem_ctx, struct ldb_dn);
	LDB_DN_NULL_FAILED(dn);

	dn->ldb = ldb;

	va_start(ap, new_fmt);
	strdn = talloc_vasprintf(dn, new_fmt, ap);
	va_end(ap);
	LDB_DN_NULL_FAILED(strdn);

	if (strdn[0] == '@') {
		dn->special = true;
	}
	if (strncasecmp(strdn, "<GUID=", 6) == 0) {
		/* this is special DN returned when the
		 * exploded_dn control is used */
		dn->special = true;
		/* FIXME: add a GUID string to ldb_dn structure */
	} else if (strncasecmp(strdn, "<SID=", 8) == 0) {
		/* this is special DN returned when the
		 * exploded_dn control is used */
		dn->special = true;
		/* FIXME: add a SID string to ldb_dn structure */
	} else if (strncasecmp(strdn, "<WKGUID=", 8) == 0) {
		/* this is special DN returned when the
		 * exploded_dn control is used */
		dn->special = true;
		/* FIXME: add a WKGUID string to ldb_dn structure */
	}
	dn->linearized = strdn;

	return dn;

failed:
	talloc_free(dn);
	return NULL;
}

static int ldb_dn_escape_internal(char *dst, const char *src, int len)
{
	const char *p, *s;
	char *d;
	int l;

	p = s = src;
	d = dst;

	while (p - src < len) {

		p += strcspn(p, ",=\n+<>#;\\\"");

		if (p - src == len) /* found no escapable chars */
			break;

		memcpy(d, s, p - s); /* copy the part of the string before the stop */
		d += (p - s); /* move to current position */

		if (*p) { /* it is a normal escapable character */
			*d++ = '\\';
			*d++ = *p++;
		} else { /* we have a zero byte in the string */
			strncpy(d, "\00", 3); /* escape the zero */
			d += 3;
			p++; /* skip the zero */
		}
		s = p; /* move forward */
	}

	/* copy the last part (with zero) and return */
	l = len - (s - src);
	memcpy(d, s, l + 1);

	/* return the length of the resulting string */
	return (l + (d - dst));
} 

char *ldb_dn_escape_value(void *mem_ctx, struct ldb_val value)
{
	char *dst;

	if (!value.length)
		return NULL;

	/* allocate destination string, it will be at most 3 times the source */
	dst = talloc_array(mem_ctx, char, value.length * 3 + 1);
	if ( ! dst) {
		talloc_free(dst);
		return NULL;
	}

	ldb_dn_escape_internal(dst, (const char *)value.data, value.length);

	dst = talloc_realloc(mem_ctx, dst, char, strlen(dst) + 1);

	return dst;
}

/*
  explode a DN string into a ldb_dn structure
  based on RFC4514 except that we don't support multiple valued RDNs
*/
static bool ldb_dn_explode(struct ldb_dn *dn)
{
	char *p, *data, *d, *dt, *t;
	bool trim = false;
	bool in_attr = false;
	bool in_value = false;
	bool in_quote = false;
	bool is_oid = false;
	bool escape = false;
	unsigned x;
	int l = 0;

	if ( ! dn || dn->invalid) return false;

	if (dn->components) {
		return true;
	}

	if ( ! dn->linearized) {
		return false;
	}

	/* Empty DNs */
	if (dn->linearized[0] == '\0') {
		return true;
	}

	/* Special DNs case */
	if (dn->special) {
		return true;
	}

	/* make sure we free this if alloced previously before replacing */
	talloc_free(dn->components);

	/* in the common case we have 3 or more components */
	/* make sure all components are zeroed, other functions depend on this */
	dn->components = talloc_zero_array(dn, struct ldb_dn_component, 3);
	if ( ! dn->components) {
		return false;
	}
	dn->comp_num = 0;

	/* Components data space is allocated here once */
	data = talloc_array(dn->components, char, strlen(dn->linearized) + 1);
	if (!data) {
		return false;
	}

	p = dn->linearized;
	in_attr = true;
	trim = true;
	t = NULL;
	d = dt = data;

	while (*p) {

		if (in_attr) {
			if (trim) {
				if (*p == ' ') {
					p++;
					continue;
				}

				/* first char */
				trim = false;

				if (isdigit(*p)) {
					is_oid = true;
				} else
				if ( ! isalpha(*p)) {
					/* not a digit nor an alpha, invalid attribute name */
					dn->invalid = true;
					goto failed;
				}
				
				*d++ = *p++;
				continue;
			}

			if (*p == ' ') {
				p++;
				/* valid only if we are at the end */
				trim = true;
				continue;
			}

			if (trim && (*p != '=')) {
				/* spaces/tabs are not allowed in attribute names */
				dn->invalid = true;
				goto failed;
			}

			if (*p == '=') {
				/* attribute terminated */
				in_attr = false;
				in_value = true;
				trim = true;
				l = 0;

				*d++ = '\0';
				dn->components[dn->comp_num].name = talloc_strdup(dn->components, dt);
				if ( ! dn->components[dn->comp_num].name) {
					/* ouch */
					goto failed;
				}

				dt = d;

				p++;
				continue;
			}

			if (is_oid && ( ! (isdigit(*p) || (*p == '.')))) {
				/* not a digit nor a dot, invalid attribute oid */
				dn->invalid = true;
				goto failed;
			} else
			if ( ! (isalpha(*p) || isdigit(*p) || (*p == '-'))) {
				/* not ALPHA, DIGIT or HYPHEN */
				dn->invalid = true;
				goto failed;
			}

			*d++ = *p++;
			continue;
		}

		if (in_value) {
			if (in_quote) {
				if (*p == '\"') {
					if (p[-1] != '\\') {
						p++;
						in_quote = false;
						continue;
					}
				}
				*d++ = *p++;
				l++;
				continue;
			}

			if (trim) {
				if (*p == ' ') {
					p++;
					continue;
				}

				/* first char */
				trim = false;

				if (*p == '\"') {
					in_quote = true;
					p++;
					continue;
				}
			}

			switch (*p) {

			/* TODO: support ber encoded values
			case '#':
			*/

			case ',':
				if (escape) {
					*d++ = *p++;
					l++;
					escape = false;
					continue;
				}
				/* ok found value terminator */

				if ( t ) {
					/* trim back */
					d -= (p - t);
					l -= (p - t);
				}

				in_attr = true;
				in_value = false;
				trim = true;

				p++;
				*d++ = '\0';
				dn->components[dn->comp_num].value.data = (uint8_t *)talloc_strdup(dn->components, dt);
				dn->components[dn->comp_num].value.length = l;
				if ( ! dn->components[dn->comp_num].value.data) {
					/* ouch ! */
					goto failed;
				}

				dt = d;

				dn->comp_num++;
				if (dn->comp_num > 2) {
					dn->components = talloc_realloc(dn,
									dn->components,
									struct ldb_dn_component,
									dn->comp_num + 1);
					if ( ! dn->components) {
						/* ouch ! */
						goto failed;
					}
					/* make sure all components are zeroed, other functions depend on this */
					memset(&dn->components[dn->comp_num], '\0', sizeof(struct ldb_dn_component));
				}

				continue;

			case '=':
			case '\n':
			case '+':
			case '<':
			case '>':
			case '#':
			case ';':
			case '\"':
				/* a string with not escaped specials is invalid (tested) */
				if ( ! escape) {
					dn->invalid = true;
					goto failed;
				}
				escape = false;

				*d++ = *p++;
				l++;

				if ( t ) t = NULL;
				break;

			case '\\':
				if ( ! escape) {
					escape = true;
					p++;
					continue;
				}
				escape = false;

				*d++ = *p++;
				l++;

				if ( t ) t = NULL;
				break;

			default:
				if (escape) {
	 				if (sscanf(p, "%02x", &x) != 1) {
						/* invalid escaping sequence */
						dn->invalid = true;
						goto failed;
					}
					escape = false;

					p += 2;
					*d++ = (unsigned char)x;
					l++;

					if ( t ) t = NULL;
					break;
				}

				if (*p == ' ') { 
					if ( ! t) t = p;
				} else {
					if ( t ) t = NULL;
				}

				*d++ = *p++;
				l++;
				
				break;
			}

		}
	}

	if (in_attr || in_quote) {
		/* invalid dn */
		dn->invalid = true;
		goto failed;
	}

	/* save last element */
	if ( t ) {
		/* trim back */
		d -= (p - t);
		l -= (p - t);
	}

	*d++ = '\0';
	dn->components[dn->comp_num].value.data = (uint8_t *)talloc_strdup(dn->components, dt);
	dn->components[dn->comp_num].value.length = l;

	if ( ! dn->components[dn->comp_num].value.data) {
		/* ouch */
		goto failed;
	}

	dn->comp_num++;

	talloc_free(data);
	return true;

failed:
	dn->comp_num = 0;
	talloc_free(dn->components);
	return false;
}

bool ldb_dn_validate(struct ldb_dn *dn)
{
	return ldb_dn_explode(dn);
}

const char *ldb_dn_get_linearized(struct ldb_dn *dn)
{
	int i, len;
	char *d, *n;

	if ( ! dn || ( dn->invalid)) return NULL;

	if (dn->linearized) return dn->linearized;

	if ( ! dn->components) {
		dn->invalid = true;
		return NULL;
	}

	if (dn->comp_num == 0) {
		dn->linearized = talloc_strdup(dn, "");
		if ( ! dn->linearized) return NULL;
		return dn->linearized;
	}

	/* calculate maximum possible length of DN */
	for (len = 0, i = 0; i < dn->comp_num; i++) {
		len += strlen(dn->components[i].name); /* name len */
		len += (dn->components[i].value.length * 3); /* max escaped data len */
		len += 2; /* '=' and ',' */
	}
	dn->linearized = talloc_array(dn, char, len);
	if ( ! dn->linearized) return NULL;

	d = dn->linearized;

	for (i = 0; i < dn->comp_num; i++) {

		/* copy the name */
		n = dn->components[i].name;
		while (*n) *d++ = *n++;

		*d++ = '=';

		/* and the value */
		d += ldb_dn_escape_internal( d,
				(char *)dn->components[i].value.data,
				dn->components[i].value.length);
		*d++ = ',';
	}

	*(--d) = '\0';

	/* don't waste more memory than necessary */
	dn->linearized = talloc_realloc(dn, dn->linearized, char, (d - dn->linearized + 1));

	return dn->linearized;
}

char *ldb_dn_alloc_linearized(void *mem_ctx, struct ldb_dn *dn)
{
	return talloc_strdup(mem_ctx, ldb_dn_get_linearized(dn));
}

/*
  casefold a dn. We need to casefold the attribute names, and canonicalize 
  attribute values of case insensitive attributes.
*/

static bool ldb_dn_casefold_internal(struct ldb_dn *dn)
{
	int i, ret;

	if ( ! dn || dn->invalid) return false;

	if (dn->valid_case) return true;

	if (( ! dn->components) && ( ! ldb_dn_explode(dn))) {
		return false;
	}

	for (i = 0; i < dn->comp_num; i++) {
		const struct ldb_attrib_handler *h;

		dn->components[i].cf_name = ldb_attr_casefold(dn->components, dn->components[i].name);
		if (!dn->components[i].cf_name) {
			goto failed;
		}

		h = ldb_attrib_handler(dn->ldb, dn->components[i].cf_name);
		ret = h->canonicalise_fn(dn->ldb, dn->components,
					 &(dn->components[i].value),
					 &(dn->components[i].cf_value));
		if (ret != 0) {
			goto failed;
		}
	}

	dn->valid_case = true;

	return true;

failed:
	for (i = 0; i < dn->comp_num; i++) {
		LDB_FREE(dn->components[i].cf_name);
		LDB_FREE(dn->components[i].cf_value.data);
	}
	return false;
}

const char *ldb_dn_get_casefold(struct ldb_dn *dn)
{
	int i, len;
	char *d, *n;

	if (dn->casefold) return dn->casefold;

	if (dn->special) { 
		dn->casefold = talloc_strdup(dn, dn->linearized);
		if (!dn->casefold) return NULL;
		dn->valid_case = true;
		return dn->casefold;
	}

	if ( ! ldb_dn_casefold_internal(dn)) {
		return NULL;
	}

	if (dn->comp_num == 0) {
		if (dn->linearized && dn->linearized[0] == '\0') {
			/* hmm a NULL dn, should we faild casefolding ? */
			dn->casefold = talloc_strdup(dn, "");
			return dn->casefold;
		}
		/* A DN must be NULL, special, or have components */
		dn->invalid = true;
		return NULL;
	}

	/* calculate maximum possible length of DN */
	for (len = 0, i = 0; i < dn->comp_num; i++) {
		len += strlen(dn->components[i].cf_name); /* name len */
		len += (dn->components[i].cf_value.length * 3); /* max escaped data len */
		len += 2; /* '=' and ',' */
	}
	dn->casefold = talloc_array(dn, char, len);
	if ( ! dn->casefold) return NULL;

	d = dn->casefold;

	for (i = 0; i < dn->comp_num; i++) {

		/* copy the name */
		n = dn->components[i].cf_name;
		while (*n) *d++ = *n++;

		*d++ = '=';

		/* and the value */
		d += ldb_dn_escape_internal( d,
				(char *)dn->components[i].cf_value.data,
				dn->components[i].cf_value.length);
		*d++ = ',';
	}
	*(--d) = '\0';

	/* don't waste more memory than necessary */
	dn->casefold = talloc_realloc(dn, dn->casefold, char, strlen(dn->casefold) + 1);

	return dn->casefold;
}

char *ldb_dn_alloc_casefold(void *mem_ctx, struct ldb_dn *dn)
{
	return talloc_strdup(mem_ctx, ldb_dn_get_casefold(dn));
}

/* Determine if dn is below base, in the ldap tree.  Used for
 * evaluating a subtree search.
 * 0 if they match, otherwise non-zero
 */

int ldb_dn_compare_base(struct ldb_dn *base, struct ldb_dn *dn)
{
	int ret;
	int n_base, n_dn;

	if ( ! base || base->invalid) return 1;
	if ( ! dn || dn->invalid) return -1;

	if (( ! base->valid_case) || ( ! dn->valid_case)) {
		if (base->linearized && dn->linearized) {
			/* try with a normal compare first, if we are lucky
			 * we will avoid exploding and casfolding */
			int dif;
			dif = strlen(dn->linearized) - strlen(base->linearized);
			if (dif < 0) return dif;
			if (strcmp(base->linearized, &dn->linearized[dif]) == 0) return 0;
		}

		if ( ! ldb_dn_casefold_internal(base)) {
			return 1;
		}

		if ( ! ldb_dn_casefold_internal(dn)) {
			return -1;
		}

	}

	/* if base has more components,
	 * they don't have the same base */
	if (base->comp_num > dn->comp_num) {
		return (dn->comp_num - base->comp_num);
	}

	if (dn->comp_num == 0) {
		if (dn->special && base->special) {
			return strcmp(base->linearized, dn->linearized);
		} else if (dn->special) {
			return -1;
		} else if (base->special) {
			return 1;
		} else {
			return 0;
		}
	}

	n_base = base->comp_num - 1;
	n_dn = dn->comp_num - 1;

	while (n_base >= 0) {
		/* compare attr names */
		ret = strcmp(base->components[n_base].cf_name, dn->components[n_dn].cf_name);
		if (ret != 0) return ret;

		/* compare attr.cf_value. */ 
		if (base->components[n_base].cf_value.length != dn->components[n_dn].cf_value.length) {
			return base->components[n_base].cf_value.length - dn->components[n_dn].cf_value.length;
		}
		ret = strcmp((char *)base->components[n_base].cf_value.data, (char *)dn->components[n_dn].cf_value.data);
		if (ret != 0) return ret;

		n_base--;
		n_dn--;
	}

	return 0;
}

/* compare DNs using casefolding compare functions.  

   If they match, then return 0
 */

int ldb_dn_compare(struct ldb_dn *dn0, struct ldb_dn *dn1)
{
	int i, ret;

	if (( ! dn0) || dn0->invalid || ! dn1 || dn1->invalid) return -1;

	if (( ! dn0->valid_case) || ( ! dn1->valid_case)) {
		if (dn0->linearized && dn1->linearized) {
			/* try with a normal compare first, if we are lucky
			 * we will avoid exploding and casfolding */
			if (strcmp(dn0->linearized, dn1->linearized) == 0) return 0;
		}

		if ( ! ldb_dn_casefold_internal(dn0)) {
			return 1;
		}

		if ( ! ldb_dn_casefold_internal(dn1)) {
			return -1;
		}

	}

	if (dn0->comp_num != dn1->comp_num) {
		return (dn1->comp_num - dn0->comp_num);
	}

	if (dn0->comp_num == 0) {
		if (dn0->special && dn1->special) {
			return strcmp(dn0->linearized, dn1->linearized);
		} else if (dn0->special) {
			return 1;
		} else if (dn1->special) {
			return -1;
		} else {
			return 0;
		}
	}

	for (i = 0; i < dn0->comp_num; i++) {
		/* compare attr names */
		ret = strcmp(dn0->components[i].cf_name, dn1->components[i].cf_name);
		if (ret != 0) return ret;

		/* compare attr.cf_value. */ 
		if (dn0->components[i].cf_value.length != dn1->components[i].cf_value.length) {
			return dn0->components[i].cf_value.length - dn1->components[i].cf_value.length;
		}
		ret = strcmp((char *)dn0->components[i].cf_value.data, (char *)dn1->components[i].cf_value.data);
		if (ret != 0) return ret;
	}

	return 0;
}

static struct ldb_dn_component ldb_dn_copy_component(void *mem_ctx, struct ldb_dn_component *src)
{
	struct ldb_dn_component dst;

	memset(&dst, 0, sizeof(dst));

	if (src == NULL) {
		return dst;
	}

	dst.value = ldb_val_dup(mem_ctx, &(src->value));
	if (dst.value.data == NULL) {
		return dst;
	}

	dst.name = talloc_strdup(mem_ctx, src->name);
	if (dst.name == NULL) {
		LDB_FREE(dst.value.data);
		return dst;
	}

	if (src->cf_value.data) {
		dst.cf_value = ldb_val_dup(mem_ctx, &(src->cf_value));
		if (dst.cf_value.data == NULL) {
			LDB_FREE(dst.value.data);
			LDB_FREE(dst.name);
			return dst;
		}

		dst.cf_name = talloc_strdup(mem_ctx, src->cf_name);
		if (dst.cf_name == NULL) {
			LDB_FREE(dst.cf_name);
			LDB_FREE(dst.value.data);
			LDB_FREE(dst.name);
			return dst;
		}
	} else {
		dst.cf_value.data = NULL;
		dst.cf_name = NULL;
	}

	return dst;
}

struct ldb_dn *ldb_dn_copy(void *mem_ctx, struct ldb_dn *dn)
{
	struct ldb_dn *new_dn;

	if (!dn || dn->invalid) {
		return NULL;
	}

	new_dn = talloc_zero(mem_ctx, struct ldb_dn);
	if ( !new_dn) {
		return NULL;
	}

	*new_dn = *dn;

	if (dn->components) {
		int i;

		new_dn->components = talloc_zero_array(new_dn, struct ldb_dn_component, dn->comp_num);
		if ( ! new_dn->components) {
			talloc_free(new_dn);
			return NULL;
		}

		for (i = 0; i < dn->comp_num; i++) {
			new_dn->components[i] = ldb_dn_copy_component(new_dn->components, &dn->components[i]);
			if ( ! new_dn->components[i].value.data) {
				talloc_free(new_dn);
				return NULL;
			}
		}
	}

	if (dn->casefold) {
		new_dn->casefold = talloc_strdup(new_dn, dn->casefold);
		if ( ! new_dn->casefold) {
			talloc_free(new_dn);
			return NULL;
		}
	}

	if (dn->linearized) {
		new_dn->linearized = talloc_strdup(new_dn, dn->linearized);
		if ( ! new_dn->linearized) {
			talloc_free(new_dn);
			return NULL;
		}
	}

	return new_dn;
}

/* modify the given dn by adding a base.
 *
 * return true if successful and false if not
 * if false is returned the dn may be marked invalid
 */
bool ldb_dn_add_base(struct ldb_dn *dn, struct ldb_dn *base)
{
	const char *s;
	char *t;

	if ( !base || base->invalid || !dn || dn->invalid) {
		return false;
	}

	if (dn->components) {
		int i;

		if ( ! ldb_dn_validate(base)) {
			return false;
		}

		s = NULL;
		if (dn->valid_case) {
			if ( ! (s = ldb_dn_get_casefold(base))) {
				return false;
			}
		}

		dn->components = talloc_realloc(dn,
						dn->components,
						struct ldb_dn_component,
						dn->comp_num + base->comp_num);
		if ( ! dn->components) {
			dn->invalid = true;
			return false;
		}

		for (i = 0; i < base->comp_num; dn->comp_num++, i++) {
			dn->components[dn->comp_num] = ldb_dn_copy_component(dn->components, &base->components[i]);
			if (dn->components[dn->comp_num].value.data == NULL) {
				dn->invalid = true;
				return false;
			}
		}

		if (dn->casefold && s) {
			t = talloc_asprintf(dn, "%s,%s", dn->casefold, s);
			LDB_FREE(dn->casefold);
			dn->casefold = t;
		}
	}

	if (dn->linearized) {

		s = ldb_dn_get_linearized(base);
		if ( ! s) {
			return false;
		}
		
		t = talloc_asprintf(dn, "%s,%s", dn->linearized, s);
		if ( ! t) {
			dn->invalid = true;
			return false;
		}
		LDB_FREE(dn->linearized);
		dn->linearized = t;
	}

	return true;
}

/* modify the given dn by adding a base.
 *
 * return true if successful and false if not
 * if false is returned the dn may be marked invalid
 */
bool ldb_dn_add_base_fmt(struct ldb_dn *dn, const char *base_fmt, ...)
{
	struct ldb_dn *base;
	char *base_str;
	va_list ap;
	bool ret;

	if ( !dn || dn->invalid) {
		return false;
	}

	va_start(ap, base_fmt);
	base_str = talloc_vasprintf(dn, base_fmt, ap);
	va_end(ap);

	if (base_str == NULL) {
		return false;
	}

	base = ldb_dn_new(base_str, dn->ldb, base_str);

	ret = ldb_dn_add_base(dn, base);

	talloc_free(base_str);

	return ret;
}	

/* modify the given dn by adding children elements.
 *
 * return true if successful and false if not
 * if false is returned the dn may be marked invalid
 */
bool ldb_dn_add_child(struct ldb_dn *dn, struct ldb_dn *child)
{
	const char *s;
	char *t;

	if ( !child || child->invalid || !dn || dn->invalid) {
		return false;
	}

	if (dn->components) {
		int n, i, j;

		if ( ! ldb_dn_validate(child)) {
			return false;
		}

		s = NULL;
		if (dn->valid_case) {
			if ( ! (s = ldb_dn_get_casefold(child))) {
				return false;
			}
		}

		n = dn->comp_num + child->comp_num;

		dn->components = talloc_realloc(dn,
						dn->components,
						struct ldb_dn_component,
						n);
		if ( ! dn->components) {
			dn->invalid = true;
			return false;
		}

		for (i = dn->comp_num - 1, j = n - 1; i >= 0; i--, j--) {
			dn->components[j] = dn->components[i];
		}

		for (i = 0; i < child->comp_num; i++) {	
			dn->components[i] = ldb_dn_copy_component(dn->components, &child->components[i]);
			if (dn->components[i].value.data == NULL) {
				dn->invalid = true;
				return false;
			}
		}

		dn->comp_num = n;

		if (dn->casefold && s) {
			t = talloc_asprintf(dn, "%s,%s", s, dn->casefold);
			LDB_FREE(dn->casefold);
			dn->casefold = t;
		}
	}

	if (dn->linearized) {

		s = ldb_dn_get_linearized(child);
		if ( ! s) {
			return false;
		}
		
		t = talloc_asprintf(dn, "%s,%s", s, dn->linearized);
		if ( ! t) {
			dn->invalid = true;
			return false;
		}
		LDB_FREE(dn->linearized);
		dn->linearized = t;
	}

	return true;
}

/* modify the given dn by adding children elements.
 *
 * return true if successful and false if not
 * if false is returned the dn may be marked invalid
 */
bool ldb_dn_add_child_fmt(struct ldb_dn *dn, const char *child_fmt, ...)
{
	struct ldb_dn *child;
	char *child_str;
	va_list ap;
	bool ret;

	if ( !dn || dn->invalid) {
		return false;
	}

	va_start(ap, child_fmt);
	child_str = talloc_vasprintf(dn, child_fmt, ap);
	va_end(ap);

	if (child_str == NULL) {
		return false;
	}

	child = ldb_dn_new(child_str, dn->ldb, child_str);

	ret = ldb_dn_add_child(dn, child);

	talloc_free(child_str);

	return ret;
}

bool ldb_dn_remove_base_components(struct ldb_dn *dn, unsigned int num)
{
	int i;

	if ( ! ldb_dn_validate(dn)) {
		return false;
	}

	if (dn->comp_num < num) {
		return false;
	}

	/* free components */
	for (i = num; i > 0; i--) {
		LDB_FREE(dn->components[dn->comp_num - i].name);
		LDB_FREE(dn->components[dn->comp_num - i].value.data);
		LDB_FREE(dn->components[dn->comp_num - i].cf_name);
		LDB_FREE(dn->components[dn->comp_num - i].cf_value.data);
	}
	
	dn->comp_num -= num;

	if (dn->valid_case) {
		for (i = 0; i < dn->comp_num; i++) {
			LDB_FREE(dn->components[i].cf_name);
			LDB_FREE(dn->components[i].cf_value.data);
		}
		dn->valid_case = false;
	}

	LDB_FREE(dn->casefold);
	LDB_FREE(dn->linearized);

	return true;
}

bool ldb_dn_remove_child_components(struct ldb_dn *dn, unsigned int num)
{
	int i, j;

	if ( ! ldb_dn_validate(dn)) {
		return false;
	}

	if (dn->comp_num < num) {
		return false;
	}

	for (i = 0, j = num; j < dn->comp_num; i++, j++) {
		if (i < num) {
			LDB_FREE(dn->components[i].name);
			LDB_FREE(dn->components[i].value.data);
			LDB_FREE(dn->components[i].cf_name);
			LDB_FREE(dn->components[i].cf_value.data);
		}
		dn->components[i] = dn->components[j];
	}

	dn->comp_num -= num;

	if (dn->valid_case) {
		for (i = 0; i < dn->comp_num; i++) {
			LDB_FREE(dn->components[i].cf_name);
			LDB_FREE(dn->components[i].cf_value.data);
		}
		dn->valid_case = false;
	}

	LDB_FREE(dn->casefold);
	LDB_FREE(dn->linearized);

	return true;
}

struct ldb_dn *ldb_dn_get_parent(void *mem_ctx, struct ldb_dn *dn)
{
	struct ldb_dn *new_dn;

	new_dn = ldb_dn_copy(mem_ctx, dn);
	if ( !new_dn ) {
		return NULL;
	}

	if ( ! ldb_dn_remove_child_components(new_dn, 1)) {
		talloc_free(new_dn);
		return NULL;
	}

	return new_dn;
}

/* Create a 'canonical name' string from a DN:

   ie dc=samba,dc=org -> samba.org/
      uid=administrator,ou=users,dc=samba,dc=org = samba.org/users/administrator

   There are two formats, the EX format has the last / replaced with a newline (\n).

*/
static char *ldb_dn_canonical(void *mem_ctx, struct ldb_dn *dn, int ex_format) {
	int i;
	TALLOC_CTX *tmpctx;
	char *cracked = NULL;
 
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}

	tmpctx = talloc_new(mem_ctx);

	/* Walk backwards down the DN, grabbing 'dc' components at first */
	for (i = dn->comp_num - 1 ; i >= 0; i--) {
		if (ldb_attr_cmp(dn->components[i].name, "dc") != 0) {
			break;
		}
		if (cracked) {
			cracked = talloc_asprintf(tmpctx, "%s.%s",
						  ldb_dn_escape_value(tmpctx, dn->components[i].value),
						  cracked);
		} else {
			cracked = ldb_dn_escape_value(tmpctx, dn->components[i].value);
		}
		if (!cracked) {
			goto done;
		}
	}

	/* Only domain components?  Finish here */
	if (i < 0) {
		if (ex_format) {
			cracked = talloc_append_string(tmpctx, cracked, "\n");
		} else {
			cracked = talloc_append_string(tmpctx, cracked, "/");
		}
		talloc_steal(mem_ctx, cracked);
		goto done;
	}

	/* Now walk backwards appending remaining components */
	for (; i > 0; i--) {
		cracked = talloc_asprintf_append(cracked, "/%s", 
					  ldb_dn_escape_value(tmpctx, dn->components[i].value));
		if (!cracked) {
			goto done;
		}
	}

	/* Last one, possibly a newline for the 'ex' format */
	if (ex_format) {
		cracked = talloc_asprintf_append(cracked, "\n%s",
					  ldb_dn_escape_value(tmpctx, dn->components[i].value));
	} else {
		cracked = talloc_asprintf_append(cracked, "/%s", 
					  ldb_dn_escape_value(tmpctx, dn->components[i].value));
	}

	talloc_steal(mem_ctx, cracked);
done:
	talloc_free(tmpctx);
	return cracked;
}

/* Wrapper functions for the above, for the two different string formats */
char *ldb_dn_canonical_string(void *mem_ctx, struct ldb_dn *dn) {
	return ldb_dn_canonical(mem_ctx, dn, 0);

}

char *ldb_dn_canonical_ex_string(void *mem_ctx, struct ldb_dn *dn) {
	return ldb_dn_canonical(mem_ctx, dn, 1);
}

int ldb_dn_get_comp_num(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return -1;
	}
	return dn->comp_num;
}

const char *ldb_dn_get_component_name(struct ldb_dn *dn, unsigned int num)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (num >= dn->comp_num) return NULL;
	return dn->components[num].name;
}

const struct ldb_val *ldb_dn_get_component_val(struct ldb_dn *dn, unsigned int num)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (num >= dn->comp_num) return NULL;
	return &dn->components[num].value;
}

const char *ldb_dn_get_rdn_name(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (dn->comp_num == 0) return NULL;
	return dn->components[0].name;
}

const struct ldb_val *ldb_dn_get_rdn_val(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (dn->comp_num == 0) return NULL;
	return &dn->components[0].value;
}

int ldb_dn_set_component(struct ldb_dn *dn, int num, const char *name, const struct ldb_val val)
{
	char *n;
	struct ldb_val v;

	if ( ! ldb_dn_validate(dn)) {
		return LDB_ERR_OTHER;
	}

	if (num >= dn->comp_num) {
		return LDB_ERR_OTHER;
	}

	n = talloc_strdup(dn, name);
	if ( ! n) {
		return LDB_ERR_OTHER;
	}

	v.length = val.length;
	v.data = (uint8_t *)talloc_memdup(dn, val.data, v.length+1);
	if ( ! v.data) {
		talloc_free(n);
		return LDB_ERR_OTHER;
	}

	talloc_free(dn->components[num].name);
	talloc_free(dn->components[num].value.data);
	dn->components[num].name = n;
	dn->components[num].value = v;

	if (dn->valid_case) {
		int i;
		for (i = 0; i < dn->comp_num; i++) {
			LDB_FREE(dn->components[i].cf_name);
			LDB_FREE(dn->components[i].cf_value.data);
		}
		dn->valid_case = false;
	}
	LDB_FREE(dn->casefold);

	return LDB_SUCCESS;
}

bool ldb_dn_is_valid(struct ldb_dn *dn)
{
	if ( ! dn) return false;
	return ! dn->invalid;
}

bool ldb_dn_is_special(struct ldb_dn *dn)
{
	if ( ! dn || dn->invalid) return false;
	return dn->special;
}

bool ldb_dn_check_special(struct ldb_dn *dn, const char *check)
{
	if ( ! dn || dn->invalid) return false;
	return ! strcmp(dn->linearized, check);
}

bool ldb_dn_is_null(struct ldb_dn *dn)
{
	if ( ! dn || dn->invalid) return false;
	if (dn->linearized && (dn->linearized[0] == '\0')) return true;
	return false;
}
