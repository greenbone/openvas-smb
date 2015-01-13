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
encode_EncKrbCredPart(unsigned char *p, size_t len, const EncKrbCredPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* r-address */
if((data)->r_address) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_HostAddress(p, len, (data)->r_address, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* s-address */
if((data)->s_address) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_HostAddress(p, len, (data)->s_address, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* usec */
if((data)->usec) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->usec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* timestamp */
if((data)->timestamp) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->timestamp, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* nonce */
if((data)->nonce) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->nonce, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* ticket-info */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
for(i = (&(data)->ticket_info)->len - 1; i >= 0; --i) {
size_t ticket_info_tag_tag_for_oldret = ret;
ret = 0;
e = encode_KrbCredInfo(p, len, &(&(data)->ticket_info)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += ticket_info_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 29, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_EncKrbCredPart(const unsigned char *p, size_t len, EncKrbCredPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 29, &Top_datalen, &l);
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
size_t ticket_info_datalen, ticket_info_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &ticket_info_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ticket_info_oldlen = len;
if (ticket_info_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ticket_info_datalen;
{
size_t ticket_info_Tag_datalen, ticket_info_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &ticket_info_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ticket_info_Tag_oldlen = len;
if (ticket_info_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ticket_info_Tag_datalen;
{
size_t ticket_info_Tag_Tag_origlen = len;
size_t ticket_info_Tag_Tag_oldret = ret;
void *ticket_info_Tag_Tag_tmp;
ret = 0;
(&(data)->ticket_info)->len = 0;
(&(data)->ticket_info)->val = NULL;
while(ret < ticket_info_Tag_Tag_origlen) {
ticket_info_Tag_Tag_tmp = realloc((&(data)->ticket_info)->val,     sizeof(*((&(data)->ticket_info)->val)) * ((&(data)->ticket_info)->len + 1));
if (ticket_info_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->ticket_info)->val = ticket_info_Tag_Tag_tmp;
e = decode_KrbCredInfo(p, len, &(&(data)->ticket_info)->val[(&(data)->ticket_info)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
(&(data)->ticket_info)->len++;
len = ticket_info_Tag_Tag_origlen - ret;
}
ret += ticket_info_Tag_Tag_oldret;
}
len = ticket_info_Tag_oldlen - ticket_info_Tag_datalen;
}
len = ticket_info_oldlen - ticket_info_datalen;
}
{
size_t nonce_datalen, nonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &nonce_datalen, &l);
if(e) {
(data)->nonce = NULL;
} else {
(data)->nonce = calloc(1, sizeof(*(data)->nonce));
if ((data)->nonce == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
nonce_oldlen = len;
if (nonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonce_datalen;
e = decode_krb5int32(p, len, (data)->nonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = nonce_oldlen - nonce_datalen;
}
}
{
size_t timestamp_datalen, timestamp_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &timestamp_datalen, &l);
if(e) {
(data)->timestamp = NULL;
} else {
(data)->timestamp = calloc(1, sizeof(*(data)->timestamp));
if ((data)->timestamp == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
timestamp_oldlen = len;
if (timestamp_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = timestamp_datalen;
e = decode_KerberosTime(p, len, (data)->timestamp, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = timestamp_oldlen - timestamp_datalen;
}
}
{
size_t usec_datalen, usec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &usec_datalen, &l);
if(e) {
(data)->usec = NULL;
} else {
(data)->usec = calloc(1, sizeof(*(data)->usec));
if ((data)->usec == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
usec_oldlen = len;
if (usec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = usec_datalen;
e = decode_krb5int32(p, len, (data)->usec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = usec_oldlen - usec_datalen;
}
}
{
size_t s_address_datalen, s_address_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &s_address_datalen, &l);
if(e) {
(data)->s_address = NULL;
} else {
(data)->s_address = calloc(1, sizeof(*(data)->s_address));
if ((data)->s_address == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
s_address_oldlen = len;
if (s_address_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = s_address_datalen;
e = decode_HostAddress(p, len, (data)->s_address, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = s_address_oldlen - s_address_datalen;
}
}
{
size_t r_address_datalen, r_address_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &r_address_datalen, &l);
if(e) {
(data)->r_address = NULL;
} else {
(data)->r_address = calloc(1, sizeof(*(data)->r_address));
if ((data)->r_address == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
r_address_oldlen = len;
if (r_address_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = r_address_datalen;
e = decode_HostAddress(p, len, (data)->r_address, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = r_address_oldlen - r_address_datalen;
}
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncKrbCredPart(data);
return e;
}

void
free_EncKrbCredPart(EncKrbCredPart *data)
{
while((&(data)->ticket_info)->len){
free_KrbCredInfo(&(&(data)->ticket_info)->val[(&(data)->ticket_info)->len-1]);
(&(data)->ticket_info)->len--;
}
free((&(data)->ticket_info)->val);
(&(data)->ticket_info)->val = NULL;
if((data)->nonce) {
free_krb5int32((data)->nonce);
free((data)->nonce);
(data)->nonce = NULL;
}
if((data)->timestamp) {
free_KerberosTime((data)->timestamp);
free((data)->timestamp);
(data)->timestamp = NULL;
}
if((data)->usec) {
free_krb5int32((data)->usec);
free((data)->usec);
(data)->usec = NULL;
}
if((data)->s_address) {
free_HostAddress((data)->s_address);
free((data)->s_address);
(data)->s_address = NULL;
}
if((data)->r_address) {
free_HostAddress((data)->r_address);
free((data)->r_address);
(data)->r_address = NULL;
}
}

size_t
length_EncKrbCredPart(const EncKrbCredPart *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
{
int ticket_info_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->ticket_info)->len - 1; i >= 0; --i){
int ticket_info_tag_tag_for_oldret = ret;
ret = 0;
ret += length_KrbCredInfo(&(&(data)->ticket_info)->val[i]);
ret += ticket_info_tag_tag_for_oldret;
}
ret += ticket_info_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->nonce){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->nonce);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->timestamp){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->timestamp);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->usec){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->usec);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->s_address){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_HostAddress((data)->s_address);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->r_address){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_HostAddress((data)->r_address);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncKrbCredPart(const EncKrbCredPart *from, EncKrbCredPart *to)
{
memset(to, 0, sizeof(*to));
if(((&(to)->ticket_info)->val = malloc((&(from)->ticket_info)->len * sizeof(*(&(to)->ticket_info)->val))) == NULL && (&(from)->ticket_info)->len != 0)
goto fail;
for((&(to)->ticket_info)->len = 0; (&(to)->ticket_info)->len < (&(from)->ticket_info)->len; (&(to)->ticket_info)->len++){
if(copy_KrbCredInfo(&(&(from)->ticket_info)->val[(&(to)->ticket_info)->len], &(&(to)->ticket_info)->val[(&(to)->ticket_info)->len])) goto fail;
}
if((from)->nonce) {
(to)->nonce = malloc(sizeof(*(to)->nonce));
if((to)->nonce == NULL) goto fail;
if(copy_krb5int32((from)->nonce, (to)->nonce)) goto fail;
}else
(to)->nonce = NULL;
if((from)->timestamp) {
(to)->timestamp = malloc(sizeof(*(to)->timestamp));
if((to)->timestamp == NULL) goto fail;
if(copy_KerberosTime((from)->timestamp, (to)->timestamp)) goto fail;
}else
(to)->timestamp = NULL;
if((from)->usec) {
(to)->usec = malloc(sizeof(*(to)->usec));
if((to)->usec == NULL) goto fail;
if(copy_krb5int32((from)->usec, (to)->usec)) goto fail;
}else
(to)->usec = NULL;
if((from)->s_address) {
(to)->s_address = malloc(sizeof(*(to)->s_address));
if((to)->s_address == NULL) goto fail;
if(copy_HostAddress((from)->s_address, (to)->s_address)) goto fail;
}else
(to)->s_address = NULL;
if((from)->r_address) {
(to)->r_address = malloc(sizeof(*(to)->r_address));
if((to)->r_address == NULL) goto fail;
if(copy_HostAddress((from)->r_address, (to)->r_address)) goto fail;
}else
(to)->r_address = NULL;
return 0;
fail:
free_EncKrbCredPart(to);
return ENOMEM;
}

