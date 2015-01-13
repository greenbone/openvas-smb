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
encode_DigestError(unsigned char *p, size_t len, const DigestError *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* code */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_integer(p, len, &(data)->code, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* reason */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->reason, &l);
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
decode_DigestError(const unsigned char *p, size_t len, DigestError *data, size_t *size)
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
size_t reason_datalen, reason_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &reason_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
reason_oldlen = len;
if (reason_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = reason_datalen;
e = der_get_utf8string(p, len, &(data)->reason, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = reason_oldlen - reason_datalen;
}
{
size_t code_datalen, code_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &code_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
code_oldlen = len;
if (code_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = code_datalen;
e = der_get_integer(p, len, &(data)->code, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = code_oldlen - code_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestError(data);
return e;
}

void
free_DigestError(DigestError *data)
{
der_free_utf8string(&(data)->reason);
}

size_t
length_DigestError(const DigestError *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->reason);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_integer(&(data)->code);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_DigestError(const DigestError *from, DigestError *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_utf8string(&(from)->reason, &(to)->reason)) goto fail;
*(&(to)->code) = *(&(from)->code);
return 0;
fail:
free_DigestError(to);
return ENOMEM;
}

