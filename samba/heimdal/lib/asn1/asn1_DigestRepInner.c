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
encode_DigestRepInner(unsigned char *p, size_t len, const DigestRepInner *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;

switch((data)->element) {
case choice_DigestRepInner_response: {size_t Top_oldret = ret;
ret = 0;
e = encode_DigestResponse(p, len, &((data))->u.response, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_oldret;
break;
}
case choice_DigestRepInner_initReply: {size_t Top_oldret = ret;
ret = 0;
e = encode_DigestInitReply(p, len, &((data))->u.initReply, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_oldret;
break;
}
case choice_DigestRepInner_error: {size_t Top_oldret = ret;
ret = 0;
e = encode_DigestError(p, len, &((data))->u.error, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_oldret;
break;
}
};
*size = ret;
return 0;
}

int
decode_DigestRepInner(const unsigned char *p, size_t len, DigestRepInner *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 0, NULL) == 0) {
{
size_t error_datalen, error_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &error_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
error_oldlen = len;
if (error_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = error_datalen;
e = decode_DigestError(p, len, &(data)->u.error, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = error_oldlen - error_datalen;
}
(data)->element = choice_DigestRepInner_error;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 1, NULL) == 0) {
{
size_t initReply_datalen, initReply_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &initReply_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
initReply_oldlen = len;
if (initReply_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = initReply_datalen;
e = decode_DigestInitReply(p, len, &(data)->u.initReply, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = initReply_oldlen - initReply_datalen;
}
(data)->element = choice_DigestRepInner_initReply;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 2, NULL) == 0) {
{
size_t response_datalen, response_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &response_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
response_oldlen = len;
if (response_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = response_datalen;
e = decode_DigestResponse(p, len, &(data)->u.response, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = response_oldlen - response_datalen;
}
(data)->element = choice_DigestRepInner_response;
}
else {
e = ASN1_PARSE_ERROR;
goto fail;
}
if(size) *size = ret;
return 0;
fail:
free_DigestRepInner(data);
return e;
}

void
free_DigestRepInner(DigestRepInner *data)
{
switch((data)->element) {
case choice_DigestRepInner_error:
free_DigestError(&(data)->u.error);
break;
case choice_DigestRepInner_initReply:
free_DigestInitReply(&(data)->u.initReply);
break;
case choice_DigestRepInner_response:
free_DigestResponse(&(data)->u.response);
break;
}
}

size_t
length_DigestRepInner(const DigestRepInner *data)
{
size_t ret = 0;
switch((data)->element) {
case choice_DigestRepInner_error:
{
size_t Top_oldret = ret;
ret = 0;
ret += length_DigestError(&(data)->u.error);
ret += 1 + der_length_len (ret);
ret += Top_oldret;
}
break;
case choice_DigestRepInner_initReply:
{
size_t Top_oldret = ret;
ret = 0;
ret += length_DigestInitReply(&(data)->u.initReply);
ret += 1 + der_length_len (ret);
ret += Top_oldret;
}
break;
case choice_DigestRepInner_response:
{
size_t Top_oldret = ret;
ret = 0;
ret += length_DigestResponse(&(data)->u.response);
ret += 1 + der_length_len (ret);
ret += Top_oldret;
}
break;
}
return ret;
}

int
copy_DigestRepInner(const DigestRepInner *from, DigestRepInner *to)
{
memset(to, 0, sizeof(*to));
(to)->element = (from)->element;
switch((from)->element) {
case choice_DigestRepInner_error:
if(copy_DigestError(&(from)->u.error, &(to)->u.error)) goto fail;
break;
case choice_DigestRepInner_initReply:
if(copy_DigestInitReply(&(from)->u.initReply, &(to)->u.initReply)) goto fail;
break;
case choice_DigestRepInner_response:
if(copy_DigestResponse(&(from)->u.response, &(to)->u.response)) goto fail;
break;
}
return 0;
fail:
free_DigestRepInner(to);
return ENOMEM;
}

