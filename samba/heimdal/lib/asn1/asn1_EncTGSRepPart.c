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
encode_EncTGSRepPart(unsigned char *p, size_t len, const EncTGSRepPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
e = encode_EncKDCRepPart(p, len, data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 26, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_EncTGSRepPart(const unsigned char *p, size_t len, EncTGSRepPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 26, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
e = decode_EncKDCRepPart(p, len, data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncTGSRepPart(data);
return e;
}

void
free_EncTGSRepPart(EncTGSRepPart *data)
{
free_EncKDCRepPart(data);
}

size_t
length_EncTGSRepPart(const EncTGSRepPart *data)
{
size_t ret = 0;
ret += length_EncKDCRepPart(data);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncTGSRepPart(const EncTGSRepPart *from, EncTGSRepPart *to)
{
memset(to, 0, sizeof(*to));
if(copy_EncKDCRepPart(from, to)) goto fail;
return 0;
fail:
free_EncTGSRepPart(to);
return ENOMEM;
}

