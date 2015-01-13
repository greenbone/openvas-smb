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
encode_KRB5SignedPath(unsigned char *p, size_t len, const KRB5SignedPath *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* delegated */
if((data)->delegated) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KRB5SignedPathPrincipals(p, len, (data)->delegated, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* cksum */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Checksum(p, len, &(data)->cksum, &l);
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
decode_KRB5SignedPath(const unsigned char *p, size_t len, KRB5SignedPath *data, size_t *size)
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
size_t cksum_datalen, cksum_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &cksum_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cksum_oldlen = len;
if (cksum_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cksum_datalen;
e = decode_Checksum(p, len, &(data)->cksum, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cksum_oldlen - cksum_datalen;
}
{
size_t delegated_datalen, delegated_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &delegated_datalen, &l);
if(e) {
(data)->delegated = NULL;
} else {
(data)->delegated = calloc(1, sizeof(*(data)->delegated));
if ((data)->delegated == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
delegated_oldlen = len;
if (delegated_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = delegated_datalen;
e = decode_KRB5SignedPathPrincipals(p, len, (data)->delegated, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = delegated_oldlen - delegated_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KRB5SignedPath(data);
return e;
}

void
free_KRB5SignedPath(KRB5SignedPath *data)
{
free_ENCTYPE(&(data)->etype);
free_Checksum(&(data)->cksum);
if((data)->delegated) {
free_KRB5SignedPathPrincipals((data)->delegated);
free((data)->delegated);
(data)->delegated = NULL;
}
}

size_t
length_KRB5SignedPath(const KRB5SignedPath *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_ENCTYPE(&(data)->etype);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Checksum(&(data)->cksum);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->delegated){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KRB5SignedPathPrincipals((data)->delegated);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KRB5SignedPath(const KRB5SignedPath *from, KRB5SignedPath *to)
{
memset(to, 0, sizeof(*to));
if(copy_ENCTYPE(&(from)->etype, &(to)->etype)) goto fail;
if(copy_Checksum(&(from)->cksum, &(to)->cksum)) goto fail;
if((from)->delegated) {
(to)->delegated = malloc(sizeof(*(to)->delegated));
if((to)->delegated == NULL) goto fail;
if(copy_KRB5SignedPathPrincipals((from)->delegated, (to)->delegated)) goto fail;
}else
(to)->delegated = NULL;
return 0;
fail:
free_KRB5SignedPath(to);
return ENOMEM;
}

