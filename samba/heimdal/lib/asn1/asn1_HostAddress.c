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
encode_HostAddress(unsigned char *p, size_t len, const HostAddress *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* address */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->address, &l);
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
/* addr-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->addr_type, &l);
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
decode_HostAddress(const unsigned char *p, size_t len, HostAddress *data, size_t *size)
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
size_t addr_type_datalen, addr_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &addr_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
addr_type_oldlen = len;
if (addr_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = addr_type_datalen;
e = decode_krb5int32(p, len, &(data)->addr_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = addr_type_oldlen - addr_type_datalen;
}
{
size_t address_datalen, address_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &address_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
address_oldlen = len;
if (address_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = address_datalen;
{
size_t address_Tag_datalen, address_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &address_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
address_Tag_oldlen = len;
if (address_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = address_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->address, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = address_Tag_oldlen - address_Tag_datalen;
}
len = address_oldlen - address_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_HostAddress(data);
return e;
}

void
free_HostAddress(HostAddress *data)
{
free_krb5int32(&(data)->addr_type);
der_free_octet_string(&(data)->address);
}

size_t
length_HostAddress(const HostAddress *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->addr_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->address);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_HostAddress(const HostAddress *from, HostAddress *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->addr_type, &(to)->addr_type)) goto fail;
if(der_copy_octet_string(&(from)->address, &(to)->address)) goto fail;
return 0;
fail:
free_HostAddress(to);
return ENOMEM;
}

