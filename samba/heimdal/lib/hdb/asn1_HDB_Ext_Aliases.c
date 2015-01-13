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
encode_HDB_Ext_Aliases(unsigned char *p, size_t len, const HDB_Ext_Aliases *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* aliases */
{
size_t Top_tag_oldret = ret;
ret = 0;
for(i = (&(data)->aliases)->len - 1; i >= 0; --i) {
size_t aliases_tag_tag_for_oldret = ret;
ret = 0;
e = encode_Principal(p, len, &(&(data)->aliases)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += aliases_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* case-insensitive */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_boolean(p, len, &(data)->case_insensitive, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Boolean, &l);
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
decode_HDB_Ext_Aliases(const unsigned char *p, size_t len, HDB_Ext_Aliases *data, size_t *size)
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
size_t case_insensitive_datalen, case_insensitive_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &case_insensitive_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
case_insensitive_oldlen = len;
if (case_insensitive_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = case_insensitive_datalen;
{
size_t case_insensitive_Tag_datalen, case_insensitive_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Boolean, &case_insensitive_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
case_insensitive_Tag_oldlen = len;
if (case_insensitive_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = case_insensitive_Tag_datalen;
e = der_get_boolean(p, len, &(data)->case_insensitive, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = case_insensitive_Tag_oldlen - case_insensitive_Tag_datalen;
}
len = case_insensitive_oldlen - case_insensitive_datalen;
}
{
size_t aliases_datalen, aliases_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &aliases_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
aliases_oldlen = len;
if (aliases_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = aliases_datalen;
{
size_t aliases_Tag_datalen, aliases_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &aliases_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
aliases_Tag_oldlen = len;
if (aliases_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = aliases_Tag_datalen;
{
size_t aliases_Tag_Tag_origlen = len;
size_t aliases_Tag_Tag_oldret = ret;
void *aliases_Tag_Tag_tmp;
ret = 0;
(&(data)->aliases)->len = 0;
(&(data)->aliases)->val = NULL;
while(ret < aliases_Tag_Tag_origlen) {
aliases_Tag_Tag_tmp = realloc((&(data)->aliases)->val,     sizeof(*((&(data)->aliases)->val)) * ((&(data)->aliases)->len + 1));
if (aliases_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->aliases)->val = aliases_Tag_Tag_tmp;
e = decode_Principal(p, len, &(&(data)->aliases)->val[(&(data)->aliases)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
(&(data)->aliases)->len++;
len = aliases_Tag_Tag_origlen - ret;
}
ret += aliases_Tag_Tag_oldret;
}
len = aliases_Tag_oldlen - aliases_Tag_datalen;
}
len = aliases_oldlen - aliases_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_HDB_Ext_Aliases(data);
return e;
}

void
free_HDB_Ext_Aliases(HDB_Ext_Aliases *data)
{
while((&(data)->aliases)->len){
free_Principal(&(&(data)->aliases)->val[(&(data)->aliases)->len-1]);
(&(data)->aliases)->len--;
}
free((&(data)->aliases)->val);
(&(data)->aliases)->val = NULL;
}

size_t
length_HDB_Ext_Aliases(const HDB_Ext_Aliases *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += 1;
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
{
int aliases_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->aliases)->len - 1; i >= 0; --i){
int aliases_tag_tag_for_oldret = ret;
ret = 0;
ret += length_Principal(&(&(data)->aliases)->val[i]);
ret += aliases_tag_tag_for_oldret;
}
ret += aliases_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_HDB_Ext_Aliases(const HDB_Ext_Aliases *from, HDB_Ext_Aliases *to)
{
memset(to, 0, sizeof(*to));
*(&(to)->case_insensitive) = *(&(from)->case_insensitive);
if(((&(to)->aliases)->val = malloc((&(from)->aliases)->len * sizeof(*(&(to)->aliases)->val))) == NULL && (&(from)->aliases)->len != 0)
goto fail;
for((&(to)->aliases)->len = 0; (&(to)->aliases)->len < (&(from)->aliases)->len; (&(to)->aliases)->len++){
if(copy_Principal(&(&(from)->aliases)->val[(&(to)->aliases)->len], &(&(to)->aliases)->val[(&(to)->aliases)->len])) goto fail;
}
return 0;
fail:
free_HDB_Ext_Aliases(to);
return ENOMEM;
}

