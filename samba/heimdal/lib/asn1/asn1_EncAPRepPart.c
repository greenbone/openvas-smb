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
encode_EncAPRepPart(unsigned char *p, size_t len, const EncAPRepPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* seq-number */
if((data)->seq_number) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5uint32(p, len, (data)->seq_number, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* subkey */
if((data)->subkey) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptionKey(p, len, (data)->subkey, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cusec */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->cusec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* ctime */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->ctime, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 27, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_EncAPRepPart(const unsigned char *p, size_t len, EncAPRepPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 27, &Top_datalen, &l);
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
size_t ctime_datalen, ctime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &ctime_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ctime_oldlen = len;
if (ctime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ctime_datalen;
e = decode_KerberosTime(p, len, &(data)->ctime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = ctime_oldlen - ctime_datalen;
}
{
size_t cusec_datalen, cusec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &cusec_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cusec_oldlen = len;
if (cusec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cusec_datalen;
e = decode_krb5int32(p, len, &(data)->cusec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cusec_oldlen - cusec_datalen;
}
{
size_t subkey_datalen, subkey_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &subkey_datalen, &l);
if(e) {
(data)->subkey = NULL;
} else {
(data)->subkey = calloc(1, sizeof(*(data)->subkey));
if ((data)->subkey == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
subkey_oldlen = len;
if (subkey_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = subkey_datalen;
e = decode_EncryptionKey(p, len, (data)->subkey, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = subkey_oldlen - subkey_datalen;
}
}
{
size_t seq_number_datalen, seq_number_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &seq_number_datalen, &l);
if(e) {
(data)->seq_number = NULL;
} else {
(data)->seq_number = calloc(1, sizeof(*(data)->seq_number));
if ((data)->seq_number == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
seq_number_oldlen = len;
if (seq_number_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = seq_number_datalen;
e = decode_krb5uint32(p, len, (data)->seq_number, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = seq_number_oldlen - seq_number_datalen;
}
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncAPRepPart(data);
return e;
}

void
free_EncAPRepPart(EncAPRepPart *data)
{
free_KerberosTime(&(data)->ctime);
free_krb5int32(&(data)->cusec);
if((data)->subkey) {
free_EncryptionKey((data)->subkey);
free((data)->subkey);
(data)->subkey = NULL;
}
if((data)->seq_number) {
free_krb5uint32((data)->seq_number);
free((data)->seq_number);
(data)->seq_number = NULL;
}
}

size_t
length_EncAPRepPart(const EncAPRepPart *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->ctime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->cusec);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->subkey){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey((data)->subkey);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->seq_number){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5uint32((data)->seq_number);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncAPRepPart(const EncAPRepPart *from, EncAPRepPart *to)
{
memset(to, 0, sizeof(*to));
if(copy_KerberosTime(&(from)->ctime, &(to)->ctime)) goto fail;
if(copy_krb5int32(&(from)->cusec, &(to)->cusec)) goto fail;
if((from)->subkey) {
(to)->subkey = malloc(sizeof(*(to)->subkey));
if((to)->subkey == NULL) goto fail;
if(copy_EncryptionKey((from)->subkey, (to)->subkey)) goto fail;
}else
(to)->subkey = NULL;
if((from)->seq_number) {
(to)->seq_number = malloc(sizeof(*(to)->seq_number));
if((to)->seq_number == NULL) goto fail;
if(copy_krb5uint32((from)->seq_number, (to)->seq_number)) goto fail;
}else
(to)->seq_number = NULL;
return 0;
fail:
free_EncAPRepPart(to);
return ENOMEM;
}

