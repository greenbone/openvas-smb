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
encode_TGS_REP(unsigned char *p, size_t len, const TGS_REP *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
e = encode_KDC_REP(p, len, data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 13, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_TGS_REP(const unsigned char *p, size_t len, TGS_REP *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 13, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
e = decode_KDC_REP(p, len, data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_TGS_REP(data);
return e;
}

void
free_TGS_REP(TGS_REP *data)
{
free_KDC_REP(data);
}

size_t
length_TGS_REP(const TGS_REP *data)
{
size_t ret = 0;
ret += length_KDC_REP(data);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_TGS_REP(const TGS_REP *from, TGS_REP *to)
{
memset(to, 0, sizeof(*to));
if(copy_KDC_REP(from, to)) goto fail;
return 0;
fail:
free_TGS_REP(to);
return ENOMEM;
}

