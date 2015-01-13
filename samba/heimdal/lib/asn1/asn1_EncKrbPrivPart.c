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
encode_EncKrbPrivPart(unsigned char *p, size_t len, const EncKrbPrivPart *data, size_t *size)
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
/* seq-number */
if((data)->seq_number) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5uint32(p, len, (data)->seq_number, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* user-data */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->user_data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 28, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_EncKrbPrivPart(const unsigned char *p, size_t len, EncKrbPrivPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 28, &Top_datalen, &l);
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
size_t user_data_datalen, user_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &user_data_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
user_data_oldlen = len;
if (user_data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = user_data_datalen;
{
size_t user_data_Tag_datalen, user_data_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &user_data_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
user_data_Tag_oldlen = len;
if (user_data_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = user_data_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->user_data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = user_data_Tag_oldlen - user_data_Tag_datalen;
}
len = user_data_oldlen - user_data_datalen;
}
{
size_t timestamp_datalen, timestamp_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &timestamp_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &usec_datalen, &l);
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
size_t seq_number_datalen, seq_number_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &seq_number_datalen, &l);
if(e) {
(data)->seq_number = NULL;
} else {
(data)->seq_number = calloc(1, sizeof(*(data)->seq_number));
if ((data)->seq_number == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
seq_number_oldlen = len;
if (seq_number_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = seq_number_datalen;
e = decode_krb5uint32(p, len, (data)->seq_number, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = seq_number_oldlen - seq_number_datalen;
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
free_EncKrbPrivPart(data);
return e;
}

void
free_EncKrbPrivPart(EncKrbPrivPart *data)
{
der_free_octet_string(&(data)->user_data);
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
if((data)->seq_number) {
free_krb5uint32((data)->seq_number);
free((data)->seq_number);
(data)->seq_number = NULL;
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
length_EncKrbPrivPart(const EncKrbPrivPart *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->user_data);
ret += 1 + der_length_len (ret);
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
if((data)->seq_number){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5uint32((data)->seq_number);
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
copy_EncKrbPrivPart(const EncKrbPrivPart *from, EncKrbPrivPart *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_octet_string(&(from)->user_data, &(to)->user_data)) goto fail;
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
if((from)->seq_number) {
(to)->seq_number = malloc(sizeof(*(to)->seq_number));
if((to)->seq_number == NULL) goto fail;
if(copy_krb5uint32((from)->seq_number, (to)->seq_number)) goto fail;
}else
(to)->seq_number = NULL;
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
free_EncKrbPrivPart(to);
return ENOMEM;
}

