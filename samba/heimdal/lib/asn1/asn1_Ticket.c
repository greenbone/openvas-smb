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
encode_Ticket(unsigned char *p, size_t len, const Ticket *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* enc-part */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptedData(p, len, &(data)->enc_part, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* sname */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, &(data)->sname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* realm */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->realm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* tkt-vno */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->tkt_vno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_Ticket(const unsigned char *p, size_t len, Ticket *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 1, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
{
size_t Top_Tag_datalen, Top_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &Top_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_Tag_oldlen = len;
if (Top_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_Tag_datalen;
{
size_t tkt_vno_datalen, tkt_vno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &tkt_vno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
tkt_vno_oldlen = len;
if (tkt_vno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = tkt_vno_datalen;
e = decode_krb5int32(p, len, &(data)->tkt_vno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = tkt_vno_oldlen - tkt_vno_datalen;
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
{
size_t sname_datalen, sname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &sname_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
sname_oldlen = len;
if (sname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = sname_datalen;
e = decode_PrincipalName(p, len, &(data)->sname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = sname_oldlen - sname_datalen;
}
{
size_t enc_part_datalen, enc_part_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &enc_part_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
enc_part_oldlen = len;
if (enc_part_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = enc_part_datalen;
e = decode_EncryptedData(p, len, &(data)->enc_part, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = enc_part_oldlen - enc_part_datalen;
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Ticket(data);
return e;
}

void
free_Ticket(Ticket *data)
{
free_krb5int32(&(data)->tkt_vno);
free_Realm(&(data)->realm);
free_PrincipalName(&(data)->sname);
free_EncryptedData(&(data)->enc_part);
}

size_t
length_Ticket(const Ticket *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->tkt_vno);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->realm);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName(&(data)->sname);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptedData(&(data)->enc_part);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Ticket(const Ticket *from, Ticket *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->tkt_vno, &(to)->tkt_vno)) goto fail;
if(copy_Realm(&(from)->realm, &(to)->realm)) goto fail;
if(copy_PrincipalName(&(from)->sname, &(to)->sname)) goto fail;
if(copy_EncryptedData(&(from)->enc_part, &(to)->enc_part)) goto fail;
return 0;
fail:
free_Ticket(to);
return ENOMEM;
}

