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
encode_Checksum(unsigned char *p, size_t len, const Checksum *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* checksum */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->checksum, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* cksumtype */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_CKSUMTYPE(p, len, &(data)->cksumtype, &l);
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
decode_Checksum(const unsigned char *p, size_t len, Checksum *data, size_t *size)
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
size_t cksumtype_datalen, cksumtype_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &cksumtype_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cksumtype_oldlen = len;
if (cksumtype_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cksumtype_datalen;
e = decode_CKSUMTYPE(p, len, &(data)->cksumtype, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cksumtype_oldlen - cksumtype_datalen;
}
{
size_t checksum_datalen, checksum_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &checksum_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
checksum_oldlen = len;
if (checksum_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = checksum_datalen;
{
size_t checksum_Tag_datalen, checksum_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &checksum_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
checksum_Tag_oldlen = len;
if (checksum_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = checksum_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->checksum, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = checksum_Tag_oldlen - checksum_Tag_datalen;
}
len = checksum_oldlen - checksum_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Checksum(data);
return e;
}

void
free_Checksum(Checksum *data)
{
free_CKSUMTYPE(&(data)->cksumtype);
der_free_octet_string(&(data)->checksum);
}

size_t
length_Checksum(const Checksum *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_CKSUMTYPE(&(data)->cksumtype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->checksum);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Checksum(const Checksum *from, Checksum *to)
{
memset(to, 0, sizeof(*to));
if(copy_CKSUMTYPE(&(from)->cksumtype, &(to)->cksumtype)) goto fail;
if(der_copy_octet_string(&(from)->checksum, &(to)->checksum)) goto fail;
return 0;
fail:
free_Checksum(to);
return ENOMEM;
}

