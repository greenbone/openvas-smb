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
encode_ETYPE_INFO_ENTRY(unsigned char *p, size_t len, const ETYPE_INFO_ENTRY *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* salttype */
if((data)->salttype) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->salttype, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* salt */
if((data)->salt) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->salt, &l);
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
decode_ETYPE_INFO_ENTRY(const unsigned char *p, size_t len, ETYPE_INFO_ENTRY *data, size_t *size)
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
size_t salt_datalen, salt_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &salt_datalen, &l);
if(e) {
(data)->salt = NULL;
} else {
(data)->salt = calloc(1, sizeof(*(data)->salt));
if ((data)->salt == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
salt_oldlen = len;
if (salt_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = salt_datalen;
{
size_t salt_Tag_datalen, salt_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &salt_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
salt_Tag_oldlen = len;
if (salt_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = salt_Tag_datalen;
e = der_get_octet_string(p, len, (data)->salt, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = salt_Tag_oldlen - salt_Tag_datalen;
}
len = salt_oldlen - salt_datalen;
}
}
{
size_t salttype_datalen, salttype_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &salttype_datalen, &l);
if(e) {
(data)->salttype = NULL;
} else {
(data)->salttype = calloc(1, sizeof(*(data)->salttype));
if ((data)->salttype == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
salttype_oldlen = len;
if (salttype_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = salttype_datalen;
e = decode_krb5int32(p, len, (data)->salttype, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = salttype_oldlen - salttype_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_ETYPE_INFO_ENTRY(data);
return e;
}

void
free_ETYPE_INFO_ENTRY(ETYPE_INFO_ENTRY *data)
{
free_ENCTYPE(&(data)->etype);
if((data)->salt) {
der_free_octet_string((data)->salt);
free((data)->salt);
(data)->salt = NULL;
}
if((data)->salttype) {
free_krb5int32((data)->salttype);
free((data)->salttype);
(data)->salttype = NULL;
}
}

size_t
length_ETYPE_INFO_ENTRY(const ETYPE_INFO_ENTRY *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_ENCTYPE(&(data)->etype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->salt){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->salt);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->salttype){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->salttype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_ETYPE_INFO_ENTRY(const ETYPE_INFO_ENTRY *from, ETYPE_INFO_ENTRY *to)
{
memset(to, 0, sizeof(*to));
if(copy_ENCTYPE(&(from)->etype, &(to)->etype)) goto fail;
if((from)->salt) {
(to)->salt = malloc(sizeof(*(to)->salt));
if((to)->salt == NULL) goto fail;
if(der_copy_octet_string((from)->salt, (to)->salt)) goto fail;
}else
(to)->salt = NULL;
if((from)->salttype) {
(to)->salttype = malloc(sizeof(*(to)->salttype));
if((to)->salttype == NULL) goto fail;
if(copy_krb5int32((from)->salttype, (to)->salttype)) goto fail;
}else
(to)->salttype = NULL;
return 0;
fail:
free_ETYPE_INFO_ENTRY(to);
return ENOMEM;
}

