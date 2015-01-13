/* Generated from k5.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <krb5_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_KRB_ERROR(unsigned char *p, size_t len, const KRB_ERROR *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* e-data */
if((data)->e_data) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->e_data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 12, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* e-text */
if((data)->e_text) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = der_put_general_string(p, len, (data)->e_text, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_GeneralString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 11, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* sname */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, &(data)->sname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* realm */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->realm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cname */
if((data)->cname) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->cname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* crealm */
if((data)->crealm) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, (data)->crealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* error-code */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->error_code, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* susec */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->susec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* stime */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->stime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cusec */
if((data)->cusec) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->cusec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* ctime */
if((data)->ctime) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->ctime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* msg-type */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* pvno */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->pvno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 30, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_KRB_ERROR(const unsigned char *p, size_t len, KRB_ERROR *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 30, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
{
size_t Top_Tag_datalen, Top_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &Top_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_Tag_oldlen = len;
if (Top_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_Tag_datalen;
{
size_t pvno_datalen, pvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &pvno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
pvno_oldlen = len;
if (pvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pvno_datalen;
e = decode_krb5int32(p, len, &(data)->pvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pvno_oldlen - pvno_datalen;
}
{
size_t msg_type_datalen, msg_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &msg_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
msg_type_oldlen = len;
if (msg_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = msg_type_datalen;
e = decode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = msg_type_oldlen - msg_type_datalen;
}
{
size_t ctime_datalen, ctime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &ctime_datalen, &l);
if(e) {
(data)->ctime = NULL;
} else {
(data)->ctime = calloc(1, sizeof(*(data)->ctime));
if ((data)->ctime == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
ctime_oldlen = len;
if (ctime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ctime_datalen;
e = decode_KerberosTime(p, len, (data)->ctime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = ctime_oldlen - ctime_datalen;
}
}
{
size_t cusec_datalen, cusec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &cusec_datalen, &l);
if(e) {
(data)->cusec = NULL;
} else {
(data)->cusec = calloc(1, sizeof(*(data)->cusec));
if ((data)->cusec == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
cusec_oldlen = len;
if (cusec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cusec_datalen;
e = decode_krb5int32(p, len, (data)->cusec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cusec_oldlen - cusec_datalen;
}
}
{
size_t stime_datalen, stime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &stime_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
stime_oldlen = len;
if (stime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = stime_datalen;
e = decode_KerberosTime(p, len, &(data)->stime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = stime_oldlen - stime_datalen;
}
{
size_t susec_datalen, susec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &susec_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
susec_oldlen = len;
if (susec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = susec_datalen;
e = decode_krb5int32(p, len, &(data)->susec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = susec_oldlen - susec_datalen;
}
{
size_t error_code_datalen, error_code_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &error_code_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
error_code_oldlen = len;
if (error_code_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = error_code_datalen;
e = decode_krb5int32(p, len, &(data)->error_code, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = error_code_oldlen - error_code_datalen;
}
{
size_t crealm_datalen, crealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &crealm_datalen, &l);
if(e) {
(data)->crealm = NULL;
} else {
(data)->crealm = calloc(1, sizeof(*(data)->crealm));
if ((data)->crealm == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
crealm_oldlen = len;
if (crealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = crealm_datalen;
e = decode_Realm(p, len, (data)->crealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = crealm_oldlen - crealm_datalen;
}
}
{
size_t cname_datalen, cname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &cname_datalen, &l);
if(e) {
(data)->cname = NULL;
} else {
(data)->cname = calloc(1, sizeof(*(data)->cname));
if ((data)->cname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
cname_oldlen = len;
if (cname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cname_datalen;
e = decode_PrincipalName(p, len, (data)->cname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cname_oldlen - cname_datalen;
}
}
{
size_t realm_datalen, realm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &realm_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
realm_oldlen = len;
if (realm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = realm_datalen;
e = decode_Realm(p, len, &(data)->realm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = realm_oldlen - realm_datalen;
}
{
size_t sname_datalen, sname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &sname_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
sname_oldlen = len;
if (sname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = sname_datalen;
e = decode_PrincipalName(p, len, &(data)->sname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = sname_oldlen - sname_datalen;
}
{
size_t e_text_datalen, e_text_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 11, &e_text_datalen, &l);
if(e) {
(data)->e_text = NULL;
} else {
(data)->e_text = calloc(1, sizeof(*(data)->e_text));
if ((data)->e_text == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
e_text_oldlen = len;
if (e_text_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = e_text_datalen;
{
size_t e_text_Tag_datalen, e_text_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_GeneralString, &e_text_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
e_text_Tag_oldlen = len;
if (e_text_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = e_text_Tag_datalen;
e = der_get_general_string(p, len, (data)->e_text, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = e_text_Tag_oldlen - e_text_Tag_datalen;
}
len = e_text_oldlen - e_text_datalen;
}
}
{
size_t e_data_datalen, e_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 12, &e_data_datalen, &l);
if(e) {
(data)->e_data = NULL;
} else {
(data)->e_data = calloc(1, sizeof(*(data)->e_data));
if ((data)->e_data == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
e_data_oldlen = len;
if (e_data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = e_data_datalen;
{
size_t e_data_Tag_datalen, e_data_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &e_data_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
e_data_Tag_oldlen = len;
if (e_data_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = e_data_Tag_datalen;
e = der_get_octet_string(p, len, (data)->e_data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = e_data_Tag_oldlen - e_data_Tag_datalen;
}
len = e_data_oldlen - e_data_datalen;
}
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KRB_ERROR(data);
return e;
}

void
free_KRB_ERROR(KRB_ERROR *data)
{
free_krb5int32(&(data)->pvno);
free_MESSAGE_TYPE(&(data)->msg_type);
if((data)->ctime) {
free_KerberosTime((data)->ctime);
free((data)->ctime);
(data)->ctime = NULL;
}
if((data)->cusec) {
free_krb5int32((data)->cusec);
free((data)->cusec);
(data)->cusec = NULL;
}
free_KerberosTime(&(data)->stime);
free_krb5int32(&(data)->susec);
free_krb5int32(&(data)->error_code);
if((data)->crealm) {
free_Realm((data)->crealm);
free((data)->crealm);
(data)->crealm = NULL;
}
if((data)->cname) {
free_PrincipalName((data)->cname);
free((data)->cname);
(data)->cname = NULL;
}
free_Realm(&(data)->realm);
free_PrincipalName(&(data)->sname);
if((data)->e_text) {
der_free_general_string((data)->e_text);
free((data)->e_text);
(data)->e_text = NULL;
}
if((data)->e_data) {
der_free_octet_string((data)->e_data);
free((data)->e_data);
(data)->e_data = NULL;
}
}

size_t
length_KRB_ERROR(const KRB_ERROR *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->pvno);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_MESSAGE_TYPE(&(data)->msg_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->ctime){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->ctime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->cusec){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->cusec);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->stime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->susec);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->error_code);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->crealm){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_Realm((data)->crealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->cname){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->cname);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->realm);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName(&(data)->sname);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->e_text){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += der_length_general_string((data)->e_text);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->e_data){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->e_data);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KRB_ERROR(const KRB_ERROR *from, KRB_ERROR *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->pvno, &(to)->pvno)) goto fail;
if(copy_MESSAGE_TYPE(&(from)->msg_type, &(to)->msg_type)) goto fail;
if((from)->ctime) {
(to)->ctime = malloc(sizeof(*(to)->ctime));
if((to)->ctime == NULL) goto fail;
if(copy_KerberosTime((from)->ctime, (to)->ctime)) goto fail;
}else
(to)->ctime = NULL;
if((from)->cusec) {
(to)->cusec = malloc(sizeof(*(to)->cusec));
if((to)->cusec == NULL) goto fail;
if(copy_krb5int32((from)->cusec, (to)->cusec)) goto fail;
}else
(to)->cusec = NULL;
if(copy_KerberosTime(&(from)->stime, &(to)->stime)) goto fail;
if(copy_krb5int32(&(from)->susec, &(to)->susec)) goto fail;
if(copy_krb5int32(&(from)->error_code, &(to)->error_code)) goto fail;
if((from)->crealm) {
(to)->crealm = malloc(sizeof(*(to)->crealm));
if((to)->crealm == NULL) goto fail;
if(copy_Realm((from)->crealm, (to)->crealm)) goto fail;
}else
(to)->crealm = NULL;
if((from)->cname) {
(to)->cname = malloc(sizeof(*(to)->cname));
if((to)->cname == NULL) goto fail;
if(copy_PrincipalName((from)->cname, (to)->cname)) goto fail;
}else
(to)->cname = NULL;
if(copy_Realm(&(from)->realm, &(to)->realm)) goto fail;
if(copy_PrincipalName(&(from)->sname, &(to)->sname)) goto fail;
if((from)->e_text) {
(to)->e_text = malloc(sizeof(*(to)->e_text));
if((to)->e_text == NULL) goto fail;
if(der_copy_general_string((from)->e_text, (to)->e_text)) goto fail;
}else
(to)->e_text = NULL;
if((from)->e_data) {
(to)->e_data = malloc(sizeof(*(to)->e_data));
if((to)->e_data == NULL) goto fail;
if(der_copy_octet_string((from)->e_data, (to)->e_data)) goto fail;
}else
(to)->e_data = NULL;
return 0;
fail:
free_KRB_ERROR(to);
return ENOMEM;
}

