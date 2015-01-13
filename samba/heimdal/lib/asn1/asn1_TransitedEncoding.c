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
encode_TransitedEncoding(unsigned char *p, size_t len, const TransitedEncoding *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* contents */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->contents, &l);
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
/* tr-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->tr_type, &l);
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
decode_TransitedEncoding(const unsigned char *p, size_t len, TransitedEncoding *data, size_t *size)
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
size_t tr_type_datalen, tr_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &tr_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tr_type_oldlen = len;
if (tr_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tr_type_datalen;
e = decode_krb5int32(p, len, &(data)->tr_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = tr_type_oldlen - tr_type_datalen;
}
{
size_t contents_datalen, contents_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &contents_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
contents_oldlen = len;
if (contents_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = contents_datalen;
{
size_t contents_Tag_datalen, contents_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &contents_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
contents_Tag_oldlen = len;
if (contents_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = contents_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->contents, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = contents_Tag_oldlen - contents_Tag_datalen;
}
len = contents_oldlen - contents_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_TransitedEncoding(data);
return e;
}

void
free_TransitedEncoding(TransitedEncoding *data)
{
free_krb5int32(&(data)->tr_type);
der_free_octet_string(&(data)->contents);
}

size_t
length_TransitedEncoding(const TransitedEncoding *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->tr_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->contents);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_TransitedEncoding(const TransitedEncoding *from, TransitedEncoding *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->tr_type, &(to)->tr_type)) goto fail;
if(der_copy_octet_string(&(from)->contents, &(to)->contents)) goto fail;
return 0;
fail:
free_TransitedEncoding(to);
return ENOMEM;
}

