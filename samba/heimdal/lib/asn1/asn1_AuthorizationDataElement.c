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
encode_AuthorizationDataElement(unsigned char *p, size_t len, const AuthorizationDataElement *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* ad-data */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->ad_data, &l);
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
/* ad-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->ad_type, &l);
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
decode_AuthorizationDataElement(const unsigned char *p, size_t len, AuthorizationDataElement *data, size_t *size)
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
size_t ad_type_datalen, ad_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &ad_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ad_type_oldlen = len;
if (ad_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ad_type_datalen;
e = decode_krb5int32(p, len, &(data)->ad_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = ad_type_oldlen - ad_type_datalen;
}
{
size_t ad_data_datalen, ad_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &ad_data_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ad_data_oldlen = len;
if (ad_data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ad_data_datalen;
{
size_t ad_data_Tag_datalen, ad_data_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &ad_data_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ad_data_Tag_oldlen = len;
if (ad_data_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ad_data_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->ad_data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = ad_data_Tag_oldlen - ad_data_Tag_datalen;
}
len = ad_data_oldlen - ad_data_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_AuthorizationDataElement(data);
return e;
}

void
free_AuthorizationDataElement(AuthorizationDataElement *data)
{
free_krb5int32(&(data)->ad_type);
der_free_octet_string(&(data)->ad_data);
}

size_t
length_AuthorizationDataElement(const AuthorizationDataElement *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->ad_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->ad_data);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_AuthorizationDataElement(const AuthorizationDataElement *from, AuthorizationDataElement *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->ad_type, &(to)->ad_type)) goto fail;
if(der_copy_octet_string(&(from)->ad_data, &(to)->ad_data)) goto fail;
return 0;
fail:
free_AuthorizationDataElement(to);
return ENOMEM;
}

