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
encode_DigestReqInner(unsigned char *p, size_t len, const DigestReqInner *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;

switch((data)->element) {
case choice_DigestReqInner_digestRequest: {size_t Top_oldret = ret;
ret = 0;
e = encode_DigestRequest(p, len, &((data))->u.digestRequest, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_oldret;
break;
}
case choice_DigestReqInner_init: {size_t Top_oldret = ret;
ret = 0;
e = encode_DigestInit(p, len, &((data))->u.init, &l);
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
decode_DigestReqInner(const unsigned char *p, size_t len, DigestReqInner *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 0, NULL) == 0) {
{
size_t init_datalen, init_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &init_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
init_oldlen = len;
if (init_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = init_datalen;
e = decode_DigestInit(p, len, &(data)->u.init, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = init_oldlen - init_datalen;
}
(data)->element = choice_DigestReqInner_init;
}
else if (der_match_tag(p, len, ASN1_C_CONTEXT, CONS, 1, NULL) == 0) {
{
size_t digestRequest_datalen, digestRequest_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &digestRequest_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
digestRequest_oldlen = len;
if (digestRequest_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = digestRequest_datalen;
e = decode_DigestRequest(p, len, &(data)->u.digestRequest, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = digestRequest_oldlen - digestRequest_datalen;
}
(data)->element = choice_DigestReqInner_digestRequest;
}
else {
e = ASN1_PARSE_ERROR;
goto fail;
}
if(size) *size = ret;
return 0;
fail:
free_DigestReqInner(data);
return e;
}

void
free_DigestReqInner(DigestReqInner *data)
{
switch((data)->element) {
case choice_DigestReqInner_init:
free_DigestInit(&(data)->u.init);
break;
case choice_DigestReqInner_digestRequest:
free_DigestRequest(&(data)->u.digestRequest);
break;
}
}

size_t
length_DigestReqInner(const DigestReqInner *data)
{
size_t ret = 0;
switch((data)->element) {
case choice_DigestReqInner_init:
{
size_t Top_oldret = ret;
ret = 0;
ret += length_DigestInit(&(data)->u.init);
ret += 1 + der_length_len (ret);
ret += Top_oldret;
}
break;
case choice_DigestReqInner_digestRequest:
{
size_t Top_oldret = ret;
ret = 0;
ret += length_DigestRequest(&(data)->u.digestRequest);
ret += 1 + der_length_len (ret);
ret += Top_oldret;
}
break;
}
return ret;
}

int
copy_DigestReqInner(const DigestReqInner *from, DigestReqInner *to)
{
memset(to, 0, sizeof(*to));
(to)->element = (from)->element;
switch((from)->element) {
case choice_DigestReqInner_init:
if(copy_DigestInit(&(from)->u.init, &(to)->u.init)) goto fail;
break;
case choice_DigestReqInner_digestRequest:
if(copy_DigestRequest(&(from)->u.digestRequest, &(to)->u.digestRequest)) goto fail;
break;
}
return 0;
fail:
free_DigestReqInner(to);
return ENOMEM;
}

