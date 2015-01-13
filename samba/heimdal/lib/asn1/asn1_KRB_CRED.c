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
encode_KRB_CRED(unsigned char *p, size_t len, const KRB_CRED *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* enc-part */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptedData(p, len, &(data)->enc_part, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* tickets */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
for(i = (&(data)->tickets)->len - 1; i >= 0; --i) {
size_t tickets_tag_tag_for_oldret = ret;
ret = 0;
e = encode_Ticket(p, len, &(&(data)->tickets)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += tickets_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* msg-type */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* pvno */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->pvno, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 22, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_KRB_CRED(const unsigned char *p, size_t len, KRB_CRED *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 22, &Top_datalen, &l);
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
size_t pvno_datalen, pvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &pvno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
pvno_oldlen = len;
if (pvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pvno_datalen;
e = decode_krb5int32(p, len, &(data)->pvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pvno_oldlen - pvno_datalen;
}
{
size_t msg_type_datalen, msg_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &msg_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
msg_type_oldlen = len;
if (msg_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = msg_type_datalen;
e = decode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = msg_type_oldlen - msg_type_datalen;
}
{
size_t tickets_datalen, tickets_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &tickets_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tickets_oldlen = len;
if (tickets_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tickets_datalen;
{
size_t tickets_Tag_datalen, tickets_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &tickets_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tickets_Tag_oldlen = len;
if (tickets_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tickets_Tag_datalen;
{
size_t tickets_Tag_Tag_origlen = len;
size_t tickets_Tag_Tag_oldret = ret;
void *tickets_Tag_Tag_tmp;
ret = 0;
(&(data)->tickets)->len = 0;
(&(data)->tickets)->val = NULL;
while(ret < tickets_Tag_Tag_origlen) {
tickets_Tag_Tag_tmp = realloc((&(data)->tickets)->val,     sizeof(*((&(data)->tickets)->val)) * ((&(data)->tickets)->len + 1));
if (tickets_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->tickets)->val = tickets_Tag_Tag_tmp;
e = decode_Ticket(p, len, &(&(data)->tickets)->val[(&(data)->tickets)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
(&(data)->tickets)->len++;
len = tickets_Tag_Tag_origlen - ret;
}
ret += tickets_Tag_Tag_oldret;
}
len = tickets_Tag_oldlen - tickets_Tag_datalen;
}
len = tickets_oldlen - tickets_datalen;
}
{
size_t enc_part_datalen, enc_part_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &enc_part_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
enc_part_oldlen = len;
if (enc_part_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = enc_part_datalen;
e = decode_EncryptedData(p, len, &(data)->enc_part, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = enc_part_oldlen - enc_part_datalen;
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KRB_CRED(data);
return e;
}

void
free_KRB_CRED(KRB_CRED *data)
{
free_krb5int32(&(data)->pvno);
free_MESSAGE_TYPE(&(data)->msg_type);
while((&(data)->tickets)->len){
free_Ticket(&(&(data)->tickets)->val[(&(data)->tickets)->len-1]);
(&(data)->tickets)->len--;
}
free((&(data)->tickets)->val);
(&(data)->tickets)->val = NULL;
free_EncryptedData(&(data)->enc_part);
}

size_t
length_KRB_CRED(const KRB_CRED *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->pvno);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_MESSAGE_TYPE(&(data)->msg_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
{
int tickets_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->tickets)->len - 1; i >= 0; --i){
int tickets_tag_tag_for_oldret = ret;
ret = 0;
ret += length_Ticket(&(&(data)->tickets)->val[i]);
ret += tickets_tag_tag_for_oldret;
}
ret += tickets_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptedData(&(data)->enc_part);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KRB_CRED(const KRB_CRED *from, KRB_CRED *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->pvno, &(to)->pvno)) goto fail;
if(copy_MESSAGE_TYPE(&(from)->msg_type, &(to)->msg_type)) goto fail;
if(((&(to)->tickets)->val = malloc((&(from)->tickets)->len * sizeof(*(&(to)->tickets)->val))) == NULL && (&(from)->tickets)->len != 0)
goto fail;
for((&(to)->tickets)->len = 0; (&(to)->tickets)->len < (&(from)->tickets)->len; (&(to)->tickets)->len++){
if(copy_Ticket(&(&(from)->tickets)->val[(&(to)->tickets)->len], &(&(to)->tickets)->val[(&(to)->tickets)->len])) goto fail;
}
if(copy_EncryptedData(&(from)->enc_part, &(to)->enc_part)) goto fail;
return 0;
fail:
free_KRB_CRED(to);
return ENOMEM;
}

