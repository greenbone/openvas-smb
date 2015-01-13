/* Generated from spnego.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <spnego_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_NegTokenResp(unsigned char *p, size_t len, const NegTokenResp *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* mechListMIC */
if((data)->mechListMIC) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->mechListMIC, &l);
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
/* responseToken */
if((data)->responseToken) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->responseToken, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* supportedMech */
if((data)->supportedMech) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_MechType(p, len, (data)->supportedMech, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* negResult */
if((data)->negResult) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_integer(p, len, (const int*)(data)->negResult, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Enumerated, &l);
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
decode_NegTokenResp(const unsigned char *p, size_t len, NegTokenResp *data, size_t *size)
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
size_t negResult_datalen, negResult_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &negResult_datalen, &l);
if(e) {
(data)->negResult = NULL;
} else {
(data)->negResult = calloc(1, sizeof(*(data)->negResult));
if ((data)->negResult == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
negResult_oldlen = len;
if (negResult_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = negResult_datalen;
{
size_t negResult_Tag_datalen, negResult_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Enumerated, &negResult_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
negResult_Tag_oldlen = len;
if (negResult_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = negResult_Tag_datalen;
e = der_get_integer(p, len, (int*)(data)->negResult, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = negResult_Tag_oldlen - negResult_Tag_datalen;
}
len = negResult_oldlen - negResult_datalen;
}
}
{
size_t supportedMech_datalen, supportedMech_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &supportedMech_datalen, &l);
if(e) {
(data)->supportedMech = NULL;
} else {
(data)->supportedMech = calloc(1, sizeof(*(data)->supportedMech));
if ((data)->supportedMech == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
supportedMech_oldlen = len;
if (supportedMech_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = supportedMech_datalen;
e = decode_MechType(p, len, (data)->supportedMech, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = supportedMech_oldlen - supportedMech_datalen;
}
}
{
size_t responseToken_datalen, responseToken_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &responseToken_datalen, &l);
if(e) {
(data)->responseToken = NULL;
} else {
(data)->responseToken = calloc(1, sizeof(*(data)->responseToken));
if ((data)->responseToken == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
responseToken_oldlen = len;
if (responseToken_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = responseToken_datalen;
{
size_t responseToken_Tag_datalen, responseToken_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &responseToken_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
responseToken_Tag_oldlen = len;
if (responseToken_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = responseToken_Tag_datalen;
e = der_get_octet_string(p, len, (data)->responseToken, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = responseToken_Tag_oldlen - responseToken_Tag_datalen;
}
len = responseToken_oldlen - responseToken_datalen;
}
}
{
size_t mechListMIC_datalen, mechListMIC_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &mechListMIC_datalen, &l);
if(e) {
(data)->mechListMIC = NULL;
} else {
(data)->mechListMIC = calloc(1, sizeof(*(data)->mechListMIC));
if ((data)->mechListMIC == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
mechListMIC_oldlen = len;
if (mechListMIC_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mechListMIC_datalen;
{
size_t mechListMIC_Tag_datalen, mechListMIC_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &mechListMIC_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
mechListMIC_Tag_oldlen = len;
if (mechListMIC_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mechListMIC_Tag_datalen;
e = der_get_octet_string(p, len, (data)->mechListMIC, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = mechListMIC_Tag_oldlen - mechListMIC_Tag_datalen;
}
len = mechListMIC_oldlen - mechListMIC_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_NegTokenResp(data);
return e;
}

void
free_NegTokenResp(NegTokenResp *data)
{
if((data)->negResult) {
free((data)->negResult);
(data)->negResult = NULL;
}
if((data)->supportedMech) {
free_MechType((data)->supportedMech);
free((data)->supportedMech);
(data)->supportedMech = NULL;
}
if((data)->responseToken) {
der_free_octet_string((data)->responseToken);
free((data)->responseToken);
(data)->responseToken = NULL;
}
if((data)->mechListMIC) {
der_free_octet_string((data)->mechListMIC);
free((data)->mechListMIC);
(data)->mechListMIC = NULL;
}
}

size_t
length_NegTokenResp(const NegTokenResp *data)
{
size_t ret = 0;
if((data)->negResult){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_integer((const int*)(data)->negResult);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->supportedMech){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_MechType((data)->supportedMech);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->responseToken){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->responseToken);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->mechListMIC){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->mechListMIC);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_NegTokenResp(const NegTokenResp *from, NegTokenResp *to)
{
memset(to, 0, sizeof(*to));
if((from)->negResult) {
(to)->negResult = malloc(sizeof(*(to)->negResult));
if((to)->negResult == NULL) goto fail;
*((to)->negResult) = *((from)->negResult);
}else
(to)->negResult = NULL;
if((from)->supportedMech) {
(to)->supportedMech = malloc(sizeof(*(to)->supportedMech));
if((to)->supportedMech == NULL) goto fail;
if(copy_MechType((from)->supportedMech, (to)->supportedMech)) goto fail;
}else
(to)->supportedMech = NULL;
if((from)->responseToken) {
(to)->responseToken = malloc(sizeof(*(to)->responseToken));
if((to)->responseToken == NULL) goto fail;
if(der_copy_octet_string((from)->responseToken, (to)->responseToken)) goto fail;
}else
(to)->responseToken = NULL;
if((from)->mechListMIC) {
(to)->mechListMIC = malloc(sizeof(*(to)->mechListMIC));
if((to)->mechListMIC == NULL) goto fail;
if(der_copy_octet_string((from)->mechListMIC, (to)->mechListMIC)) goto fail;
}else
(to)->mechListMIC = NULL;
return 0;
fail:
free_NegTokenResp(to);
return ENOMEM;
}

