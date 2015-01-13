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
encode_DigestREP(unsigned char *p, size_t len, const DigestREP *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* innerRep */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptedData(p, len, &(data)->innerRep, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* apRep */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->apRep, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 129, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_DigestREP(const unsigned char *p, size_t len, DigestREP *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 129, &Top_datalen, &l);
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
size_t apRep_datalen, apRep_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &apRep_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
apRep_oldlen = len;
if (apRep_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = apRep_datalen;
{
size_t apRep_Tag_datalen, apRep_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &apRep_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
apRep_Tag_oldlen = len;
if (apRep_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = apRep_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->apRep, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = apRep_Tag_oldlen - apRep_Tag_datalen;
}
len = apRep_oldlen - apRep_datalen;
}
{
size_t innerRep_datalen, innerRep_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &innerRep_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
innerRep_oldlen = len;
if (innerRep_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = innerRep_datalen;
e = decode_EncryptedData(p, len, &(data)->innerRep, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = innerRep_oldlen - innerRep_datalen;
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestREP(data);
return e;
}

void
free_DigestREP(DigestREP *data)
{
der_free_octet_string(&(data)->apRep);
free_EncryptedData(&(data)->innerRep);
}

size_t
length_DigestREP(const DigestREP *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->apRep);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptedData(&(data)->innerRep);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 3 + der_length_len (ret);
return ret;
}

int
copy_DigestREP(const DigestREP *from, DigestREP *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_octet_string(&(from)->apRep, &(to)->apRep)) goto fail;
if(copy_EncryptedData(&(from)->innerRep, &(to)->innerRep)) goto fail;
return 0;
fail:
free_DigestREP(to);
return ENOMEM;
}

