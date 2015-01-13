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
encode_KRB5SignedPathPrincipals(unsigned char *p, size_t len, const KRB5SignedPathPrincipals *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
for(i = (data)->len - 1; i >= 0; --i) {
size_t Top_tag_for_oldret = ret;
ret = 0;
e = encode_Principal(p, len, &(data)->val[i], &l);
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
decode_KRB5SignedPathPrincipals(const unsigned char *p, size_t len, KRB5SignedPathPrincipals *data, size_t *size)
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
e = decode_Principal(p, len, &(data)->val[(data)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
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
free_KRB5SignedPathPrincipals(data);
return e;
}

void
free_KRB5SignedPathPrincipals(KRB5SignedPathPrincipals *data)
{
while((data)->len){
free_Principal(&(data)->val[(data)->len-1]);
(data)->len--;
}
free((data)->val);
(data)->val = NULL;
}

size_t
length_KRB5SignedPathPrincipals(const KRB5SignedPathPrincipals *data)
{
size_t ret = 0;
{
int Top_tag_oldret = ret;
int i;
ret = 0;
for(i = (data)->len - 1; i >= 0; --i){
int Top_tag_for_oldret = ret;
ret = 0;
ret += length_Principal(&(data)->val[i]);
ret += Top_tag_for_oldret;
}
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KRB5SignedPathPrincipals(const KRB5SignedPathPrincipals *from, KRB5SignedPathPrincipals *to)
{
memset(to, 0, sizeof(*to));
if(((to)->val = malloc((from)->len * sizeof(*(to)->val))) == NULL && (from)->len != 0)
goto fail;
for((to)->len = 0; (to)->len < (from)->len; (to)->len++){
if(copy_Principal(&(from)->val[(to)->len], &(to)->val[(to)->len])) goto fail;
}
return 0;
fail:
free_KRB5SignedPathPrincipals(to);
return ENOMEM;
}

int
add_KRB5SignedPathPrincipals(KRB5SignedPathPrincipals *data, const Principal *element)
{
int ret;
void *ptr;

ptr = realloc(data->val, 
	(data->len + 1) * sizeof(data->val[0]));
if (ptr == NULL) return ENOMEM;
data->val = ptr;

ret = copy_Principal(element, &data->val[data->len]);
if (ret) return ret;
data->len++;
return 0;
}

int
remove_KRB5SignedPathPrincipals(KRB5SignedPathPrincipals *data, unsigned int element)
{
void *ptr;

if (data->len == 0 || element >= data->len)
	return ASN1_OVERRUN;
free_Principal(&data->val[element]);
data->len--;
if (element < data->len)
	memmove(&data->val[element], &data->val[element + 1], 
		sizeof(data->val[0]) * data->len);
ptr = realloc(data->val, data->len * sizeof(data->val[0]));
if (ptr) data->val = ptr;
return 0;
}

