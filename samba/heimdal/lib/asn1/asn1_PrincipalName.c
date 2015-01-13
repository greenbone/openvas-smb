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
encode_PrincipalName(unsigned char *p, size_t len, const PrincipalName *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* name-string */
{
size_t Top_tag_oldret = ret;
ret = 0;
for(i = (&(data)->name_string)->len - 1; i >= 0; --i) {
size_t name_string_tag_tag_for_oldret = ret;
ret = 0;
e = der_put_general_string(p, len, &(&(data)->name_string)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_GeneralString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += name_string_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* name-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_NAME_TYPE(p, len, &(data)->name_type, &l);
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
decode_PrincipalName(const unsigned char *p, size_t len, PrincipalName *data, size_t *size)
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
size_t name_type_datalen, name_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &name_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
name_type_oldlen = len;
if (name_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = name_type_datalen;
e = decode_NAME_TYPE(p, len, &(data)->name_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = name_type_oldlen - name_type_datalen;
}
{
size_t name_string_datalen, name_string_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &name_string_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
name_string_oldlen = len;
if (name_string_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = name_string_datalen;
{
size_t name_string_Tag_datalen, name_string_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &name_string_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
name_string_Tag_oldlen = len;
if (name_string_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = name_string_Tag_datalen;
{
size_t name_string_Tag_Tag_origlen = len;
size_t name_string_Tag_Tag_oldret = ret;
void *name_string_Tag_Tag_tmp;
ret = 0;
(&(data)->name_string)->len = 0;
(&(data)->name_string)->val = NULL;
while(ret < name_string_Tag_Tag_origlen) {
name_string_Tag_Tag_tmp = realloc((&(data)->name_string)->val,     sizeof(*((&(data)->name_string)->val)) * ((&(data)->name_string)->len + 1));
if (name_string_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->name_string)->val = name_string_Tag_Tag_tmp;
{
size_t name_string_Tag_Tag_s_of_datalen, name_string_Tag_Tag_s_of_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_GeneralString, &name_string_Tag_Tag_s_of_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
name_string_Tag_Tag_s_of_oldlen = len;
if (name_string_Tag_Tag_s_of_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = name_string_Tag_Tag_s_of_datalen;
e = der_get_general_string(p, len, &(&(data)->name_string)->val[(&(data)->name_string)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = name_string_Tag_Tag_s_of_oldlen - name_string_Tag_Tag_s_of_datalen;
}
(&(data)->name_string)->len++;
len = name_string_Tag_Tag_origlen - ret;
}
ret += name_string_Tag_Tag_oldret;
}
len = name_string_Tag_oldlen - name_string_Tag_datalen;
}
len = name_string_oldlen - name_string_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_PrincipalName(data);
return e;
}

void
free_PrincipalName(PrincipalName *data)
{
free_NAME_TYPE(&(data)->name_type);
while((&(data)->name_string)->len){
der_free_general_string(&(&(data)->name_string)->val[(&(data)->name_string)->len-1]);
(&(data)->name_string)->len--;
}
free((&(data)->name_string)->val);
(&(data)->name_string)->val = NULL;
}

size_t
length_PrincipalName(const PrincipalName *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_NAME_TYPE(&(data)->name_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
{
int name_string_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->name_string)->len - 1; i >= 0; --i){
int name_string_tag_tag_for_oldret = ret;
ret = 0;
ret += der_length_general_string(&(&(data)->name_string)->val[i]);
ret += 1 + der_length_len (ret);
ret += name_string_tag_tag_for_oldret;
}
ret += name_string_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_PrincipalName(const PrincipalName *from, PrincipalName *to)
{
memset(to, 0, sizeof(*to));
if(copy_NAME_TYPE(&(from)->name_type, &(to)->name_type)) goto fail;
if(((&(to)->name_string)->val = malloc((&(from)->name_string)->len * sizeof(*(&(to)->name_string)->val))) == NULL && (&(from)->name_string)->len != 0)
goto fail;
for((&(to)->name_string)->len = 0; (&(to)->name_string)->len < (&(from)->name_string)->len; (&(to)->name_string)->len++){
if(der_copy_general_string(&(&(from)->name_string)->val[(&(to)->name_string)->len], &(&(to)->name_string)->val[(&(to)->name_string)->len])) goto fail;
}
return 0;
fail:
free_PrincipalName(to);
return ENOMEM;
}

