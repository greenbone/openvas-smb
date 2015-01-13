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
encode_Salt(unsigned char *p, size_t len, const Salt *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* salt */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->salt, &l);
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
/* type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, &(data)->type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
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
decode_Salt(const unsigned char *p, size_t len, Salt *data, size_t *size)
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
size_t type_datalen, type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
type_oldlen = len;
if (type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = type_datalen;
{
size_t type_Tag_datalen, type_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &type_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
type_Tag_oldlen = len;
if (type_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = type_Tag_datalen;
e = der_get_unsigned(p, len, &(data)->type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = type_Tag_oldlen - type_Tag_datalen;
}
len = type_oldlen - type_datalen;
}
{
size_t salt_datalen, salt_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &salt_datalen, &l);
if(e) goto fail;
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
e = der_get_octet_string(p, len, &(data)->salt, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = salt_Tag_oldlen - salt_Tag_datalen;
}
len = salt_oldlen - salt_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Salt(data);
return e;
}

void
free_Salt(Salt *data)
{
der_free_octet_string(&(data)->salt);
}

size_t
length_Salt(const Salt *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned(&(data)->type);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->salt);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Salt(const Salt *from, Salt *to)
{
memset(to, 0, sizeof(*to));
*(&(to)->type) = *(&(from)->type);
if(der_copy_octet_string(&(from)->salt, &(to)->salt)) goto fail;
return 0;
fail:
free_Salt(to);
return ENOMEM;
}

