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
encode_EncryptedData(unsigned char *p, size_t len, const EncryptedData *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* cipher */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->cipher, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* kvno */
if((data)->kvno) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->kvno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* etype */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_ENCTYPE(p, len, &(data)->etype, &l);
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
decode_EncryptedData(const unsigned char *p, size_t len, EncryptedData *data, size_t *size)
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
size_t etype_datalen, etype_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &etype_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
etype_oldlen = len;
if (etype_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etype_datalen;
e = decode_ENCTYPE(p, len, &(data)->etype, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = etype_oldlen - etype_datalen;
}
{
size_t kvno_datalen, kvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &kvno_datalen, &l);
if(e) {
(data)->kvno = NULL;
} else {
(data)->kvno = calloc(1, sizeof(*(data)->kvno));
if ((data)->kvno == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
kvno_oldlen = len;
if (kvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = kvno_datalen;
e = decode_krb5int32(p, len, (data)->kvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = kvno_oldlen - kvno_datalen;
}
}
{
size_t cipher_datalen, cipher_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &cipher_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cipher_oldlen = len;
if (cipher_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cipher_datalen;
{
size_t cipher_Tag_datalen, cipher_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &cipher_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cipher_Tag_oldlen = len;
if (cipher_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cipher_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->cipher, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cipher_Tag_oldlen - cipher_Tag_datalen;
}
len = cipher_oldlen - cipher_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncryptedData(data);
return e;
}

void
free_EncryptedData(EncryptedData *data)
{
free_ENCTYPE(&(data)->etype);
if((data)->kvno) {
free_krb5int32((data)->kvno);
free((data)->kvno);
(data)->kvno = NULL;
}
der_free_octet_string(&(data)->cipher);
}

size_t
length_EncryptedData(const EncryptedData *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_ENCTYPE(&(data)->etype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->kvno){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->kvno);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->cipher);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncryptedData(const EncryptedData *from, EncryptedData *to)
{
memset(to, 0, sizeof(*to));
if(copy_ENCTYPE(&(from)->etype, &(to)->etype)) goto fail;
if((from)->kvno) {
(to)->kvno = malloc(sizeof(*(to)->kvno));
if((to)->kvno == NULL) goto fail;
if(copy_krb5int32((from)->kvno, (to)->kvno)) goto fail;
}else
(to)->kvno = NULL;
if(der_copy_octet_string(&(from)->cipher, &(to)->cipher)) goto fail;
return 0;
fail:
free_EncryptedData(to);
return ENOMEM;
}

