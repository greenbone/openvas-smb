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
encode_LastReq(unsigned char *p, size_t len, const LastReq *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
for(i = (data)->len - 1; i >= 0; --i) {
size_t Top_tag_for_oldret = ret;
ret = 0;
/* lr-value */
{
size_t Top_tag_S_Of_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(&(data)->val[i])->lr_value, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_S_Of_tag_oldret;
}
/* lr-type */
{
size_t Top_tag_S_Of_tag_oldret = ret;
ret = 0;
e = encode_LR_TYPE(p, len, &(&(data)->val[i])->lr_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_S_Of_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_LastReq(const unsigned char *p, size_t len, LastReq *data, size_t *size)
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
size_t Top_Tag_origlen = len;
size_t Top_Tag_oldret = ret;
void *Top_Tag_tmp;
ret = 0;
(data)->len = 0;
(data)->val = NULL;
while(ret < Top_Tag_origlen) {
Top_Tag_tmp = realloc((data)->val,     sizeof(*((data)->val)) * ((data)->len + 1));
if (Top_Tag_tmp == NULL) { goto fail; }
(data)->val = Top_Tag_tmp;
{
size_t Top_Tag_s_of_datalen, Top_Tag_s_of_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &Top_Tag_s_of_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_Tag_s_of_oldlen = len;
if (Top_Tag_s_of_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_Tag_s_of_datalen;
{
size_t lr_type_datalen, lr_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &lr_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
lr_type_oldlen = len;
if (lr_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = lr_type_datalen;
e = decode_LR_TYPE(p, len, &(&(data)->val[(data)->len])->lr_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = lr_type_oldlen - lr_type_datalen;
}
{
size_t lr_value_datalen, lr_value_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &lr_value_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
lr_value_oldlen = len;
if (lr_value_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = lr_value_datalen;
e = decode_KerberosTime(p, len, &(&(data)->val[(data)->len])->lr_value, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = lr_value_oldlen - lr_value_datalen;
}
len = Top_Tag_s_of_oldlen - Top_Tag_s_of_datalen;
}
(data)->len++;
len = Top_Tag_origlen - ret;
}
ret += Top_Tag_oldret;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_LastReq(data);
return e;
}

void
free_LastReq(LastReq *data)
{
while((data)->len){
free_LR_TYPE(&(&(data)->val[(data)->len-1])->lr_type);
free_KerberosTime(&(&(data)->val[(data)->len-1])->lr_value);
(data)->len--;
}
free((data)->val);
(data)->val = NULL;
}

size_t
length_LastReq(const LastReq *data)
{
size_t ret = 0;
{
int Top_tag_oldret = ret;
int i;
ret = 0;
for(i = (data)->len - 1; i >= 0; --i){
int Top_tag_for_oldret = ret;
ret = 0;
{
size_t Top_tag_S_Of_tag_oldret = ret;
ret = 0;
ret += length_LR_TYPE(&(&(data)->val[i])->lr_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_S_Of_tag_oldret;
}
{
size_t Top_tag_S_Of_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(&(data)->val[i])->lr_value);
ret += 1 + der_length_len (ret);
ret += Top_tag_S_Of_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += Top_tag_for_oldret;
}
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_LastReq(const LastReq *from, LastReq *to)
{
memset(to, 0, sizeof(*to));
if(((to)->val = malloc((from)->len * sizeof(*(to)->val))) == NULL && (from)->len != 0)
goto fail;
for((to)->len = 0; (to)->len < (from)->len; (to)->len++){
if(copy_LR_TYPE(&(&(from)->val[(to)->len])->lr_type, &(&(to)->val[(to)->len])->lr_type)) goto fail;
if(copy_KerberosTime(&(&(from)->val[(to)->len])->lr_value, &(&(to)->val[(to)->len])->lr_value)) goto fail;
}
return 0;
fail:
free_LastReq(to);
return ENOMEM;
}

