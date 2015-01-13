/* Generated from spnego.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <spnego_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_NegHints(unsigned char *p, size_t len, const NegHints *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* hintAddress */
if((data)->hintAddress) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, (data)->hintAddress, &l);
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
/* hintName */
if((data)->hintName) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_general_string(p, len, (data)->hintName, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_GeneralString, &l);
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
decode_NegHints(const unsigned char *p, size_t len, NegHints *data, size_t *size)
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
size_t hintName_datalen, hintName_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &hintName_datalen, &l);
if(e) {
(data)->hintName = NULL;
} else {
(data)->hintName = calloc(1, sizeof(*(data)->hintName));
if ((data)->hintName == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
hintName_oldlen = len;
if (hintName_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hintName_datalen;
{
size_t hintName_Tag_datalen, hintName_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_GeneralString, &hintName_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
hintName_Tag_oldlen = len;
if (hintName_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hintName_Tag_datalen;
e = der_get_general_string(p, len, (data)->hintName, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = hintName_Tag_oldlen - hintName_Tag_datalen;
}
len = hintName_oldlen - hintName_datalen;
}
}
{
size_t hintAddress_datalen, hintAddress_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &hintAddress_datalen, &l);
if(e) {
(data)->hintAddress = NULL;
} else {
(data)->hintAddress = calloc(1, sizeof(*(data)->hintAddress));
if ((data)->hintAddress == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
hintAddress_oldlen = len;
if (hintAddress_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hintAddress_datalen;
{
size_t hintAddress_Tag_datalen, hintAddress_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &hintAddress_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
hintAddress_Tag_oldlen = len;
if (hintAddress_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hintAddress_Tag_datalen;
e = der_get_octet_string(p, len, (data)->hintAddress, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = hintAddress_Tag_oldlen - hintAddress_Tag_datalen;
}
len = hintAddress_oldlen - hintAddress_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_NegHints(data);
return e;
}

void
free_NegHints(NegHints *data)
{
if((data)->hintName) {
der_free_general_string((data)->hintName);
free((data)->hintName);
(data)->hintName = NULL;
}
if((data)->hintAddress) {
der_free_octet_string((data)->hintAddress);
free((data)->hintAddress);
(data)->hintAddress = NULL;
}
}

size_t
length_NegHints(const NegHints *data)
{
size_t ret = 0;
if((data)->hintName){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_general_string((data)->hintName);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->hintAddress){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string((data)->hintAddress);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_NegHints(const NegHints *from, NegHints *to)
{
memset(to, 0, sizeof(*to));
if((from)->hintName) {
(to)->hintName = malloc(sizeof(*(to)->hintName));
if((to)->hintName == NULL) goto fail;
if(der_copy_general_string((from)->hintName, (to)->hintName)) goto fail;
}else
(to)->hintName = NULL;
if((from)->hintAddress) {
(to)->hintAddress = malloc(sizeof(*(to)->hintAddress));
if((to)->hintAddress == NULL) goto fail;
if(der_copy_octet_string((from)->hintAddress, (to)->hintAddress)) goto fail;
}else
(to)->hintAddress = NULL;
return 0;
fail:
free_NegHints(to);
return ENOMEM;
}

