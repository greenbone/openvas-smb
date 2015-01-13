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
encode_hdb_entry(unsigned char *p, size_t len, const hdb_entry *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* extensions */
if((data)->extensions) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_HDB_extensions(p, len, (data)->extensions, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 13, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* generation */
if((data)->generation) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_GENERATION(p, len, (data)->generation, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 12, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* etypes */
if((data)->etypes) {
size_t Top_tag_oldret = ret;
ret = 0;
for(i = ((data)->etypes)->len - 1; i >= 0; --i) {
size_t etypes_tag_tag_for_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, &((data)->etypes)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += etypes_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 11, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* flags */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_HDBFlags(p, len, &(data)->flags, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* max-renew */
if((data)->max_renew) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, (data)->max_renew, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* max-life */
if((data)->max_life) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, (data)->max_life, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* pw-end */
if((data)->pw_end) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->pw_end, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* valid-end */
if((data)->valid_end) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->valid_end, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* valid-start */
if((data)->valid_start) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->valid_start, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* modified-by */
if((data)->modified_by) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Event(p, len, (data)->modified_by, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* created-by */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Event(p, len, &(data)->created_by, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* keys */
{
size_t Top_tag_oldret = ret;
ret = 0;
for(i = (&(data)->keys)->len - 1; i >= 0; --i) {
size_t keys_tag_tag_for_oldret = ret;
ret = 0;
e = encode_Key(p, len, &(&(data)->keys)->val[i], &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += keys_tag_tag_for_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* kvno */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, &(data)->kvno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* principal */
if((data)->principal) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Principal(p, len, (data)->principal, &l);
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
decode_hdb_entry(const unsigned char *p, size_t len, hdb_entry *data, size_t *size)
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
size_t principal_datalen, principal_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &principal_datalen, &l);
if(e) {
(data)->principal = NULL;
} else {
(data)->principal = calloc(1, sizeof(*(data)->principal));
if ((data)->principal == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
principal_oldlen = len;
if (principal_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = principal_datalen;
e = decode_Principal(p, len, (data)->principal, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = principal_oldlen - principal_datalen;
}
}
{
size_t kvno_datalen, kvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &kvno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
kvno_oldlen = len;
if (kvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = kvno_datalen;
{
size_t kvno_Tag_datalen, kvno_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &kvno_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
kvno_Tag_oldlen = len;
if (kvno_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = kvno_Tag_datalen;
e = der_get_unsigned(p, len, &(data)->kvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = kvno_Tag_oldlen - kvno_Tag_datalen;
}
len = kvno_oldlen - kvno_datalen;
}
{
size_t keys_datalen, keys_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &keys_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
keys_oldlen = len;
if (keys_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = keys_datalen;
{
size_t keys_Tag_datalen, keys_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &keys_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
keys_Tag_oldlen = len;
if (keys_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = keys_Tag_datalen;
{
size_t keys_Tag_Tag_origlen = len;
size_t keys_Tag_Tag_oldret = ret;
void *keys_Tag_Tag_tmp;
ret = 0;
(&(data)->keys)->len = 0;
(&(data)->keys)->val = NULL;
while(ret < keys_Tag_Tag_origlen) {
keys_Tag_Tag_tmp = realloc((&(data)->keys)->val,     sizeof(*((&(data)->keys)->val)) * ((&(data)->keys)->len + 1));
if (keys_Tag_Tag_tmp == NULL) { goto fail; }
(&(data)->keys)->val = keys_Tag_Tag_tmp;
e = decode_Key(p, len, &(&(data)->keys)->val[(&(data)->keys)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
(&(data)->keys)->len++;
len = keys_Tag_Tag_origlen - ret;
}
ret += keys_Tag_Tag_oldret;
}
len = keys_Tag_oldlen - keys_Tag_datalen;
}
len = keys_oldlen - keys_datalen;
}
{
size_t created_by_datalen, created_by_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &created_by_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
created_by_oldlen = len;
if (created_by_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = created_by_datalen;
e = decode_Event(p, len, &(data)->created_by, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = created_by_oldlen - created_by_datalen;
}
{
size_t modified_by_datalen, modified_by_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &modified_by_datalen, &l);
if(e) {
(data)->modified_by = NULL;
} else {
(data)->modified_by = calloc(1, sizeof(*(data)->modified_by));
if ((data)->modified_by == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
modified_by_oldlen = len;
if (modified_by_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = modified_by_datalen;
e = decode_Event(p, len, (data)->modified_by, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = modified_by_oldlen - modified_by_datalen;
}
}
{
size_t valid_start_datalen, valid_start_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &valid_start_datalen, &l);
if(e) {
(data)->valid_start = NULL;
} else {
(data)->valid_start = calloc(1, sizeof(*(data)->valid_start));
if ((data)->valid_start == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
valid_start_oldlen = len;
if (valid_start_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = valid_start_datalen;
e = decode_KerberosTime(p, len, (data)->valid_start, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = valid_start_oldlen - valid_start_datalen;
}
}
{
size_t valid_end_datalen, valid_end_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &valid_end_datalen, &l);
if(e) {
(data)->valid_end = NULL;
} else {
(data)->valid_end = calloc(1, sizeof(*(data)->valid_end));
if ((data)->valid_end == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
valid_end_oldlen = len;
if (valid_end_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = valid_end_datalen;
e = decode_KerberosTime(p, len, (data)->valid_end, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = valid_end_oldlen - valid_end_datalen;
}
}
{
size_t pw_end_datalen, pw_end_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &pw_end_datalen, &l);
if(e) {
(data)->pw_end = NULL;
} else {
(data)->pw_end = calloc(1, sizeof(*(data)->pw_end));
if ((data)->pw_end == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
pw_end_oldlen = len;
if (pw_end_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pw_end_datalen;
e = decode_KerberosTime(p, len, (data)->pw_end, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pw_end_oldlen - pw_end_datalen;
}
}
{
size_t max_life_datalen, max_life_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &max_life_datalen, &l);
if(e) {
(data)->max_life = NULL;
} else {
(data)->max_life = calloc(1, sizeof(*(data)->max_life));
if ((data)->max_life == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
max_life_oldlen = len;
if (max_life_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = max_life_datalen;
{
size_t max_life_Tag_datalen, max_life_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &max_life_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
max_life_Tag_oldlen = len;
if (max_life_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = max_life_Tag_datalen;
e = der_get_unsigned(p, len, (data)->max_life, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = max_life_Tag_oldlen - max_life_Tag_datalen;
}
len = max_life_oldlen - max_life_datalen;
}
}
{
size_t max_renew_datalen, max_renew_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &max_renew_datalen, &l);
if(e) {
(data)->max_renew = NULL;
} else {
(data)->max_renew = calloc(1, sizeof(*(data)->max_renew));
if ((data)->max_renew == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
max_renew_oldlen = len;
if (max_renew_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = max_renew_datalen;
{
size_t max_renew_Tag_datalen, max_renew_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &max_renew_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
max_renew_Tag_oldlen = len;
if (max_renew_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = max_renew_Tag_datalen;
e = der_get_unsigned(p, len, (data)->max_renew, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = max_renew_Tag_oldlen - max_renew_Tag_datalen;
}
len = max_renew_oldlen - max_renew_datalen;
}
}
{
size_t flags_datalen, flags_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &flags_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
flags_oldlen = len;
if (flags_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = flags_datalen;
e = decode_HDBFlags(p, len, &(data)->flags, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = flags_oldlen - flags_datalen;
}
{
size_t etypes_datalen, etypes_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 11, &etypes_datalen, &l);
if(e) {
(data)->etypes = NULL;
} else {
(data)->etypes = calloc(1, sizeof(*(data)->etypes));
if ((data)->etypes == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
etypes_oldlen = len;
if (etypes_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etypes_datalen;
{
size_t etypes_Tag_datalen, etypes_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &etypes_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
etypes_Tag_oldlen = len;
if (etypes_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etypes_Tag_datalen;
{
size_t etypes_Tag_Tag_origlen = len;
size_t etypes_Tag_Tag_oldret = ret;
void *etypes_Tag_Tag_tmp;
ret = 0;
((data)->etypes)->len = 0;
((data)->etypes)->val = NULL;
while(ret < etypes_Tag_Tag_origlen) {
etypes_Tag_Tag_tmp = realloc(((data)->etypes)->val,     sizeof(*(((data)->etypes)->val)) * (((data)->etypes)->len + 1));
if (etypes_Tag_Tag_tmp == NULL) { goto fail; }
((data)->etypes)->val = etypes_Tag_Tag_tmp;
{
size_t etypes_Tag_Tag_s_of_datalen, etypes_Tag_Tag_s_of_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &etypes_Tag_Tag_s_of_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
etypes_Tag_Tag_s_of_oldlen = len;
if (etypes_Tag_Tag_s_of_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = etypes_Tag_Tag_s_of_datalen;
e = der_get_unsigned(p, len, &((data)->etypes)->val[((data)->etypes)->len], &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = etypes_Tag_Tag_s_of_oldlen - etypes_Tag_Tag_s_of_datalen;
}
((data)->etypes)->len++;
len = etypes_Tag_Tag_origlen - ret;
}
ret += etypes_Tag_Tag_oldret;
}
len = etypes_Tag_oldlen - etypes_Tag_datalen;
}
len = etypes_oldlen - etypes_datalen;
}
}
{
size_t generation_datalen, generation_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 12, &generation_datalen, &l);
if(e) {
(data)->generation = NULL;
} else {
(data)->generation = calloc(1, sizeof(*(data)->generation));
if ((data)->generation == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
generation_oldlen = len;
if (generation_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = generation_datalen;
e = decode_GENERATION(p, len, (data)->generation, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = generation_oldlen - generation_datalen;
}
}
{
size_t extensions_datalen, extensions_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 13, &extensions_datalen, &l);
if(e) {
(data)->extensions = NULL;
} else {
(data)->extensions = calloc(1, sizeof(*(data)->extensions));
if ((data)->extensions == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
extensions_oldlen = len;
if (extensions_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = extensions_datalen;
e = decode_HDB_extensions(p, len, (data)->extensions, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = extensions_oldlen - extensions_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_hdb_entry(data);
return e;
}

void
free_hdb_entry(hdb_entry *data)
{
if((data)->principal) {
free_Principal((data)->principal);
free((data)->principal);
(data)->principal = NULL;
}
while((&(data)->keys)->len){
free_Key(&(&(data)->keys)->val[(&(data)->keys)->len-1]);
(&(data)->keys)->len--;
}
free((&(data)->keys)->val);
(&(data)->keys)->val = NULL;
free_Event(&(data)->created_by);
if((data)->modified_by) {
free_Event((data)->modified_by);
free((data)->modified_by);
(data)->modified_by = NULL;
}
if((data)->valid_start) {
free_KerberosTime((data)->valid_start);
free((data)->valid_start);
(data)->valid_start = NULL;
}
if((data)->valid_end) {
free_KerberosTime((data)->valid_end);
free((data)->valid_end);
(data)->valid_end = NULL;
}
if((data)->pw_end) {
free_KerberosTime((data)->pw_end);
free((data)->pw_end);
(data)->pw_end = NULL;
}
if((data)->max_life) {
free((data)->max_life);
(data)->max_life = NULL;
}
if((data)->max_renew) {
free((data)->max_renew);
(data)->max_renew = NULL;
}
free_HDBFlags(&(data)->flags);
if((data)->etypes) {
while(((data)->etypes)->len){
((data)->etypes)->len--;
}
free(((data)->etypes)->val);
((data)->etypes)->val = NULL;
free((data)->etypes);
(data)->etypes = NULL;
}
if((data)->generation) {
free_GENERATION((data)->generation);
free((data)->generation);
(data)->generation = NULL;
}
if((data)->extensions) {
free_HDB_extensions((data)->extensions);
free((data)->extensions);
(data)->extensions = NULL;
}
}

size_t
length_hdb_entry(const hdb_entry *data)
{
size_t ret = 0;
if((data)->principal){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Principal((data)->principal);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned(&(data)->kvno);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
{
int keys_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = (&(data)->keys)->len - 1; i >= 0; --i){
int keys_tag_tag_for_oldret = ret;
ret = 0;
ret += length_Key(&(&(data)->keys)->val[i]);
ret += keys_tag_tag_for_oldret;
}
ret += keys_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Event(&(data)->created_by);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->modified_by){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Event((data)->modified_by);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->valid_start){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->valid_start);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->valid_end){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->valid_end);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->pw_end){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->pw_end);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->max_life){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned((data)->max_life);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->max_renew){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned((data)->max_renew);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_HDBFlags(&(data)->flags);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->etypes){
size_t Top_tag_oldret = ret;
ret = 0;
{
int etypes_tag_tag_oldret = ret;
int i;
ret = 0;
for(i = ((data)->etypes)->len - 1; i >= 0; --i){
int etypes_tag_tag_for_oldret = ret;
ret = 0;
ret += der_length_unsigned(&((data)->etypes)->val[i]);
ret += 1 + der_length_len (ret);
ret += etypes_tag_tag_for_oldret;
}
ret += etypes_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->generation){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_GENERATION((data)->generation);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->extensions){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_HDB_extensions((data)->extensions);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_hdb_entry(const hdb_entry *from, hdb_entry *to)
{
memset(to, 0, sizeof(*to));
if((from)->principal) {
(to)->principal = malloc(sizeof(*(to)->principal));
if((to)->principal == NULL) goto fail;
if(copy_Principal((from)->principal, (to)->principal)) goto fail;
}else
(to)->principal = NULL;
*(&(to)->kvno) = *(&(from)->kvno);
if(((&(to)->keys)->val = malloc((&(from)->keys)->len * sizeof(*(&(to)->keys)->val))) == NULL && (&(from)->keys)->len != 0)
goto fail;
for((&(to)->keys)->len = 0; (&(to)->keys)->len < (&(from)->keys)->len; (&(to)->keys)->len++){
if(copy_Key(&(&(from)->keys)->val[(&(to)->keys)->len], &(&(to)->keys)->val[(&(to)->keys)->len])) goto fail;
}
if(copy_Event(&(from)->created_by, &(to)->created_by)) goto fail;
if((from)->modified_by) {
(to)->modified_by = malloc(sizeof(*(to)->modified_by));
if((to)->modified_by == NULL) goto fail;
if(copy_Event((from)->modified_by, (to)->modified_by)) goto fail;
}else
(to)->modified_by = NULL;
if((from)->valid_start) {
(to)->valid_start = malloc(sizeof(*(to)->valid_start));
if((to)->valid_start == NULL) goto fail;
if(copy_KerberosTime((from)->valid_start, (to)->valid_start)) goto fail;
}else
(to)->valid_start = NULL;
if((from)->valid_end) {
(to)->valid_end = malloc(sizeof(*(to)->valid_end));
if((to)->valid_end == NULL) goto fail;
if(copy_KerberosTime((from)->valid_end, (to)->valid_end)) goto fail;
}else
(to)->valid_end = NULL;
if((from)->pw_end) {
(to)->pw_end = malloc(sizeof(*(to)->pw_end));
if((to)->pw_end == NULL) goto fail;
if(copy_KerberosTime((from)->pw_end, (to)->pw_end)) goto fail;
}else
(to)->pw_end = NULL;
if((from)->max_life) {
(to)->max_life = malloc(sizeof(*(to)->max_life));
if((to)->max_life == NULL) goto fail;
*((to)->max_life) = *((from)->max_life);
}else
(to)->max_life = NULL;
if((from)->max_renew) {
(to)->max_renew = malloc(sizeof(*(to)->max_renew));
if((to)->max_renew == NULL) goto fail;
*((to)->max_renew) = *((from)->max_renew);
}else
(to)->max_renew = NULL;
if(copy_HDBFlags(&(from)->flags, &(to)->flags)) goto fail;
if((from)->etypes) {
(to)->etypes = malloc(sizeof(*(to)->etypes));
if((to)->etypes == NULL) goto fail;
if((((to)->etypes)->val = malloc(((from)->etypes)->len * sizeof(*((to)->etypes)->val))) == NULL && ((from)->etypes)->len != 0)
goto fail;
for(((to)->etypes)->len = 0; ((to)->etypes)->len < ((from)->etypes)->len; ((to)->etypes)->len++){
*(&((to)->etypes)->val[((to)->etypes)->len]) = *(&((from)->etypes)->val[((to)->etypes)->len]);
}
}else
(to)->etypes = NULL;
if((from)->generation) {
(to)->generation = malloc(sizeof(*(to)->generation));
if((to)->generation == NULL) goto fail;
if(copy_GENERATION((from)->generation, (to)->generation)) goto fail;
}else
(to)->generation = NULL;
if((from)->extensions) {
(to)->extensions = malloc(sizeof(*(to)->extensions));
if((to)->extensions == NULL) goto fail;
if(copy_HDB_extensions((from)->extensions, (to)->extensions)) goto fail;
}else
(to)->extensions = NULL;
return 0;
fail:
free_hdb_entry(to);
return ENOMEM;
}

