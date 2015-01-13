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
encode_DigestInitReply(unsigned char *p, size_t len, const DigestInitReply *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* identifier */
if((data)->identifier) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->identifier, &l);
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
/* opaque */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->opaque, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* nonce */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->nonce, &l);
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
decode_DigestInitReply(const unsigned char *p, size_t len, DigestInitReply *data, size_t *size)
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
size_t nonce_datalen, nonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &nonce_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
nonce_oldlen = len;
if (nonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonce_datalen;
e = der_get_utf8string(p, len, &(data)->nonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = nonce_oldlen - nonce_datalen;
}
{
size_t opaque_datalen, opaque_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &opaque_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
opaque_oldlen = len;
if (opaque_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = opaque_datalen;
e = der_get_utf8string(p, len, &(data)->opaque, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = opaque_oldlen - opaque_datalen;
}
{
size_t identifier_datalen, identifier_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &identifier_datalen, &l);
if(e) {
(data)->identifier = NULL;
} else {
(data)->identifier = calloc(1, sizeof(*(data)->identifier));
if ((data)->identifier == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
identifier_oldlen = len;
if (identifier_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = identifier_datalen;
{
size_t identifier_Tag_datalen, identifier_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &identifier_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
identifier_Tag_oldlen = len;
if (identifier_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = identifier_Tag_datalen;
e = der_get_utf8string(p, len, (data)->identifier, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = identifier_Tag_oldlen - identifier_Tag_datalen;
}
len = identifier_oldlen - identifier_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestInitReply(data);
return e;
}

void
free_DigestInitReply(DigestInitReply *data)
{
der_free_utf8string(&(data)->nonce);
der_free_utf8string(&(data)->opaque);
if((data)->identifier) {
der_free_utf8string((data)->identifier);
free((data)->identifier);
(data)->identifier = NULL;
}
}

size_t
length_DigestInitReply(const DigestInitReply *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->nonce);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->opaque);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->identifier){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->identifier);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_DigestInitReply(const DigestInitReply *from, DigestInitReply *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_utf8string(&(from)->nonce, &(to)->nonce)) goto fail;
if(der_copy_utf8string(&(from)->opaque, &(to)->opaque)) goto fail;
if((from)->identifier) {
(to)->identifier = malloc(sizeof(*(to)->identifier));
if((to)->identifier == NULL) goto fail;
if(der_copy_utf8string((from)->identifier, (to)->identifier)) goto fail;
}else
(to)->identifier = NULL;
return 0;
fail:
free_DigestInitReply(to);
return ENOMEM;
}

