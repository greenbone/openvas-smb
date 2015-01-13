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
encode_EncryptionKey(unsigned char *p, size_t len, const EncryptionKey *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* keyvalue */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->keyvalue, &l);
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
/* keytype */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->keytype, &l);
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
decode_EncryptionKey(const unsigned char *p, size_t len, EncryptionKey *data, size_t *size)
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
size_t keytype_datalen, keytype_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &keytype_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
keytype_oldlen = len;
if (keytype_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = keytype_datalen;
e = decode_krb5int32(p, len, &(data)->keytype, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = keytype_oldlen - keytype_datalen;
}
{
size_t keyvalue_datalen, keyvalue_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &keyvalue_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
keyvalue_oldlen = len;
if (keyvalue_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = keyvalue_datalen;
{
size_t keyvalue_Tag_datalen, keyvalue_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &keyvalue_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
keyvalue_Tag_oldlen = len;
if (keyvalue_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = keyvalue_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->keyvalue, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = keyvalue_Tag_oldlen - keyvalue_Tag_datalen;
}
len = keyvalue_oldlen - keyvalue_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncryptionKey(data);
return e;
}

void
free_EncryptionKey(EncryptionKey *data)
{
free_krb5int32(&(data)->keytype);
der_free_octet_string(&(data)->keyvalue);
}

size_t
length_EncryptionKey(const EncryptionKey *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->keytype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->keyvalue);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncryptionKey(const EncryptionKey *from, EncryptionKey *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->keytype, &(to)->keytype)) goto fail;
if(der_copy_octet_string(&(from)->keyvalue, &(to)->keyvalue)) goto fail;
return 0;
fail:
free_EncryptionKey(to);
return ENOMEM;
}

