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
encode_Principal(unsigned char *p, size_t len, const Principal *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* realm */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->realm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* name */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, &(data)->name, &l);
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
decode_Principal(const unsigned char *p, size_t len, Principal *data, size_t *size)
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
size_t name_datalen, name_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &name_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
name_oldlen = len;
if (name_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = name_datalen;
e = decode_PrincipalName(p, len, &(data)->name, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = name_oldlen - name_datalen;
}
{
size_t realm_datalen, realm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &realm_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
realm_oldlen = len;
if (realm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = realm_datalen;
e = decode_Realm(p, len, &(data)->realm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = realm_oldlen - realm_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Principal(data);
return e;
}

void
free_Principal(Principal *data)
{
free_PrincipalName(&(data)->name);
free_Realm(&(data)->realm);
}

size_t
length_Principal(const Principal *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName(&(data)->name);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->realm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Principal(const Principal *from, Principal *to)
{
memset(to, 0, sizeof(*to));
if(copy_PrincipalName(&(from)->name, &(to)->name)) goto fail;
if(copy_Realm(&(from)->realm, &(to)->realm)) goto fail;
return 0;
fail:
free_Principal(to);
return ENOMEM;
}

