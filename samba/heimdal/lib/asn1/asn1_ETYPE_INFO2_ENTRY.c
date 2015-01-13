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
encode_ETYPE_INFO2_ENTRY(unsigned char *p, size_t len, const ETYPE_INFO2_ENTRY *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* s2kparams */
if((data)->s2kparams) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->s2kparams, &l);
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
/* salt */
if((data)->salt) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosString(p, len, (data)->salt, &l);
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
decode_ETYPE_INFO2_ENTRY(const unsigned char *p, size_t len, ETYPE_INFO2_ENTRY *data, size_t *size)
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
e = decode_KerberosString(p, len, (data)->salt, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = salt_oldlen - salt_datalen;
}
}
{
size_t s2kparams_datalen, s2kparams_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &s2kparams_datalen, &l);
if(e) {
(data)->s2kparams = NULL;
} else {
(data)->s2kparams = calloc(1, sizeof(*(data)->s2kparams));
if ((data)->s2kparams == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
s2kparams_oldlen = len;
if (s2kparams_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = s2kparams_datalen;
{
size_t s2kparams_Tag_datalen, s2kparams_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &s2kparams_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
s2kparams_Tag_oldlen = len;
if (s2kparams_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = s2kparams_Tag_datalen;
e = der_get_octet_string(p, len, (data)->s2kparams, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = s2kparams_Tag_oldlen - s2kparams_Tag_datalen;
}
len = s2kparams_oldlen - s2kparams_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_ETYPE_INFO2_ENTRY(data);
return e;
}

void
free_ETYPE_INFO2_ENTRY(ETYPE_INFO2_ENTRY *data)
{
free_ENCTYPE(&(data)->etype);
if((data)->salt) {
free_KerberosString((data)->salt);
free((data)->salt);
(data)->salt = NULL;
}
if((data)->s2kparams) {
der_free_octet_string((data)->s2kparams);
free((data)->s2kparams);
(data)->s2kparams = NULL;
}
}

size_t
length_ETYPE_INFO2_ENTRY(const ETYPE_INFO2_ENTRY *data)
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
ret += length_KerberosString((data)->salt);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->s2kparams){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->s2kparams);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_ETYPE_INFO2_ENTRY(const ETYPE_INFO2_ENTRY *from, ETYPE_INFO2_ENTRY *to)
{
memset(to, 0, sizeof(*to));
if(copy_ENCTYPE(&(from)->etype, &(to)->etype)) goto fail;
if((from)->salt) {
(to)->salt = malloc(sizeof(*(to)->salt));
if((to)->salt == NULL) goto fail;
if(copy_KerberosString((from)->salt, (to)->salt)) goto fail;
}else
(to)->salt = NULL;
if((from)->s2kparams) {
(to)->s2kparams = malloc(sizeof(*(to)->s2kparams));
if((to)->s2kparams == NULL) goto fail;
if(der_copy_octet_string((from)->s2kparams, (to)->s2kparams)) goto fail;
}else
(to)->s2kparams = NULL;
return 0;
fail:
free_ETYPE_INFO2_ENTRY(to);
return ENOMEM;
}

