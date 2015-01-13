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
encode_DigestREQ(unsigned char *p, size_t len, const DigestREQ *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* innerReq */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptedData(p, len, &(data)->innerReq, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* apReq */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->apReq, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 128, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_DigestREQ(const unsigned char *p, size_t len, DigestREQ *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 128, &Top_datalen, &l);
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
size_t apReq_datalen, apReq_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &apReq_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
apReq_oldlen = len;
if (apReq_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = apReq_datalen;
{
size_t apReq_Tag_datalen, apReq_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &apReq_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
apReq_Tag_oldlen = len;
if (apReq_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = apReq_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->apReq, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = apReq_Tag_oldlen - apReq_Tag_datalen;
}
len = apReq_oldlen - apReq_datalen;
}
{
size_t innerReq_datalen, innerReq_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &innerReq_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
innerReq_oldlen = len;
if (innerReq_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = innerReq_datalen;
e = decode_EncryptedData(p, len, &(data)->innerReq, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = innerReq_oldlen - innerReq_datalen;
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestREQ(data);
return e;
}

void
free_DigestREQ(DigestREQ *data)
{
der_free_octet_string(&(data)->apReq);
free_EncryptedData(&(data)->innerReq);
}

size_t
length_DigestREQ(const DigestREQ *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->apReq);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptedData(&(data)->innerReq);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 3 + der_length_len (ret);
return ret;
}

int
copy_DigestREQ(const DigestREQ *from, DigestREQ *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_octet_string(&(from)->apReq, &(to)->apReq)) goto fail;
if(copy_EncryptedData(&(from)->innerReq, &(to)->innerReq)) goto fail;
return 0;
fail:
free_DigestREQ(to);
return ENOMEM;
}

