/*
 * SPDX-FileCopyrightText: Simo Sorce 2004
 * SPDX-FileCopyrightText: Stefan Metzmacher 2004
 * SPDX-FileCopyrightText: Volker Lendecke 2004
 * SPDX-FileCopyrightText: Andrew Tridgell  2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS mplementation.
   LDAP protocol helper functions for SAMBA
   
   Copyright (C) Andrew Tridgell  2004
   Copyright (C) Volker Lendecke 2004
   Copyright (C) Stefan Metzmacher 2004
   Copyright (C) Simo Sorce 2004
    
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

#include "includes.h"
#include "libcli/util/asn_1.h"
#include "libcli/ldap/ldap.h"


static BOOL ldap_push_filter(struct asn1_data *data, struct ldb_parse_tree *tree)
{
	int i;

	switch (tree->operation) {
	case LDB_OP_AND:
	case LDB_OP_OR:
		asn1_push_tag(data, ASN1_CONTEXT(tree->operation==LDB_OP_AND?0:1));
		for (i=0; i<tree->u.list.num_elements; i++) {
			if (!ldap_push_filter(data, tree->u.list.elements[i])) {
				return False;
			}
		}
		asn1_pop_tag(data);
		break;

	case LDB_OP_NOT:
		asn1_push_tag(data, ASN1_CONTEXT(2));
		if (!ldap_push_filter(data, tree->u.isnot.child)) {
			return False;
		}
		asn1_pop_tag(data);
		break;

	case LDB_OP_EQUALITY:
		/* equality test */
		asn1_push_tag(data, ASN1_CONTEXT(3));
		asn1_write_OctetString(data, tree->u.equality.attr,
				      strlen(tree->u.equality.attr));
		asn1_write_OctetString(data, tree->u.equality.value.data,
				      tree->u.equality.value.length);
		asn1_pop_tag(data);
		break;

	case LDB_OP_SUBSTRING:
		/*
		  SubstringFilter ::= SEQUENCE {
			  type            AttributeDescription,
			  -- at least one must be present
			  substrings      SEQUENCE OF CHOICE {
				  initial [0] LDAPString,
				  any     [1] LDAPString,
				  final   [2] LDAPString } }
		*/
		asn1_push_tag(data, ASN1_CONTEXT(4));
		asn1_write_OctetString(data, tree->u.substring.attr, strlen(tree->u.substring.attr));
		asn1_push_tag(data, ASN1_SEQUENCE(0));
		i = 0;
		if ( ! tree->u.substring.start_with_wildcard) {
			asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(0));
			asn1_write_LDAPString(data, (char *)tree->u.substring.chunks[i]->data);
			asn1_pop_tag(data);
			i++;
		}
		while (tree->u.substring.chunks[i]) {
			int ctx;

			if (( ! tree->u.substring.chunks[i + 1]) &&
			    (tree->u.substring.end_with_wildcard == 0)) {
				ctx = 2;
			} else {
				ctx = 1;
			}
			asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(ctx));
			asn1_write_LDAPString(data, (char *)tree->u.substring.chunks[i]->data);
			asn1_pop_tag(data);
			i++;
		}
		asn1_pop_tag(data);
		asn1_pop_tag(data);
		break;

	case LDB_OP_GREATER:
		/* greaterOrEqual test */
		asn1_push_tag(data, ASN1_CONTEXT(5));
		asn1_write_OctetString(data, tree->u.comparison.attr,
				      strlen(tree->u.comparison.attr));
		asn1_write_OctetString(data, tree->u.comparison.value.data,
				      tree->u.comparison.value.length);
		asn1_pop_tag(data);
		break;

	case LDB_OP_LESS:
		/* lessOrEqual test */
		asn1_push_tag(data, ASN1_CONTEXT(6));
		asn1_write_OctetString(data, tree->u.comparison.attr,
				      strlen(tree->u.comparison.attr));
		asn1_write_OctetString(data, tree->u.comparison.value.data,
				      tree->u.comparison.value.length);
		asn1_pop_tag(data);
		break;

	case LDB_OP_PRESENT:
		/* present test */
		asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(7));
		asn1_write_LDAPString(data, tree->u.present.attr);
		asn1_pop_tag(data);
		return !data->has_error;

	case LDB_OP_APPROX:
		/* approx test */
		asn1_push_tag(data, ASN1_CONTEXT(8));
		asn1_write_OctetString(data, tree->u.comparison.attr,
				      strlen(tree->u.comparison.attr));
		asn1_write_OctetString(data, tree->u.comparison.value.data,
				      tree->u.comparison.value.length);
		asn1_pop_tag(data);
		break;

	case LDB_OP_EXTENDED:
		/*
		  MatchingRuleAssertion ::= SEQUENCE {
		  matchingRule    [1] MatchingRuleID OPTIONAL,
		  type            [2] AttributeDescription OPTIONAL,
		  matchValue      [3] AssertionValue,
		  dnAttributes    [4] BOOLEAN DEFAULT FALSE
		  }
		*/
		asn1_push_tag(data, ASN1_CONTEXT(9));
		if (tree->u.extended.rule_id) {
			asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(1));
			asn1_write_LDAPString(data, tree->u.extended.rule_id);
			asn1_pop_tag(data);
		}
		if (tree->u.extended.attr) {
			asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(2));
			asn1_write_LDAPString(data, tree->u.extended.attr);
			asn1_pop_tag(data);
		}
		asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(3));
		asn1_write_LDAPString(data, (char *)tree->u.extended.value.data);
		asn1_pop_tag(data);
		asn1_push_tag(data, ASN1_CONTEXT_SIMPLE(4));
		asn1_write_uint8(data, tree->u.extended.dnAttributes);
		asn1_pop_tag(data);
		asn1_pop_tag(data);
		break;

	default:
		return False;
	}
	return !data->has_error;
}

static void ldap_encode_response(struct asn1_data *data, struct ldap_Result *result)
{
	asn1_write_enumerated(data, result->resultcode);
	asn1_write_OctetString(data, result->dn,
			       (result->dn) ? strlen(result->dn) : 0);
	asn1_write_OctetString(data, result->errormessage,
			       (result->errormessage) ?
			       strlen(result->errormessage) : 0);
	if (result->referral) {
		asn1_push_tag(data, ASN1_CONTEXT(3));
		asn1_write_OctetString(data, result->referral,
				       strlen(result->referral));
		asn1_pop_tag(data);
	}
}

BOOL ldap_encode(struct ldap_message *msg, DATA_BLOB *result, TALLOC_CTX *mem_ctx)
{
	struct asn1_data data;
	int i, j;

	ZERO_STRUCT(data);
	asn1_push_tag(&data, ASN1_SEQUENCE(0));
	asn1_write_Integer(&data, msg->messageid);

	switch (msg->type) {
	case LDAP_TAG_BindRequest: {
		struct ldap_BindRequest *r = &msg->r.BindRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_Integer(&data, r->version);
		asn1_write_OctetString(&data, r->dn,
				       (r->dn != NULL) ? strlen(r->dn) : 0);

		switch (r->mechanism) {
		case LDAP_AUTH_MECH_SIMPLE:
			/* context, primitive */
			asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(0));
			asn1_write(&data, r->creds.password,
				   strlen(r->creds.password));
			asn1_pop_tag(&data);
			break;
		case LDAP_AUTH_MECH_SASL:
			/* context, constructed */
			asn1_push_tag(&data, ASN1_CONTEXT(3));
			asn1_write_OctetString(&data, r->creds.SASL.mechanism,
					       strlen(r->creds.SASL.mechanism));
			if (r->creds.SASL.secblob) {
				asn1_write_OctetString(&data, r->creds.SASL.secblob->data,
						       r->creds.SASL.secblob->length);
			}
			asn1_pop_tag(&data);
			break;
		default:
			return False;
		}

		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_BindResponse: {
		struct ldap_BindResponse *r = &msg->r.BindResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, &r->response);
		if (r->SASL.secblob) {
			asn1_write_ContextSimple(&data, 7, r->SASL.secblob);
		}
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_UnbindRequest: {
/*		struct ldap_UnbindRequest *r = &msg->r.UnbindRequest; */
		break;
	}
	case LDAP_TAG_SearchRequest: {
		struct ldap_SearchRequest *r = &msg->r.SearchRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->basedn, strlen(r->basedn));
		asn1_write_enumerated(&data, r->scope);
		asn1_write_enumerated(&data, r->deref);
		asn1_write_Integer(&data, r->sizelimit);
		asn1_write_Integer(&data, r->timelimit);
		asn1_write_BOOLEAN(&data, r->attributesonly);

		if (!ldap_push_filter(&data, r->tree)) {
			return False;
		}

		asn1_push_tag(&data, ASN1_SEQUENCE(0));
		for (i=0; i<r->num_attributes; i++) {
			asn1_write_OctetString(&data, r->attributes[i],
					       strlen(r->attributes[i]));
		}
		asn1_pop_tag(&data);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_SearchResultEntry: {
		struct ldap_SearchResEntry *r = &msg->r.SearchResultEntry;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->dn, strlen(r->dn));
		asn1_push_tag(&data, ASN1_SEQUENCE(0));
		for (i=0; i<r->num_attributes; i++) {
			struct ldb_message_element *attr = &r->attributes[i];
			asn1_push_tag(&data, ASN1_SEQUENCE(0));
			asn1_write_OctetString(&data, attr->name,
					       strlen(attr->name));
			asn1_push_tag(&data, ASN1_SEQUENCE(1));
			for (j=0; j<attr->num_values; j++) {
				asn1_write_OctetString(&data,
						       attr->values[j].data,
						       attr->values[j].length);
			}
			asn1_pop_tag(&data);
			asn1_pop_tag(&data);
		}
		asn1_pop_tag(&data);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_SearchResultDone: {
		struct ldap_Result *r = &msg->r.SearchResultDone;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ModifyRequest: {
		struct ldap_ModifyRequest *r = &msg->r.ModifyRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->dn, strlen(r->dn));
		asn1_push_tag(&data, ASN1_SEQUENCE(0));

		for (i=0; i<r->num_mods; i++) {
			struct ldb_message_element *attrib = &r->mods[i].attrib;
			asn1_push_tag(&data, ASN1_SEQUENCE(0));
			asn1_write_enumerated(&data, r->mods[i].type);
			asn1_push_tag(&data, ASN1_SEQUENCE(0));
			asn1_write_OctetString(&data, attrib->name,
					       strlen(attrib->name));
			asn1_push_tag(&data, ASN1_SET);
			for (j=0; j<attrib->num_values; j++) {
				asn1_write_OctetString(&data,
						       attrib->values[j].data,
						       attrib->values[j].length);
	
			}
			asn1_pop_tag(&data);
			asn1_pop_tag(&data);
			asn1_pop_tag(&data);
		}
		
		asn1_pop_tag(&data);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ModifyResponse: {
		struct ldap_Result *r = &msg->r.ModifyResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_AddRequest: {
		struct ldap_AddRequest *r = &msg->r.AddRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->dn, strlen(r->dn));
		asn1_push_tag(&data, ASN1_SEQUENCE(0));

		for (i=0; i<r->num_attributes; i++) {
			struct ldb_message_element *attrib = &r->attributes[i];
			asn1_push_tag(&data, ASN1_SEQUENCE(0));
			asn1_write_OctetString(&data, attrib->name,
					       strlen(attrib->name));
			asn1_push_tag(&data, ASN1_SET);
			for (j=0; j<r->attributes[i].num_values; j++) {
				asn1_write_OctetString(&data,
						       attrib->values[j].data,
						       attrib->values[j].length);
			}
			asn1_pop_tag(&data);
			asn1_pop_tag(&data);
		}
		asn1_pop_tag(&data);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_AddResponse: {
		struct ldap_Result *r = &msg->r.AddResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_DelRequest: {
		struct ldap_DelRequest *r = &msg->r.DelRequest;
		asn1_push_tag(&data, ASN1_APPLICATION_SIMPLE(msg->type));
		asn1_write(&data, r->dn, strlen(r->dn));
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_DelResponse: {
		struct ldap_Result *r = &msg->r.DelResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ModifyDNRequest: {
		struct ldap_ModifyDNRequest *r = &msg->r.ModifyDNRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->dn, strlen(r->dn));
		asn1_write_OctetString(&data, r->newrdn, strlen(r->newrdn));
		asn1_write_BOOLEAN(&data, r->deleteolddn);
		if (r->newsuperior) {
			asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(0));
			asn1_write(&data, r->newsuperior,
				   strlen(r->newsuperior));
			asn1_pop_tag(&data);
		}
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ModifyDNResponse: {
		struct ldap_Result *r = &msg->r.ModifyDNResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_CompareRequest: {
		struct ldap_CompareRequest *r = &msg->r.CompareRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->dn, strlen(r->dn));
		asn1_push_tag(&data, ASN1_SEQUENCE(0));
		asn1_write_OctetString(&data, r->attribute,
				       strlen(r->attribute));
		asn1_write_OctetString(&data, r->value.data,
				       r->value.length);
		asn1_pop_tag(&data);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_CompareResponse: {
		struct ldap_Result *r = &msg->r.ModifyDNResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, r);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_AbandonRequest: {
		struct ldap_AbandonRequest *r = &msg->r.AbandonRequest;
		asn1_push_tag(&data, ASN1_APPLICATION_SIMPLE(msg->type));
		asn1_write_implicit_Integer(&data, r->messageid);
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_SearchResultReference: {
		struct ldap_SearchResRef *r = &msg->r.SearchResultReference;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_write_OctetString(&data, r->referral, strlen(r->referral));
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ExtendedRequest: {
		struct ldap_ExtendedRequest *r = &msg->r.ExtendedRequest;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(0));
		asn1_write(&data, r->oid, strlen(r->oid));
		asn1_pop_tag(&data);
		if (r->value) {
			asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(1));
			asn1_write(&data, r->value->data, r->value->length);
			asn1_pop_tag(&data);
		}
		asn1_pop_tag(&data);
		break;
	}
	case LDAP_TAG_ExtendedResponse: {
		struct ldap_ExtendedResponse *r = &msg->r.ExtendedResponse;
		asn1_push_tag(&data, ASN1_APPLICATION(msg->type));
		ldap_encode_response(&data, &r->response);
		if (r->oid) {
			asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(10));
			asn1_write(&data, r->oid, strlen(r->oid));
			asn1_pop_tag(&data);
		}
		if (r->value) {
			asn1_push_tag(&data, ASN1_CONTEXT_SIMPLE(11));
			asn1_write(&data, r->value->data, r->value->length);
			asn1_pop_tag(&data);
		}
		asn1_pop_tag(&data);
		break;
	}
	default:
		return False;
	}

	if (msg->controls != NULL) {
		asn1_push_tag(&data, ASN1_CONTEXT(0));
		
		for (i = 0; msg->controls[i] != NULL; i++) {
			if (!ldap_encode_control(mem_ctx, &data, msg->controls[i])) {
				return False;
			}
		}

		asn1_pop_tag(&data);
	}

	asn1_pop_tag(&data);

	if (data.has_error) {
		asn1_free(&data);
		return False;
	}

	*result = data_blob_talloc(mem_ctx, data.data, data.length);
	asn1_free(&data);
	return True;
}

static const char *blob2string_talloc(TALLOC_CTX *mem_ctx,
				      DATA_BLOB blob)
{
	char *result = talloc_size(mem_ctx, blob.length+1);
	memcpy(result, blob.data, blob.length);
	result[blob.length] = '\0';
	return result;
}

static BOOL asn1_read_OctetString_talloc(TALLOC_CTX *mem_ctx,
					 struct asn1_data *data,
					 const char **result)
{
	DATA_BLOB string;
	if (!asn1_read_OctetString(data, &string))
		return False;
	*result = blob2string_talloc(mem_ctx, string);
	data_blob_free(&string);
	return True;
}

static void ldap_decode_response(TALLOC_CTX *mem_ctx,
				 struct asn1_data *data,
				 struct ldap_Result *result)
{
	asn1_read_enumerated(data, &result->resultcode);
	asn1_read_OctetString_talloc(mem_ctx, data, &result->dn);
	asn1_read_OctetString_talloc(mem_ctx, data, &result->errormessage);
	if (asn1_peek_tag(data, ASN1_CONTEXT(3))) {
		asn1_start_tag(data, ASN1_CONTEXT(3));
		asn1_read_OctetString_talloc(mem_ctx, data, &result->referral);
		asn1_end_tag(data);
	} else {
		result->referral = NULL;
	}
}

static struct ldb_val **ldap_decode_substring(TALLOC_CTX *mem_ctx, struct ldb_val **chunks, int chunk_num, char *value)
{

	chunks = talloc_realloc(mem_ctx, chunks, struct ldb_val *, chunk_num + 2);
	if (chunks == NULL) {
		return NULL;
	}

	chunks[chunk_num] = talloc(mem_ctx, struct ldb_val);
	if (chunks[chunk_num] == NULL) {
		return NULL;
	}

	chunks[chunk_num]->data = (uint8_t *)talloc_strdup(mem_ctx, value);
	if (chunks[chunk_num]->data == NULL) {
		return NULL;
	}
	chunks[chunk_num]->length = strlen(value);

	chunks[chunk_num + 1] = '\0';

	return chunks;
}


/*
  parse the ASN.1 formatted search string into a ldb_parse_tree
*/
static struct ldb_parse_tree *ldap_decode_filter_tree(TALLOC_CTX *mem_ctx, 
						      struct asn1_data *data)
{
	uint8_t filter_tag;
	struct ldb_parse_tree *ret;

	if (!asn1_peek_uint8(data, &filter_tag)) {
		return NULL;
	}

	filter_tag &= 0x1f;	/* strip off the asn1 stuff */

	ret = talloc(mem_ctx, struct ldb_parse_tree);
	if (ret == NULL) return NULL;

	switch(filter_tag) {
	case 0:
	case 1:
		/* AND or OR of one or more filters */
		ret->operation = (filter_tag == 0)?LDB_OP_AND:LDB_OP_OR;
		ret->u.list.num_elements = 0;
		ret->u.list.elements = NULL;

		if (!asn1_start_tag(data, ASN1_CONTEXT(filter_tag))) {
			goto failed;
		}

		while (asn1_tag_remaining(data) > 0) {
			struct ldb_parse_tree *subtree;
			subtree = ldap_decode_filter_tree(ret, data);
			if (subtree == NULL) {
				goto failed;
			}
			ret->u.list.elements = 
				talloc_realloc(ret, ret->u.list.elements, 
					       struct ldb_parse_tree *, 
					       ret->u.list.num_elements+1);
			if (ret->u.list.elements == NULL) {
				goto failed;
			}
			talloc_steal(ret->u.list.elements, subtree);
			ret->u.list.elements[ret->u.list.num_elements] = subtree;
			ret->u.list.num_elements++;
		}
		if (!asn1_end_tag(data)) {
			goto failed;
		}
		break;

	case 2:
		/* 'not' operation */
		if (!asn1_start_tag(data, ASN1_CONTEXT(filter_tag))) {
			goto failed;
		}

		ret->operation = LDB_OP_NOT;
		ret->u.isnot.child = ldap_decode_filter_tree(ret, data);
		if (ret->u.isnot.child == NULL) {
			goto failed;
		}
		if (!asn1_end_tag(data)) {
			goto failed;
		}
		break;

	case 3: {
		/* equalityMatch */
		const char *attrib;
		DATA_BLOB value;

		asn1_start_tag(data, ASN1_CONTEXT(filter_tag));
		asn1_read_OctetString_talloc(mem_ctx, data, &attrib);
		asn1_read_OctetString(data, &value);
		asn1_end_tag(data);
		if ((data->has_error) || (attrib == NULL) || (value.data == NULL)) {
			goto failed;
		}

		ret->operation = LDB_OP_EQUALITY;
		ret->u.equality.attr = talloc_steal(ret, attrib);
		ret->u.equality.value.data = talloc_steal(ret, value.data);
		ret->u.equality.value.length = value.length;
		break;
	}
	case 4: {
		/* substrings */
		DATA_BLOB attr;
		uint8_t subs_tag;
		char *value;
		int chunk_num = 0;

		if (!asn1_start_tag(data, ASN1_CONTEXT(filter_tag))) {
			goto failed;
		}
		if (!asn1_read_OctetString(data, &attr)) {
			goto failed;
		}

		ret->operation = LDB_OP_SUBSTRING;
		ret->u.substring.attr = talloc_strndup(ret, (char *)attr.data, attr.length);
		ret->u.substring.chunks = NULL;
		ret->u.substring.start_with_wildcard = 1;
		ret->u.substring.end_with_wildcard = 1;

		if (!asn1_start_tag(data, ASN1_SEQUENCE(0))) {
			goto failed;
		}

		while (asn1_tag_remaining(data)) {
			asn1_peek_uint8(data, &subs_tag);
			subs_tag &= 0x1f;	/* strip off the asn1 stuff */
			if (subs_tag > 2) goto failed;

			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(subs_tag));
			asn1_read_LDAPString(data, &value);
			asn1_end_tag(data);

			switch (subs_tag) {
			case 0:
				if (ret->u.substring.chunks != NULL) {
					/* initial value found in the middle */
					goto failed;
				}

				ret->u.substring.chunks = ldap_decode_substring(ret, NULL, 0, value);
				if (ret->u.substring.chunks == NULL) {
					goto failed;
				}

				ret->u.substring.start_with_wildcard = 0;
				chunk_num = 1;
				break;

			case 1:
				if (ret->u.substring.end_with_wildcard == 0) {
					/* "any" value found after a "final" value */
					goto failed;
				}

				ret->u.substring.chunks = ldap_decode_substring(ret,
										ret->u.substring.chunks,
										chunk_num,
										value);
				if (ret->u.substring.chunks == NULL) {
					goto failed;
				}

				chunk_num++;
				break;

			case 2:
				ret->u.substring.chunks = ldap_decode_substring(ret,
										ret->u.substring.chunks,
										chunk_num,
										value);
				if (ret->u.substring.chunks == NULL) {
					goto failed;
				}

				ret->u.substring.end_with_wildcard = 0;
				break;

			default:
				goto failed;
			}

		}

		if (!asn1_end_tag(data)) { /* SEQUENCE */
			goto failed;
		}

		if (!asn1_end_tag(data)) {
			goto failed;
		}
		break;
	}
	case 5: {
		/* greaterOrEqual */
		const char *attrib;
		DATA_BLOB value;

		asn1_start_tag(data, ASN1_CONTEXT(filter_tag));
		asn1_read_OctetString_talloc(mem_ctx, data, &attrib);
		asn1_read_OctetString(data, &value);
		asn1_end_tag(data);
		if ((data->has_error) || (attrib == NULL) || (value.data == NULL)) {
			goto failed;
		}

		ret->operation = LDB_OP_GREATER;
		ret->u.comparison.attr = talloc_steal(ret, attrib);
		ret->u.comparison.value.data = talloc_steal(ret, value.data);
		ret->u.comparison.value.length = value.length;
		break;
	}
	case 6: {
		/* lessOrEqual */
		const char *attrib;
		DATA_BLOB value;

		asn1_start_tag(data, ASN1_CONTEXT(filter_tag));
		asn1_read_OctetString_talloc(mem_ctx, data, &attrib);
		asn1_read_OctetString(data, &value);
		asn1_end_tag(data);
		if ((data->has_error) || (attrib == NULL) || (value.data == NULL)) {
			goto failed;
		}

		ret->operation = LDB_OP_LESS;
		ret->u.comparison.attr = talloc_steal(ret, attrib);
		ret->u.comparison.value.data = talloc_steal(ret, value.data);
		ret->u.comparison.value.length = value.length;
		break;
	}
	case 7: {
		/* Normal presence, "attribute=*" */
		char *attr;

		if (!asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(filter_tag))) {
			goto failed;
		}
		if (!asn1_read_LDAPString(data, &attr)) {
			goto failed;
		}

		ret->operation = LDB_OP_PRESENT;
		ret->u.present.attr = talloc_steal(ret, attr);

		if (!asn1_end_tag(data)) {
			goto failed;
		}
		break;
	}
	case 8: {
		/* approx */
		const char *attrib;
		DATA_BLOB value;

		asn1_start_tag(data, ASN1_CONTEXT(filter_tag));
		asn1_read_OctetString_talloc(mem_ctx, data, &attrib);
		asn1_read_OctetString(data, &value);
		asn1_end_tag(data);
		if ((data->has_error) || (attrib == NULL) || (value.data == NULL)) {
			goto failed;
		}

		ret->operation = LDB_OP_APPROX;
		ret->u.comparison.attr = talloc_steal(ret, attrib);
		ret->u.comparison.value.data = talloc_steal(ret, value.data);
		ret->u.comparison.value.length = value.length;
		break;
	}
	case 9: {
		char *oid = NULL, *attr = NULL, *value;
		uint8_t dnAttributes;
		/* an extended search */
		if (!asn1_start_tag(data, ASN1_CONTEXT(filter_tag))) {
			goto failed;
		}

		/* FIXME: read carefully rfc2251.txt there are a number of 'MUST's
		   we need to check we properly implement --SSS */ 
		/* either oid or type must be defined */
		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(1))) { /* optional */
			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(1));
			asn1_read_LDAPString(data, &oid);
			asn1_end_tag(data);
		}
		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(2))) {	/* optional  */
			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(2));
			asn1_read_LDAPString(data, &attr);
			asn1_end_tag(data);
		}
		asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(3));
		asn1_read_LDAPString(data, &value);
		asn1_end_tag(data);
		/* dnAttributes is marked as BOOLEAN DEFAULT FALSE
		   it is not marked as OPTIONAL but openldap tools
		   do not set this unless it is to be set as TRUE
		   NOTE: openldap tools do not work with AD as it
		   seems that AD always requires the dnAttributes
		   boolean value to be set */
		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(4))) {
			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(4));
			asn1_read_uint8(data, &dnAttributes);
			asn1_end_tag(data);
		} else {
			dnAttributes = 0;
		}
		if ((oid == NULL && attr == NULL) || (value == NULL)) {
			goto failed;
		}

		if (oid) {
			ret->operation               = LDB_OP_EXTENDED;

			/* From the RFC2251: If the type field is
			   absent and matchingRule is present, the matchValue is compared
			   against all attributes in an entry which support that matchingRule
			*/
			if (attr) {
				ret->u.extended.attr = talloc_steal(ret, attr);
			} else {
				ret->u.extended.attr = talloc_strdup(ret, "*");
			}
			ret->u.extended.rule_id      = talloc_steal(ret, oid);
			ret->u.extended.value.data   = talloc_steal(ret, value);
			ret->u.extended.value.length = strlen(value);
			ret->u.extended.dnAttributes = dnAttributes;
		} else {
			ret->operation               = LDB_OP_EQUALITY;
			ret->u.equality.attr         = talloc_steal(ret, attr);
			ret->u.equality.value.data   = talloc_steal(ret, value);
			ret->u.equality.value.length = strlen(value);
		}
		if (!asn1_end_tag(data)) {
			goto failed;
		}
		break;
	}

	default:
		DEBUG(0,("Unsupported LDAP filter operation 0x%x\n", filter_tag));
		goto failed;
	}
	
	return ret;

failed:
	talloc_free(ret);
	return NULL;	
}


static void ldap_decode_attrib(TALLOC_CTX *mem_ctx, struct asn1_data *data,
			       struct ldb_message_element *attrib)
{
	asn1_start_tag(data, ASN1_SEQUENCE(0));
	asn1_read_OctetString_talloc(mem_ctx, data, &attrib->name);
	asn1_start_tag(data, ASN1_SET);
	while (asn1_peek_tag(data, ASN1_OCTET_STRING)) {
		DATA_BLOB blob;
		asn1_read_OctetString(data, &blob);
		add_value_to_attrib(mem_ctx, &blob, attrib);
	}
	asn1_end_tag(data);
	asn1_end_tag(data);
	
}

static void ldap_decode_attribs(TALLOC_CTX *mem_ctx, struct asn1_data *data,
				struct ldb_message_element **attributes,
				int *num_attributes)
{
	asn1_start_tag(data, ASN1_SEQUENCE(0));
	while (asn1_peek_tag(data, ASN1_SEQUENCE(0))) {
		struct ldb_message_element attrib;
		ZERO_STRUCT(attrib);
		ldap_decode_attrib(mem_ctx, data, &attrib);
		add_attrib_to_array_talloc(mem_ctx, &attrib,
					   attributes, num_attributes);
	}
	asn1_end_tag(data);
}

BOOL ldap_decode(struct asn1_data *data, struct ldap_message *msg)
{
	uint8_t tag;

	asn1_start_tag(data, ASN1_SEQUENCE(0));
	asn1_read_Integer(data, &msg->messageid);

	if (!asn1_peek_uint8(data, &tag))
		return False;

	switch(tag) {

	case ASN1_APPLICATION(LDAP_TAG_BindRequest): {
		struct ldap_BindRequest *r = &msg->r.BindRequest;
		msg->type = LDAP_TAG_BindRequest;
		asn1_start_tag(data, tag);
		asn1_read_Integer(data, &r->version);
		asn1_read_OctetString_talloc(msg, data, &r->dn);
		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(0))) {
			int pwlen;
			r->creds.password = "";
			r->mechanism = LDAP_AUTH_MECH_SIMPLE;
			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(0));
			pwlen = asn1_tag_remaining(data);
			if (pwlen == -1) {
				return False;
			}
			if (pwlen != 0) {
				char *pw = talloc_size(msg, pwlen+1);
				if (!pw) {
					return False;
				}
				asn1_read(data, pw, pwlen);
				pw[pwlen] = '\0';
				r->creds.password = pw;
			}
			asn1_end_tag(data);
		} else if (asn1_peek_tag(data, ASN1_CONTEXT(3))){
			asn1_start_tag(data, ASN1_CONTEXT(3));
			r->mechanism = LDAP_AUTH_MECH_SASL;
			asn1_read_OctetString_talloc(msg, data, &r->creds.SASL.mechanism);
			if (asn1_peek_tag(data, ASN1_OCTET_STRING)) { /* optional */
				DATA_BLOB tmp_blob = data_blob(NULL, 0);
				asn1_read_OctetString(data, &tmp_blob);
				r->creds.SASL.secblob = talloc(msg, DATA_BLOB);
				if (!r->creds.SASL.secblob) {
					return False;
				}
				*r->creds.SASL.secblob = data_blob_talloc(r->creds.SASL.secblob,
									  tmp_blob.data, tmp_blob.length);
				data_blob_free(&tmp_blob);
			} else {
				r->creds.SASL.secblob = NULL;
			}
			asn1_end_tag(data);
		} else {
			/* Neither Simple nor SASL bind */
			return False;
		}
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_BindResponse): {
		struct ldap_BindResponse *r = &msg->r.BindResponse;
		msg->type = LDAP_TAG_BindResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, &r->response);
		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(7))) {
			DATA_BLOB tmp_blob = data_blob(NULL, 0);
			asn1_read_ContextSimple(data, 7, &tmp_blob);
			r->SASL.secblob = talloc(msg, DATA_BLOB);
			if (!r->SASL.secblob) {
				return False;
			}
			*r->SASL.secblob = data_blob_talloc(r->SASL.secblob,
							    tmp_blob.data, tmp_blob.length);
			data_blob_free(&tmp_blob);
		} else {
			r->SASL.secblob = NULL;
		}
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION_SIMPLE(LDAP_TAG_UnbindRequest): {
		msg->type = LDAP_TAG_UnbindRequest;
		asn1_start_tag(data, tag);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_SearchRequest): {
		struct ldap_SearchRequest *r = &msg->r.SearchRequest;
		msg->type = LDAP_TAG_SearchRequest;
		asn1_start_tag(data, tag);
		asn1_read_OctetString_talloc(msg, data, &r->basedn);
		asn1_read_enumerated(data, (int *)&(r->scope));
		asn1_read_enumerated(data, (int *)&(r->deref));
		asn1_read_Integer(data, &r->sizelimit);
		asn1_read_Integer(data, &r->timelimit);
		asn1_read_BOOLEAN(data, &r->attributesonly);

		r->tree = ldap_decode_filter_tree(msg, data);
		if (r->tree == NULL) {
			return False;
		}

		asn1_start_tag(data, ASN1_SEQUENCE(0));

		r->num_attributes = 0;
		r->attributes = NULL;

		while (asn1_tag_remaining(data) > 0) {
			const char *attr;
			if (!asn1_read_OctetString_talloc(msg, data,
							  &attr))
				return False;
			if (!add_string_to_array(msg, attr,
						 &r->attributes,
						 &r->num_attributes))
				return False;
		}

		asn1_end_tag(data);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_SearchResultEntry): {
		struct ldap_SearchResEntry *r = &msg->r.SearchResultEntry;
		msg->type = LDAP_TAG_SearchResultEntry;
		r->attributes = NULL;
		r->num_attributes = 0;
		asn1_start_tag(data, tag);
		asn1_read_OctetString_talloc(msg, data, &r->dn);
		ldap_decode_attribs(msg, data, &r->attributes,
				    &r->num_attributes);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_SearchResultDone): {
		struct ldap_Result *r = &msg->r.SearchResultDone;
		msg->type = LDAP_TAG_SearchResultDone;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_SearchResultReference): {
		struct ldap_SearchResRef *r = &msg->r.SearchResultReference;
		msg->type = LDAP_TAG_SearchResultReference;
		asn1_start_tag(data, tag);
		asn1_read_OctetString_talloc(msg, data, &r->referral);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ModifyRequest): {
		struct ldap_ModifyRequest *r = &msg->r.ModifyRequest;
		msg->type = LDAP_TAG_ModifyRequest;
		asn1_start_tag(data, ASN1_APPLICATION(LDAP_TAG_ModifyRequest));
		asn1_read_OctetString_talloc(msg, data, &r->dn);
		asn1_start_tag(data, ASN1_SEQUENCE(0));

		r->num_mods = 0;
		r->mods = NULL;

		while (asn1_tag_remaining(data) > 0) {
			struct ldap_mod mod;
			int v;
			ZERO_STRUCT(mod);
			asn1_start_tag(data, ASN1_SEQUENCE(0));
			asn1_read_enumerated(data, &v);
			mod.type = v;
			ldap_decode_attrib(msg, data, &mod.attrib);
			asn1_end_tag(data);
			if (!add_mod_to_array_talloc(msg, &mod,
						     &r->mods, &r->num_mods)) {
				return False;
			}
		}

		asn1_end_tag(data);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ModifyResponse): {
		struct ldap_Result *r = &msg->r.ModifyResponse;
		msg->type = LDAP_TAG_ModifyResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_AddRequest): {
		struct ldap_AddRequest *r = &msg->r.AddRequest;
		msg->type = LDAP_TAG_AddRequest;
		asn1_start_tag(data, tag);
		asn1_read_OctetString_talloc(msg, data, &r->dn);

		r->attributes = NULL;
		r->num_attributes = 0;
		ldap_decode_attribs(msg, data, &r->attributes,
				    &r->num_attributes);

		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_AddResponse): {
		struct ldap_Result *r = &msg->r.AddResponse;
		msg->type = LDAP_TAG_AddResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION_SIMPLE(LDAP_TAG_DelRequest): {
		struct ldap_DelRequest *r = &msg->r.DelRequest;
		int len;
		char *dn;
		msg->type = LDAP_TAG_DelRequest;
		asn1_start_tag(data,
			       ASN1_APPLICATION_SIMPLE(LDAP_TAG_DelRequest));
		len = asn1_tag_remaining(data);
		if (len == -1) {
			return False;
		}
		dn = talloc_size(msg, len+1);
		if (dn == NULL)
			break;
		asn1_read(data, dn, len);
		dn[len] = '\0';
		r->dn = dn;
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_DelResponse): {
		struct ldap_Result *r = &msg->r.DelResponse;
		msg->type = LDAP_TAG_DelResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ModifyDNRequest): {
		struct ldap_ModifyDNRequest *r = &msg->r.ModifyDNRequest;
		msg->type = LDAP_TAG_ModifyDNRequest;
		asn1_start_tag(data,
			       ASN1_APPLICATION(LDAP_TAG_ModifyDNRequest));
		asn1_read_OctetString_talloc(msg, data, &r->dn);
		asn1_read_OctetString_talloc(msg, data, &r->newrdn);
		asn1_read_BOOLEAN(data, &r->deleteolddn);
		r->newsuperior = NULL;
		if (asn1_tag_remaining(data) > 0) {
			int len;
			char *newsup;
			asn1_start_tag(data, ASN1_CONTEXT_SIMPLE(0));
			len = asn1_tag_remaining(data);
			if (len == -1) {
				return False;
			}
			newsup = talloc_size(msg, len+1);
			if (newsup == NULL) {
				return False;
			}
			asn1_read(data, newsup, len);
			newsup[len] = '\0';
			r->newsuperior = newsup;
			asn1_end_tag(data);
		}
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ModifyDNResponse): {
		struct ldap_Result *r = &msg->r.ModifyDNResponse;
		msg->type = LDAP_TAG_ModifyDNResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_CompareRequest): {
		struct ldap_CompareRequest *r = &msg->r.CompareRequest;
		msg->type = LDAP_TAG_CompareRequest;
		asn1_start_tag(data,
			       ASN1_APPLICATION(LDAP_TAG_CompareRequest));
		asn1_read_OctetString_talloc(msg, data, &r->dn);
		asn1_start_tag(data, ASN1_SEQUENCE(0));
		asn1_read_OctetString_talloc(msg, data, &r->attribute);
		asn1_read_OctetString(data, &r->value);
		if (r->value.data) {
			talloc_steal(msg, r->value.data);
		}
		asn1_end_tag(data);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_CompareResponse): {
		struct ldap_Result *r = &msg->r.CompareResponse;
		msg->type = LDAP_TAG_CompareResponse;
		asn1_start_tag(data, tag);
		ldap_decode_response(msg, data, r);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION_SIMPLE(LDAP_TAG_AbandonRequest): {
		struct ldap_AbandonRequest *r = &msg->r.AbandonRequest;
		msg->type = LDAP_TAG_AbandonRequest;
		asn1_start_tag(data, tag);
		asn1_read_implicit_Integer(data, &r->messageid);
		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ExtendedRequest): {
		struct ldap_ExtendedRequest *r = &msg->r.ExtendedRequest;
		DATA_BLOB tmp_blob = data_blob(NULL, 0);

		msg->type = LDAP_TAG_ExtendedRequest;
		asn1_start_tag(data,tag);
		if (!asn1_read_ContextSimple(data, 0, &tmp_blob)) {
			return False;
		}
		r->oid = blob2string_talloc(msg, tmp_blob);
		data_blob_free(&tmp_blob);
		if (!r->oid) {
			return False;
		}

		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(1))) {
			asn1_read_ContextSimple(data, 1, &tmp_blob);
			r->value = talloc(msg, DATA_BLOB);
			if (!r->value) {
				return False;
			}
			*r->value = data_blob_talloc(r->value, tmp_blob.data, tmp_blob.length);
			data_blob_free(&tmp_blob);
		} else {
			r->value = NULL;
		}

		asn1_end_tag(data);
		break;
	}

	case ASN1_APPLICATION(LDAP_TAG_ExtendedResponse): {
		struct ldap_ExtendedResponse *r = &msg->r.ExtendedResponse;
		DATA_BLOB tmp_blob = data_blob(NULL, 0);

		msg->type = LDAP_TAG_ExtendedResponse;
		asn1_start_tag(data, tag);		
		ldap_decode_response(msg, data, &r->response);

		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(10))) {
			asn1_read_ContextSimple(data, 1, &tmp_blob);
			r->oid = blob2string_talloc(msg, tmp_blob);
			data_blob_free(&tmp_blob);
			if (!r->oid) {
				return False;
			}
		} else {
			r->oid = NULL;
		}

		if (asn1_peek_tag(data, ASN1_CONTEXT_SIMPLE(11))) {
			asn1_read_ContextSimple(data, 1, &tmp_blob);
			r->value = talloc(msg, DATA_BLOB);
			if (!r->value) {
				return False;
			}
			*r->value = data_blob_talloc(r->value, tmp_blob.data, tmp_blob.length);
			data_blob_free(&tmp_blob);
		} else {
			r->value = NULL;
		}

		asn1_end_tag(data);
		break;
	}
	default: 
		return False;
	}

	msg->controls = NULL;

	if (asn1_peek_tag(data, ASN1_CONTEXT(0))) {
		int i;
		struct ldb_control **ctrl = NULL;

		asn1_start_tag(data, ASN1_CONTEXT(0));

		for (i=0; asn1_peek_tag(data, ASN1_SEQUENCE(0)); i++) {
			/* asn1_start_tag(data, ASN1_SEQUENCE(0)); */

			ctrl = talloc_realloc(msg, ctrl, struct ldb_control *, i+2);
			if (!ctrl) {
				return False;
			}

			ctrl[i] = talloc(ctrl, struct ldb_control);
			if (!ctrl[i]) {
				return False;
			}

			if (!ldap_decode_control(ctrl, data, ctrl[i])) {
				return False;
			}

		}

		if (ctrl != NULL) {
			ctrl[i] = NULL;
		}

		msg->controls = ctrl;

		asn1_end_tag(data);
	}

	asn1_end_tag(data);
	return ((!data->has_error) && (data->nesting == NULL));
}


/*
  return NT_STATUS_OK if a blob has enough bytes in it to be a full
  ldap packet. Set packet_size if true.
*/
NTSTATUS ldap_full_packet(void *private_data, DATA_BLOB blob, size_t *packet_size)
{
	return asn1_full_tag(blob, ASN1_SEQUENCE(0), packet_size);
}
