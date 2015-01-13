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
encode_KDC_REQ_BODY(unsigned char *p, size_t len, const KDC_REQ_BODY *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* additional-tickets */
if((data)->additional_tickets) {
size_t Top_tag_oldret = ret;
ret = 0;
for(i = ((data)->additional_tickets)->len - 1; i >= 0; --i) {
size_t additional_tickets_tag_tag_for_oldret = ret;
ret = 0;
e = encode_Ticket(p, len, &((data)->additional_tickets)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += additional_tickets_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 11, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* enc-authorization-data */
if((data)->enc_authorization_data) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_EncryptedData(p, len, (data)->enc_authorization_data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* addresses */
if((data)->addresses) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_HostAddresses(p, len, (data)->addresses, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* etype */
{
size_t Top_tag_oldret = ret;
ret = 0;
for(i = (&(data)->etype)->len - 1; i >= 0; --i) {
size_t etype_tag_tag_for_oldret = ret;
ret = 0;
e = encode_ENCTYPE(p, len, &(&(data)->etype)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += etype_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* nonce */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->nonce, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* rtime */
if((data)->rtime) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->rtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* till */
if((data)->till) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->till, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* from */
if((data)->from) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->from, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* sname */
if((data)->sname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->sname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* realm */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->realm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* cname */
if((data)->cname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->cname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* kdc-options */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KDCOptions(p, len, &(data)->kdc_options, &l);
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
decode_KDC_REQ_BODY(const unsigned char *p, size_t len, KDC_REQ_BODY *data, size_t *size)
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
size_t kdc_options_datalen, kdc_options_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &kdc_options_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
kdc_options_oldlen = len;
if (kdc_options_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = kdc_options_datalen;
e = decode_KDCOptions(p, len, &(data)->kdc_options, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = kdc_options_oldlen - kdc_options_datalen;
}
{
size_t cname_datalen, cname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &cname_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &realm_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &sname_datalen, &l);
if(e) {
(data)->sname = NULL;
} else {
(data)->sname = calloc(1, sizeof(*(data)->sname));
if ((data)->sname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
sname_oldlen = len;
if (sname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = sname_datalen;
e = decode_PrincipalName(p, len, (data)->sname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = sname_oldlen - sname_datalen;
}
}
{
size_t from_datalen, from_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &from_datalen, &l);
if(e) {
(data)->from = NULL;
} else {
(data)->from = calloc(1, sizeof(*(data)->from));
if ((data)->from == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
from_oldlen = len;
if (from_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = from_datalen;
e = decode_KerberosTime(p, len, (data)->from, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = from_oldlen - from_datalen;
}
}
{
size_t till_datalen, till_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &till_datalen, &l);
if(e) {
(data)->till = NULL;
} else {
(data)->till = calloc(1, sizeof(*(data)->till));
if ((data)->till == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
till_oldlen = len;
if (till_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = till_datalen;
e = decode_KerberosTime(p, len, (data)->till, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = till_oldlen - till_datalen;
}
}
{
size_t rtime_datalen, rtime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &rtime_datalen, &l);
if(e) {
(data)->rtime = NULL;
} else {
(data)->rtime = calloc(1, sizeof(*(data)->rtime));
if ((data)->rtime == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
rtime_oldlen = len;
if (rtime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = rtime_datalen;
e = decode_KerberosTime(p, len, (data)->rtime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = rtime_oldlen - rtime_datalen;
}
}
{
size_t nonce_datalen, nonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &nonce_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
nonce_oldlen = len;
if (nonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonce_datalen;
e = decode_krb5int32(p, len, &(data)->nonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = nonce_oldlen - nonce_datalen;
}
{
size_t etype_datalen, etype_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &etype_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
etype_oldlen = len;
if (etype_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etype_datalen;
{
size_t etype_Tag_datalen, etype_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &etype_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
etype_Tag_oldlen = len;
if (etype_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etype_Tag_datalen;
{
size_t etype_Tag_Tag_origlen = len;
size_t etype_Tag_Tag_oldret = ret;
void *etype_Tag_Tag_tmp;
ret = 0;
(&(data)->etype)->len = 0;
(&(data)->etype)->val = NULL;
while(ret < etype_Tag_Tag_origlen) {
etype_Tag_Tag_tmp = realloc((&(data)->etype)->val,     sizeof(*((&(data)->etype)->val)) * ((&(data)->etype)->len + 1));
if (etype_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->etype)->val = etype_Tag_Tag_tmp;
e = decode_ENCTYPE(p, len, &(&(data)->etype)->val[(&(data)->etype)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
(&(data)->etype)->len++;
len = etype_Tag_Tag_origlen - ret;
}
ret += etype_Tag_Tag_oldret;
}
len = etype_Tag_oldlen - etype_Tag_datalen;
}
len = etype_oldlen - etype_datalen;
}
{
size_t addresses_datalen, addresses_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &addresses_datalen, &l);
if(e) {
(data)->addresses = NULL;
} else {
(data)->addresses = calloc(1, sizeof(*(data)->addresses));
if ((data)->addresses == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
addresses_oldlen = len;
if (addresses_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = addresses_datalen;
e = decode_HostAddresses(p, len, (data)->addresses, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = addresses_oldlen - addresses_datalen;
}
}
{
size_t enc_authorization_data_datalen, enc_authorization_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &enc_authorization_data_datalen, &l);
if(e) {
(data)->enc_authorization_data = NULL;
} else {
(data)->enc_authorization_data = calloc(1, sizeof(*(data)->enc_authorization_data));
if ((data)->enc_authorization_data == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
enc_authorization_data_oldlen = len;
if (enc_authorization_data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = enc_authorization_data_datalen;
e = decode_EncryptedData(p, len, (data)->enc_authorization_data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = enc_authorization_data_oldlen - enc_authorization_data_datalen;
}
}
{
size_t additional_tickets_datalen, additional_tickets_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 11, &additional_tickets_datalen, &l);
if(e) {
(data)->additional_tickets = NULL;
} else {
(data)->additional_tickets = calloc(1, sizeof(*(data)->additional_tickets));
if ((data)->additional_tickets == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
additional_tickets_oldlen = len;
if (additional_tickets_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = additional_tickets_datalen;
{
size_t additional_tickets_Tag_datalen, additional_tickets_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &additional_tickets_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
additional_tickets_Tag_oldlen = len;
if (additional_tickets_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = additional_tickets_Tag_datalen;
{
size_t additional_tickets_Tag_Tag_origlen = len;
size_t additional_tickets_Tag_Tag_oldret = ret;
void *additional_tickets_Tag_Tag_tmp;
ret = 0;
((data)->additional_tickets)->len = 0;
((data)->additional_tickets)->val = NULL;
while(ret < additional_tickets_Tag_Tag_origlen) {
additional_tickets_Tag_Tag_tmp = realloc(((data)->additional_tickets)->val,     sizeof(*(((data)->additional_tickets)->val)) * (((data)->additional_tickets)->len + 1));
if (additional_tickets_Tag_Tag_tmp == NULL) { goto fail; }
((data)->additional_tickets)->val = additional_tickets_Tag_Tag_tmp;
e = decode_Ticket(p, len, &((data)->additional_tickets)->val[((data)->additional_tickets)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
((data)->additional_tickets)->len++;
len = additional_tickets_Tag_Tag_origlen - ret;
}
ret += additional_tickets_Tag_Tag_oldret;
}
len = additional_tickets_Tag_oldlen - additional_tickets_Tag_datalen;
}
len = additional_tickets_oldlen - additional_tickets_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KDC_REQ_BODY(data);
return e;
}

void
free_KDC_REQ_BODY(KDC_REQ_BODY *data)
{
free_KDCOptions(&(data)->kdc_options);
if((data)->cname) {
free_PrincipalName((data)->cname);
free((data)->cname);
(data)->cname = NULL;
}
free_Realm(&(data)->realm);
if((data)->sname) {
free_PrincipalName((data)->sname);
free((data)->sname);
(data)->sname = NULL;
}
if((data)->from) {
free_KerberosTime((data)->from);
free((data)->from);
(data)->from = NULL;
}
if((data)->till) {
free_KerberosTime((data)->till);
free((data)->till);
(data)->till = NULL;
}
if((data)->rtime) {
free_KerberosTime((data)->rtime);
free((data)->rtime);
(data)->rtime = NULL;
}
free_krb5int32(&(data)->nonce);
while((&(data)->etype)->len){
free_ENCTYPE(&(&(data)->etype)->val[(&(data)->etype)->len-1]);
(&(data)->etype)->len--;
}
free((&(data)->etype)->val);
(&(data)->etype)->val = NULL;
if((data)->addresses) {
free_HostAddresses((data)->addresses);
free((data)->addresses);
(data)->addresses = NULL;
}
if((data)->enc_authorization_data) {
free_EncryptedData((data)->enc_authorization_data);
free((data)->enc_authorization_data);
(data)->enc_authorization_data = NULL;
}
if((data)->additional_tickets) {
while(((data)->additional_tickets)->len){
free_Ticket(&((data)->additional_tickets)->val[((data)->additional_tickets)->len-1]);
((data)->additional_tickets)->len--;
}
free(((data)->additional_tickets)->val);
((data)->additional_tickets)->val = NULL;
free((data)->additional_tickets);
(data)->additional_tickets = NULL;
}
}

size_t
length_KDC_REQ_BODY(const KDC_REQ_BODY *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KDCOptions(&(data)->kdc_options);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->cname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->cname);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->realm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->sname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->sname);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->from){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->from);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->till){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->till);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->rtime){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->rtime);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->nonce);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
{
int etype_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->etype)->len - 1; i >= 0; --i){
int etype_tag_tag_for_oldret = ret;
ret = 0;
ret += length_ENCTYPE(&(&(data)->etype)->val[i]);
ret += etype_tag_tag_for_oldret;
}
ret += etype_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->addresses){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_HostAddresses((data)->addresses);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->enc_authorization_data){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_EncryptedData((data)->enc_authorization_data);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->additional_tickets){
size_t Top_tag_oldret = ret;
ret = 0;
{
int additional_tickets_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = ((data)->additional_tickets)->len - 1; i >= 0; --i){
int additional_tickets_tag_tag_for_oldret = ret;
ret = 0;
ret += length_Ticket(&((data)->additional_tickets)->val[i]);
ret += additional_tickets_tag_tag_for_oldret;
}
ret += additional_tickets_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KDC_REQ_BODY(const KDC_REQ_BODY *from, KDC_REQ_BODY *to)
{
memset(to, 0, sizeof(*to));
if(copy_KDCOptions(&(from)->kdc_options, &(to)->kdc_options)) goto fail;
if((from)->cname) {
(to)->cname = malloc(sizeof(*(to)->cname));
if((to)->cname == NULL) goto fail;
if(copy_PrincipalName((from)->cname, (to)->cname)) goto fail;
}else
(to)->cname = NULL;
if(copy_Realm(&(from)->realm, &(to)->realm)) goto fail;
if((from)->sname) {
(to)->sname = malloc(sizeof(*(to)->sname));
if((to)->sname == NULL) goto fail;
if(copy_PrincipalName((from)->sname, (to)->sname)) goto fail;
}else
(to)->sname = NULL;
if((from)->from) {
(to)->from = malloc(sizeof(*(to)->from));
if((to)->from == NULL) goto fail;
if(copy_KerberosTime((from)->from, (to)->from)) goto fail;
}else
(to)->from = NULL;
if((from)->till) {
(to)->till = malloc(sizeof(*(to)->till));
if((to)->till == NULL) goto fail;
if(copy_KerberosTime((from)->till, (to)->till)) goto fail;
}else
(to)->till = NULL;
if((from)->rtime) {
(to)->rtime = malloc(sizeof(*(to)->rtime));
if((to)->rtime == NULL) goto fail;
if(copy_KerberosTime((from)->rtime, (to)->rtime)) goto fail;
}else
(to)->rtime = NULL;
if(copy_krb5int32(&(from)->nonce, &(to)->nonce)) goto fail;
if(((&(to)->etype)->val = malloc((&(from)->etype)->len * sizeof(*(&(to)->etype)->val))) == NULL && (&(from)->etype)->len != 0)
goto fail;
for((&(to)->etype)->len = 0; (&(to)->etype)->len < (&(from)->etype)->len; (&(to)->etype)->len++){
if(copy_ENCTYPE(&(&(from)->etype)->val[(&(to)->etype)->len], &(&(to)->etype)->val[(&(to)->etype)->len])) goto fail;
}
if((from)->addresses) {
(to)->addresses = malloc(sizeof(*(to)->addresses));
if((to)->addresses == NULL) goto fail;
if(copy_HostAddresses((from)->addresses, (to)->addresses)) goto fail;
}else
(to)->addresses = NULL;
if((from)->enc_authorization_data) {
(to)->enc_authorization_data = malloc(sizeof(*(to)->enc_authorization_data));
if((to)->enc_authorization_data == NULL) goto fail;
if(copy_EncryptedData((from)->enc_authorization_data, (to)->enc_authorization_data)) goto fail;
}else
(to)->enc_authorization_data = NULL;
if((from)->additional_tickets) {
(to)->additional_tickets = malloc(sizeof(*(to)->additional_tickets));
if((to)->additional_tickets == NULL) goto fail;
if((((to)->additional_tickets)->val = malloc(((from)->additional_tickets)->len * sizeof(*((to)->additional_tickets)->val))) == NULL && ((from)->additional_tickets)->len != 0)
goto fail;
for(((to)->additional_tickets)->len = 0; ((to)->additional_tickets)->len < ((from)->additional_tickets)->len; ((to)->additional_tickets)->len++){
if(copy_Ticket(&((from)->additional_tickets)->val[((to)->additional_tickets)->len], &((to)->additional_tickets)->val[((to)->additional_tickets)->len])) goto fail;
}
}else
(to)->additional_tickets = NULL;
return 0;
fail:
free_KDC_REQ_BODY(to);
return ENOMEM;
}

