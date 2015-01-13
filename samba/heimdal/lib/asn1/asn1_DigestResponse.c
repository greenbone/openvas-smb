/* Generated from digest.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <digest_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_DigestResponse(unsigned char *p, size_t len, const DigestResponse *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* hash-a1 */
if((data)->hash_a1) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->hash_a1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* channel */
if((data)->channel) {
size_t Top_tag_oldret = ret;
ret = 0;
/* cb-binding */
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &((data)->channel)->cb_binding, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += channel_tag_tag_oldret;
}
/* cb-type */
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &((data)->channel)->cb_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += channel_tag_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* tickets */
if((data)->tickets) {
size_t Top_tag_oldret = ret;
ret = 0;
for(i = ((data)->tickets)->len - 1; i >= 0; --i) {
size_t tickets_tag_tag_for_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &((data)->tickets)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += tickets_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* rsp */
if((data)->rsp) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->rsp, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* responseData */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->responseData, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
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
decode_DigestResponse(const unsigned char *p, size_t len, DigestResponse *data, size_t *size)
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
size_t responseData_datalen, responseData_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &responseData_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
responseData_oldlen = len;
if (responseData_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = responseData_datalen;
e = der_get_utf8string(p, len, &(data)->responseData, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = responseData_oldlen - responseData_datalen;
}
{
size_t rsp_datalen, rsp_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &rsp_datalen, &l);
if(e) {
(data)->rsp = NULL;
} else {
(data)->rsp = calloc(1, sizeof(*(data)->rsp));
if ((data)->rsp == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
rsp_oldlen = len;
if (rsp_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = rsp_datalen;
{
size_t rsp_Tag_datalen, rsp_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &rsp_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
rsp_Tag_oldlen = len;
if (rsp_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = rsp_Tag_datalen;
e = der_get_utf8string(p, len, (data)->rsp, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = rsp_Tag_oldlen - rsp_Tag_datalen;
}
len = rsp_oldlen - rsp_datalen;
}
}
{
size_t tickets_datalen, tickets_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &tickets_datalen, &l);
if(e) {
(data)->tickets = NULL;
} else {
(data)->tickets = calloc(1, sizeof(*(data)->tickets));
if ((data)->tickets == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
tickets_oldlen = len;
if (tickets_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tickets_datalen;
{
size_t tickets_Tag_datalen, tickets_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &tickets_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tickets_Tag_oldlen = len;
if (tickets_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tickets_Tag_datalen;
{
size_t tickets_Tag_Tag_origlen = len;
size_t tickets_Tag_Tag_oldret = ret;
void *tickets_Tag_Tag_tmp;
ret = 0;
((data)->tickets)->len = 0;
((data)->tickets)->val = NULL;
while(ret < tickets_Tag_Tag_origlen) {
tickets_Tag_Tag_tmp = realloc(((data)->tickets)->val,     sizeof(*(((data)->tickets)->val)) * (((data)->tickets)->len + 1));
if (tickets_Tag_Tag_tmp == NULL) { goto fail; }
((data)->tickets)->val = tickets_Tag_Tag_tmp;
{
size_t tickets_Tag_Tag_s_of_datalen, tickets_Tag_Tag_s_of_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &tickets_Tag_Tag_s_of_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tickets_Tag_Tag_s_of_oldlen = len;
if (tickets_Tag_Tag_s_of_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tickets_Tag_Tag_s_of_datalen;
e = der_get_octet_string(p, len, &((data)->tickets)->val[((data)->tickets)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = tickets_Tag_Tag_s_of_oldlen - tickets_Tag_Tag_s_of_datalen;
}
((data)->tickets)->len++;
len = tickets_Tag_Tag_origlen - ret;
}
ret += tickets_Tag_Tag_oldret;
}
len = tickets_Tag_oldlen - tickets_Tag_datalen;
}
len = tickets_oldlen - tickets_datalen;
}
}
{
size_t channel_datalen, channel_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &channel_datalen, &l);
if(e) {
(data)->channel = NULL;
} else {
(data)->channel = calloc(1, sizeof(*(data)->channel));
if ((data)->channel == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
channel_oldlen = len;
if (channel_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = channel_datalen;
{
size_t channel_Tag_datalen, channel_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &channel_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
channel_Tag_oldlen = len;
if (channel_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = channel_Tag_datalen;
{
size_t cb_type_datalen, cb_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &cb_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cb_type_oldlen = len;
if (cb_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cb_type_datalen;
e = der_get_utf8string(p, len, &((data)->channel)->cb_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cb_type_oldlen - cb_type_datalen;
}
{
size_t cb_binding_datalen, cb_binding_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &cb_binding_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cb_binding_oldlen = len;
if (cb_binding_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cb_binding_datalen;
e = der_get_utf8string(p, len, &((data)->channel)->cb_binding, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cb_binding_oldlen - cb_binding_datalen;
}
len = channel_Tag_oldlen - channel_Tag_datalen;
}
len = channel_oldlen - channel_datalen;
}
}
{
size_t hash_a1_datalen, hash_a1_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &hash_a1_datalen, &l);
if(e) {
(data)->hash_a1 = NULL;
} else {
(data)->hash_a1 = calloc(1, sizeof(*(data)->hash_a1));
if ((data)->hash_a1 == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
hash_a1_oldlen = len;
if (hash_a1_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hash_a1_datalen;
{
size_t hash_a1_Tag_datalen, hash_a1_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &hash_a1_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
hash_a1_Tag_oldlen = len;
if (hash_a1_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hash_a1_Tag_datalen;
e = der_get_octet_string(p, len, (data)->hash_a1, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = hash_a1_Tag_oldlen - hash_a1_Tag_datalen;
}
len = hash_a1_oldlen - hash_a1_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestResponse(data);
return e;
}

void
free_DigestResponse(DigestResponse *data)
{
der_free_utf8string(&(data)->responseData);
if((data)->rsp) {
der_free_utf8string((data)->rsp);
free((data)->rsp);
(data)->rsp = NULL;
}
if((data)->tickets) {
while(((data)->tickets)->len){
der_free_octet_string(&((data)->tickets)->val[((data)->tickets)->len-1]);
((data)->tickets)->len--;
}
free(((data)->tickets)->val);
((data)->tickets)->val = NULL;
free((data)->tickets);
(data)->tickets = NULL;
}
if((data)->channel) {
der_free_utf8string(&((data)->channel)->cb_type);
der_free_utf8string(&((data)->channel)->cb_binding);
free((data)->channel);
(data)->channel = NULL;
}
if((data)->hash_a1) {
der_free_octet_string((data)->hash_a1);
free((data)->hash_a1);
(data)->hash_a1 = NULL;
}
}

size_t
length_DigestResponse(const DigestResponse *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->responseData);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->rsp){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->rsp);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->tickets){
size_t Top_tag_oldret = ret;
ret = 0;
{
int tickets_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = ((data)->tickets)->len - 1; i >= 0; --i){
int tickets_tag_tag_for_oldret = ret;
ret = 0;
ret += der_length_octet_string(&((data)->tickets)->val[i]);
ret += 1 + der_length_len (ret);
ret += tickets_tag_tag_for_oldret;
}
ret += tickets_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->channel){
size_t Top_tag_oldret = ret;
ret = 0;
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&((data)->channel)->cb_type);
ret += 1 + der_length_len (ret);
ret += channel_tag_tag_oldret;
}
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&((data)->channel)->cb_binding);
ret += 1 + der_length_len (ret);
ret += channel_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->hash_a1){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->hash_a1);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_DigestResponse(const DigestResponse *from, DigestResponse *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_utf8string(&(from)->responseData, &(to)->responseData)) goto fail;
if((from)->rsp) {
(to)->rsp = malloc(sizeof(*(to)->rsp));
if((to)->rsp == NULL) goto fail;
if(der_copy_utf8string((from)->rsp, (to)->rsp)) goto fail;
}else
(to)->rsp = NULL;
if((from)->tickets) {
(to)->tickets = malloc(sizeof(*(to)->tickets));
if((to)->tickets == NULL) goto fail;
if((((to)->tickets)->val = malloc(((from)->tickets)->len * sizeof(*((to)->tickets)->val))) == NULL && ((from)->tickets)->len != 0)
goto fail;
for(((to)->tickets)->len = 0; ((to)->tickets)->len < ((from)->tickets)->len; ((to)->tickets)->len++){
if(der_copy_octet_string(&((from)->tickets)->val[((to)->tickets)->len], &((to)->tickets)->val[((to)->tickets)->len])) goto fail;
}
}else
(to)->tickets = NULL;
if((from)->channel) {
(to)->channel = malloc(sizeof(*(to)->channel));
if((to)->channel == NULL) goto fail;
if(der_copy_utf8string(&((from)->channel)->cb_type, &((to)->channel)->cb_type)) goto fail;
if(der_copy_utf8string(&((from)->channel)->cb_binding, &((to)->channel)->cb_binding)) goto fail;
}else
(to)->channel = NULL;
if((from)->hash_a1) {
(to)->hash_a1 = malloc(sizeof(*(to)->hash_a1));
if((to)->hash_a1 == NULL) goto fail;
if(der_copy_octet_string((from)->hash_a1, (to)->hash_a1)) goto fail;
}else
(to)->hash_a1 = NULL;
return 0;
fail:
free_DigestResponse(to);
return ENOMEM;
}

