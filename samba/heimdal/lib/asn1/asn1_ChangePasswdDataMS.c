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
encode_ChangePasswdDataMS(unsigned char *p, size_t len, const ChangePasswdDataMS *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* targrealm */
if((data)->targrealm) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, (data)->targrealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* targname */
if((data)->targname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->targname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* newpasswd */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_octet_string(p, len, &(data)->newpasswd, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_OctetString, &l);
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
decode_ChangePasswdDataMS(const unsigned char *p, size_t len, ChangePasswdDataMS *data, size_t *size)
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
size_t newpasswd_datalen, newpasswd_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &newpasswd_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
newpasswd_oldlen = len;
if (newpasswd_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = newpasswd_datalen;
{
size_t newpasswd_Tag_datalen, newpasswd_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_OctetString, &newpasswd_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
newpasswd_Tag_oldlen = len;
if (newpasswd_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = newpasswd_Tag_datalen;
e = der_get_octet_string(p, len, &(data)->newpasswd, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = newpasswd_Tag_oldlen - newpasswd_Tag_datalen;
}
len = newpasswd_oldlen - newpasswd_datalen;
}
{
size_t targname_datalen, targname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &targname_datalen, &l);
if(e) {
(data)->targname = NULL;
} else {
(data)->targname = calloc(1, sizeof(*(data)->targname));
if ((data)->targname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
targname_oldlen = len;
if (targname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = targname_datalen;
e = decode_PrincipalName(p, len, (data)->targname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = targname_oldlen - targname_datalen;
}
}
{
size_t targrealm_datalen, targrealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &targrealm_datalen, &l);
if(e) {
(data)->targrealm = NULL;
} else {
(data)->targrealm = calloc(1, sizeof(*(data)->targrealm));
if ((data)->targrealm == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
targrealm_oldlen = len;
if (targrealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = targrealm_datalen;
e = decode_Realm(p, len, (data)->targrealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = targrealm_oldlen - targrealm_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_ChangePasswdDataMS(data);
return e;
}

void
free_ChangePasswdDataMS(ChangePasswdDataMS *data)
{
der_free_octet_string(&(data)->newpasswd);
if((data)->targname) {
free_PrincipalName((data)->targname);
free((data)->targname);
(data)->targname = NULL;
}
if((data)->targrealm) {
free_Realm((data)->targrealm);
free((data)->targrealm);
(data)->targrealm = NULL;
}
}

size_t
length_ChangePasswdDataMS(const ChangePasswdDataMS *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_octet_string(&(data)->newpasswd);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->targname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->targname);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->targrealm){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm((data)->targrealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_ChangePasswdDataMS(const ChangePasswdDataMS *from, ChangePasswdDataMS *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_octet_string(&(from)->newpasswd, &(to)->newpasswd)) goto fail;
if((from)->targname) {
(to)->targname = malloc(sizeof(*(to)->targname));
if((to)->targname == NULL) goto fail;
if(copy_PrincipalName((from)->targname, (to)->targname)) goto fail;
}else
(to)->targname = NULL;
if((from)->targrealm) {
(to)->targrealm = malloc(sizeof(*(to)->targrealm));
if((to)->targrealm == NULL) goto fail;
if(copy_Realm((from)->targrealm, (to)->targrealm)) goto fail;
}else
(to)->targrealm = NULL;
return 0;
fail:
free_ChangePasswdDataMS(to);
return ENOMEM;
}

