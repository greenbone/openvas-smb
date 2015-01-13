/* Generated from hdb.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <hdb_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_Key(unsigned char *p, size_t len, const Key *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* salt */
if((data)->salt) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Salt(p, len, (data)->salt, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* key */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_EncryptionKey(p, len, &(data)->key, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* mkvno */
if((data)->mkvno) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, (data)->mkvno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
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
decode_Key(const unsigned char *p, size_t len, Key *data, size_t *size)
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
size_t mkvno_datalen, mkvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &mkvno_datalen, &l);
if(e) {
(data)->mkvno = NULL;
} else {
(data)->mkvno = calloc(1, sizeof(*(data)->mkvno));
if ((data)->mkvno == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
mkvno_oldlen = len;
if (mkvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mkvno_datalen;
{
size_t mkvno_Tag_datalen, mkvno_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &mkvno_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
mkvno_Tag_oldlen = len;
if (mkvno_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = mkvno_Tag_datalen;
e = der_get_unsigned(p, len, (data)->mkvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = mkvno_Tag_oldlen - mkvno_Tag_datalen;
}
len = mkvno_oldlen - mkvno_datalen;
}
}
{
size_t key_datalen, key_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &key_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
key_oldlen = len;
if (key_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = key_datalen;
e = decode_EncryptionKey(p, len, &(data)->key, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = key_oldlen - key_datalen;
}
{
size_t salt_datalen, salt_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &salt_datalen, &l);
if(e) {
(data)->salt = NULL;
} else {
(data)->salt = calloc(1, sizeof(*(data)->salt));
if ((data)->salt == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
salt_oldlen = len;
if (salt_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = salt_datalen;
e = decode_Salt(p, len, (data)->salt, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = salt_oldlen - salt_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Key(data);
return e;
}

void
free_Key(Key *data)
{
if((data)->mkvno) {
free((data)->mkvno);
(data)->mkvno = NULL;
}
free_EncryptionKey(&(data)->key);
if((data)->salt) {
free_Salt((data)->salt);
free((data)->salt);
(data)->salt = NULL;
}
}

size_t
length_Key(const Key *data)
{
size_t ret = 0;
if((data)->mkvno){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned((data)->mkvno);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey(&(data)->key);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->salt){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Salt((data)->salt);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Key(const Key *from, Key *to)
{
memset(to, 0, sizeof(*to));
if((from)->mkvno) {
(to)->mkvno = malloc(sizeof(*(to)->mkvno));
if((to)->mkvno == NULL) goto fail;
*((to)->mkvno) = *((from)->mkvno);
}else
(to)->mkvno = NULL;
if(copy_EncryptionKey(&(from)->key, &(to)->key)) goto fail;
if((from)->salt) {
(to)->salt = malloc(sizeof(*(to)->salt));
if((to)->salt == NULL) goto fail;
if(copy_Salt((from)->salt, (to)->salt)) goto fail;
}else
(to)->salt = NULL;
return 0;
fail:
free_Key(to);
return ENOMEM;
}

