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
encode_PA_DATA(unsigned char *p, size_t len, const PA_DATA *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* padata-value */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->padata_value, &l);
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
/* padata-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PADATA_TYPE(p, len, &(data)->padata_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
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
decode_PA_DATA(const unsigned char *p, size_t len, PA_DATA *data, size_t *size)
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
size_t padata_type_datalen, padata_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &padata_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
padata_type_oldlen = len;
if (padata_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = padata_type_datalen;
e = decode_PADATA_TYPE(p, len, &(data)->padata_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = padata_type_oldlen - padata_type_datalen;
}
{
size_t padata_value_datalen, padata_value_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &padata_value_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
padata_value_oldlen = len;
if (padata_value_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = padata_value_datalen;
{
size_t padata_value_Tag_datalen, padata_value_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &padata_value_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
padata_value_Tag_oldlen = len;
if (padata_value_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = padata_value_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->padata_value, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = padata_value_Tag_oldlen - padata_value_Tag_datalen;
}
len = padata_value_oldlen - padata_value_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_PA_DATA(data);
return e;
}

void
free_PA_DATA(PA_DATA *data)
{
free_PADATA_TYPE(&(data)->padata_type);
der_free_octet_string(&(data)->padata_value);
}

size_t
length_PA_DATA(const PA_DATA *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PADATA_TYPE(&(data)->padata_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->padata_value);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_PA_DATA(const PA_DATA *from, PA_DATA *to)
{
memset(to, 0, sizeof(*to));
if(copy_PADATA_TYPE(&(from)->padata_type, &(to)->padata_type)) goto fail;
if(der_copy_octet_string(&(from)->padata_value, &(to)->padata_value)) goto fail;
return 0;
fail:
free_PA_DATA(to);
return ENOMEM;
}

