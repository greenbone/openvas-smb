/* Generated from hdb.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <hdb_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_HDB_extension(unsigned char *p, size_t len, const HDB_extension *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* data */
{
size_t Top_tag_oldret = ret;
ret = 0;

switch((&(data)->data)->element) {
case choice_HDB_extension_data_last_pw_change: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &((&(data)->data))->u.last_pw_change, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_aliases: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_Aliases(p, len, &((&(data)->data))->u.aliases, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_password: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_Password(p, len, &((&(data)->data))->u.password, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_lm_owf: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_Lan_Manager_OWF(p, len, &((&(data)->data))->u.lm_owf, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_allowed_to_delegate_to: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_Constrained_delegation_acl(p, len, &((&(data)->data))->u.allowed_to_delegate_to, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_pkinit_cert_hash: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_PKINIT_hash(p, len, &((&(data)->data))->u.pkinit_cert_hash, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_pkinit_acl: {size_t data_tag_oldret = ret;
ret = 0;
e = encode_HDB_Ext_PKINIT_acl(p, len, &((&(data)->data))->u.pkinit_acl, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += data_tag_oldret;
break;
}
case choice_HDB_extension_data_asn1_ellipsis: {
if (len < (&(data)->data)->u.asn1_ellipsis.length)
return ASN1_OVERFLOW;
p -= (&(data)->data)->u.asn1_ellipsis.length;
ret += (&(data)->data)->u.asn1_ellipsis.length;
memcpy(p + 1, (&(data)->data)->u.asn1_ellipsis.data, (&(data)->data)->u.asn1_ellipsis.length);
break;
}
};
e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* mandatory */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_boolean(p, len, &(data)->mandatory, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Boolean, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_HDB_extension(const unsigned char *p, size_t len, HDB_extension *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
{
size_t mandatory_datalen, mandatory_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &mandatory_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
mandatory_oldlen = len;
if (mandatory_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mandatory_datalen;
{
size_t mandatory_Tag_datalen, mandatory_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Boolean, &mandatory_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
mandatory_Tag_oldlen = len;
if (mandatory_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mandatory_Tag_datalen;
e = der_get_boolean(p, len, &(data)->mandatory, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = mandatory_Tag_oldlen - mandatory_Tag_datalen;
}
len = mandatory_oldlen - mandatory_datalen;
}
{
size_t data_datalen, data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &data_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
data_oldlen = len;
if (data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = data_datalen;
if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 0, NULL) == 0) {
{
size_t pkinit_acl_datalen, pkinit_acl_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &pkinit_acl_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
pkinit_acl_oldlen = len;
if (pkinit_acl_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pkinit_acl_datalen;
e = decode_HDB_Ext_PKINIT_acl(p, len, &(&(data)->data)->u.pkinit_acl, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pkinit_acl_oldlen - pkinit_acl_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_pkinit_acl;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 1, NULL) == 0) {
{
size_t pkinit_cert_hash_datalen, pkinit_cert_hash_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &pkinit_cert_hash_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
pkinit_cert_hash_oldlen = len;
if (pkinit_cert_hash_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pkinit_cert_hash_datalen;
e = decode_HDB_Ext_PKINIT_hash(p, len, &(&(data)->data)->u.pkinit_cert_hash, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pkinit_cert_hash_oldlen - pkinit_cert_hash_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_pkinit_cert_hash;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 2, NULL) == 0) {
{
size_t allowed_to_delegate_to_datalen, allowed_to_delegate_to_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &allowed_to_delegate_to_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
allowed_to_delegate_to_oldlen = len;
if (allowed_to_delegate_to_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = allowed_to_delegate_to_datalen;
e = decode_HDB_Ext_Constrained_delegation_acl(p, len, &(&(data)->data)->u.allowed_to_delegate_to, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = allowed_to_delegate_to_oldlen - allowed_to_delegate_to_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_allowed_to_delegate_to;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 4, NULL) == 0) {
{
size_t lm_owf_datalen, lm_owf_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &lm_owf_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
lm_owf_oldlen = len;
if (lm_owf_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = lm_owf_datalen;
e = decode_HDB_Ext_Lan_Manager_OWF(p, len, &(&(data)->data)->u.lm_owf, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = lm_owf_oldlen - lm_owf_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_lm_owf;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 5, NULL) == 0) {
{
size_t password_datalen, password_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &password_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
password_oldlen = len;
if (password_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = password_datalen;
e = decode_HDB_Ext_Password(p, len, &(&(data)->data)->u.password, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = password_oldlen - password_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_password;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 6, NULL) == 0) {
{
size_t aliases_datalen, aliases_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &aliases_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
aliases_oldlen = len;
if (aliases_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = aliases_datalen;
e = decode_HDB_Ext_Aliases(p, len, &(&(data)->data)->u.aliases, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = aliases_oldlen - aliases_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_aliases;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 7, NULL) == 0) {
{
size_t last_pw_change_datalen, last_pw_change_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &last_pw_change_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
last_pw_change_oldlen = len;
if (last_pw_change_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = last_pw_change_datalen;
e = decode_KerberosTime(p, len, &(&(data)->data)->u.last_pw_change, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = last_pw_change_oldlen - last_pw_change_datalen;
}
(&(data)->data)->element = choice_HDB_extension_data_last_pw_change;
}
else {
(&(data)->data)->u.asn1_ellipsis.data = calloc(1, len);
if ((&(data)->data)->u.asn1_ellipsis.data == NULL) {
e = ENOMEM; goto fail;
}
(&(data)->data)->u.asn1_ellipsis.length = len;
memcpy((&(data)->data)->u.asn1_ellipsis.data, p, len);
(&(data)->data)->element = choice_HDB_extension_data_asn1_ellipsis;
p += len;
ret += len;
len -= len;
}
len = data_oldlen - data_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_HDB_extension(data);
return e;
}

void
free_HDB_extension(HDB_extension *data)
{
switch((&(data)->data)->element) {
case choice_HDB_extension_data_pkinit_acl:
free_HDB_Ext_PKINIT_acl(&(&(data)->data)->u.pkinit_acl);
break;
case choice_HDB_extension_data_pkinit_cert_hash:
free_HDB_Ext_PKINIT_hash(&(&(data)->data)->u.pkinit_cert_hash);
break;
case choice_HDB_extension_data_allowed_to_delegate_to:
free_HDB_Ext_Constrained_delegation_acl(&(&(data)->data)->u.allowed_to_delegate_to);
break;
case choice_HDB_extension_data_lm_owf:
free_HDB_Ext_Lan_Manager_OWF(&(&(data)->data)->u.lm_owf);
break;
case choice_HDB_extension_data_password:
free_HDB_Ext_Password(&(&(data)->data)->u.password);
break;
case choice_HDB_extension_data_aliases:
free_HDB_Ext_Aliases(&(&(data)->data)->u.aliases);
break;
case choice_HDB_extension_data_last_pw_change:
free_KerberosTime(&(&(data)->data)->u.last_pw_change);
break;
case choice_HDB_extension_data_asn1_ellipsis:
der_free_octet_string(&(&(data)->data)->u.asn1_ellipsis);
break;}
}

size_t
length_HDB_extension(const HDB_extension *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += 1;
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
switch((&(data)->data)->element) {
case choice_HDB_extension_data_pkinit_acl:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_PKINIT_acl(&(&(data)->data)->u.pkinit_acl);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_pkinit_cert_hash:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_PKINIT_hash(&(&(data)->data)->u.pkinit_cert_hash);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_allowed_to_delegate_to:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_Constrained_delegation_acl(&(&(data)->data)->u.allowed_to_delegate_to);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_lm_owf:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_Lan_Manager_OWF(&(&(data)->data)->u.lm_owf);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_password:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_Password(&(&(data)->data)->u.password);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_aliases:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_HDB_Ext_Aliases(&(&(data)->data)->u.aliases);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_last_pw_change:
{
size_t data_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(&(data)->data)->u.last_pw_change);
ret += 1 + der_length_len (ret);
ret += data_tag_oldret;
}
break;
case choice_HDB_extension_data_asn1_ellipsis:
ret += (&(data)->data)->u.asn1_ellipsis.length;
break;
}
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_HDB_extension(const HDB_extension *from, HDB_extension *to)
{
memset(to, 0, sizeof(*to));
*(&(to)->mandatory) = *(&(from)->mandatory);
(&(to)->data)->element = (&(from)->data)->element;
switch((&(from)->data)->element) {
case choice_HDB_extension_data_pkinit_acl:
if(copy_HDB_Ext_PKINIT_acl(&(&(from)->data)->u.pkinit_acl, &(&(to)->data)->u.pkinit_acl)) goto fail;
break;
case choice_HDB_extension_data_pkinit_cert_hash:
if(copy_HDB_Ext_PKINIT_hash(&(&(from)->data)->u.pkinit_cert_hash, &(&(to)->data)->u.pkinit_cert_hash)) goto fail;
break;
case choice_HDB_extension_data_allowed_to_delegate_to:
if(copy_HDB_Ext_Constrained_delegation_acl(&(&(from)->data)->u.allowed_to_delegate_to, &(&(to)->data)->u.allowed_to_delegate_to)) goto fail;
break;
case choice_HDB_extension_data_lm_owf:
if(copy_HDB_Ext_Lan_Manager_OWF(&(&(from)->data)->u.lm_owf, &(&(to)->data)->u.lm_owf)) goto fail;
break;
case choice_HDB_extension_data_password:
if(copy_HDB_Ext_Password(&(&(from)->data)->u.password, &(&(to)->data)->u.password)) goto fail;
break;
case choice_HDB_extension_data_aliases:
if(copy_HDB_Ext_Aliases(&(&(from)->data)->u.aliases, &(&(to)->data)->u.aliases)) goto fail;
break;
case choice_HDB_extension_data_last_pw_change:
if(copy_KerberosTime(&(&(from)->data)->u.last_pw_change, &(&(to)->data)->u.last_pw_change)) goto fail;
break;
case choice_HDB_extension_data_asn1_ellipsis: {
int ret;
ret=der_copy_octet_string(&(&(from)->data)->u.asn1_ellipsis, &(&(to)->data)->u.asn1_ellipsis);
if (ret) goto fail;
break;
}
}
return 0;
fail:
free_HDB_extension(to);
return ENOMEM;
}

