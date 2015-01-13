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
encode_KrbCredInfo(unsigned char *p, size_t len, const KrbCredInfo *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* caddr */
if((data)->caddr) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_HostAddresses(p, len, (data)->caddr, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* sname */
if((data)->sname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->sname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* srealm */
if((data)->srealm) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, (data)->srealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* renew-till */
if((data)->renew_till) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->renew_till, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* endtime */
if((data)->endtime) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->endtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* starttime */
if((data)->starttime) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->starttime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* authtime */
if((data)->authtime) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->authtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* flags */
if((data)->flags) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_TicketFlags(p, len, (data)->flags, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* pname */
if((data)->pname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, (data)->pname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* prealm */
if((data)->prealm) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, (data)->prealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* key */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_EncryptionKey(p, len, &(data)->key, &l);
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
decode_KrbCredInfo(const unsigned char *p, size_t len, KrbCredInfo *data, size_t *size)
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
size_t key_datalen, key_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &key_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
key_oldlen = len;
if (key_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = key_datalen;
e = decode_EncryptionKey(p, len, &(data)->key, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = key_oldlen - key_datalen;
}
{
size_t prealm_datalen, prealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &prealm_datalen, &l);
if(e) {
(data)->prealm = NULL;
} else {
(data)->prealm = calloc(1, sizeof(*(data)->prealm));
if ((data)->prealm == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
prealm_oldlen = len;
if (prealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = prealm_datalen;
e = decode_Realm(p, len, (data)->prealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = prealm_oldlen - prealm_datalen;
}
}
{
size_t pname_datalen, pname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &pname_datalen, &l);
if(e) {
(data)->pname = NULL;
} else {
(data)->pname = calloc(1, sizeof(*(data)->pname));
if ((data)->pname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
pname_oldlen = len;
if (pname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pname_datalen;
e = decode_PrincipalName(p, len, (data)->pname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pname_oldlen - pname_datalen;
}
}
{
size_t flags_datalen, flags_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &flags_datalen, &l);
if(e) {
(data)->flags = NULL;
} else {
(data)->flags = calloc(1, sizeof(*(data)->flags));
if ((data)->flags == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
flags_oldlen = len;
if (flags_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = flags_datalen;
e = decode_TicketFlags(p, len, (data)->flags, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = flags_oldlen - flags_datalen;
}
}
{
size_t authtime_datalen, authtime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &authtime_datalen, &l);
if(e) {
(data)->authtime = NULL;
} else {
(data)->authtime = calloc(1, sizeof(*(data)->authtime));
if ((data)->authtime == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
authtime_oldlen = len;
if (authtime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authtime_datalen;
e = decode_KerberosTime(p, len, (data)->authtime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authtime_oldlen - authtime_datalen;
}
}
{
size_t starttime_datalen, starttime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &starttime_datalen, &l);
if(e) {
(data)->starttime = NULL;
} else {
(data)->starttime = calloc(1, sizeof(*(data)->starttime));
if ((data)->starttime == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
starttime_oldlen = len;
if (starttime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = starttime_datalen;
e = decode_KerberosTime(p, len, (data)->starttime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = starttime_oldlen - starttime_datalen;
}
}
{
size_t endtime_datalen, endtime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &endtime_datalen, &l);
if(e) {
(data)->endtime = NULL;
} else {
(data)->endtime = calloc(1, sizeof(*(data)->endtime));
if ((data)->endtime == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
endtime_oldlen = len;
if (endtime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = endtime_datalen;
e = decode_KerberosTime(p, len, (data)->endtime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = endtime_oldlen - endtime_datalen;
}
}
{
size_t renew_till_datalen, renew_till_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &renew_till_datalen, &l);
if(e) {
(data)->renew_till = NULL;
} else {
(data)->renew_till = calloc(1, sizeof(*(data)->renew_till));
if ((data)->renew_till == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
renew_till_oldlen = len;
if (renew_till_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = renew_till_datalen;
e = decode_KerberosTime(p, len, (data)->renew_till, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = renew_till_oldlen - renew_till_datalen;
}
}
{
size_t srealm_datalen, srealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &srealm_datalen, &l);
if(e) {
(data)->srealm = NULL;
} else {
(data)->srealm = calloc(1, sizeof(*(data)->srealm));
if ((data)->srealm == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
srealm_oldlen = len;
if (srealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = srealm_datalen;
e = decode_Realm(p, len, (data)->srealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = srealm_oldlen - srealm_datalen;
}
}
{
size_t sname_datalen, sname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &sname_datalen, &l);
if(e) {
(data)->sname = NULL;
} else {
(data)->sname = calloc(1, sizeof(*(data)->sname));
if ((data)->sname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
sname_oldlen = len;
if (sname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = sname_datalen;
e = decode_PrincipalName(p, len, (data)->sname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = sname_oldlen - sname_datalen;
}
}
{
size_t caddr_datalen, caddr_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &caddr_datalen, &l);
if(e) {
(data)->caddr = NULL;
} else {
(data)->caddr = calloc(1, sizeof(*(data)->caddr));
if ((data)->caddr == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
caddr_oldlen = len;
if (caddr_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = caddr_datalen;
e = decode_HostAddresses(p, len, (data)->caddr, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = caddr_oldlen - caddr_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KrbCredInfo(data);
return e;
}

void
free_KrbCredInfo(KrbCredInfo *data)
{
free_EncryptionKey(&(data)->key);
if((data)->prealm) {
free_Realm((data)->prealm);
free((data)->prealm);
(data)->prealm = NULL;
}
if((data)->pname) {
free_PrincipalName((data)->pname);
free((data)->pname);
(data)->pname = NULL;
}
if((data)->flags) {
free_TicketFlags((data)->flags);
free((data)->flags);
(data)->flags = NULL;
}
if((data)->authtime) {
free_KerberosTime((data)->authtime);
free((data)->authtime);
(data)->authtime = NULL;
}
if((data)->starttime) {
free_KerberosTime((data)->starttime);
free((data)->starttime);
(data)->starttime = NULL;
}
if((data)->endtime) {
free_KerberosTime((data)->endtime);
free((data)->endtime);
(data)->endtime = NULL;
}
if((data)->renew_till) {
free_KerberosTime((data)->renew_till);
free((data)->renew_till);
(data)->renew_till = NULL;
}
if((data)->srealm) {
free_Realm((data)->srealm);
free((data)->srealm);
(data)->srealm = NULL;
}
if((data)->sname) {
free_PrincipalName((data)->sname);
free((data)->sname);
(data)->sname = NULL;
}
if((data)->caddr) {
free_HostAddresses((data)->caddr);
free((data)->caddr);
(data)->caddr = NULL;
}
}

size_t
length_KrbCredInfo(const KrbCredInfo *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey(&(data)->key);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->prealm){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm((data)->prealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->pname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->pname);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->flags){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_TicketFlags((data)->flags);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->authtime){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->authtime);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->starttime){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->starttime);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->endtime){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->endtime);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->renew_till){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->renew_till);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->srealm){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm((data)->srealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->sname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName((data)->sname);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->caddr){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_HostAddresses((data)->caddr);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KrbCredInfo(const KrbCredInfo *from, KrbCredInfo *to)
{
memset(to, 0, sizeof(*to));
if(copy_EncryptionKey(&(from)->key, &(to)->key)) goto fail;
if((from)->prealm) {
(to)->prealm = malloc(sizeof(*(to)->prealm));
if((to)->prealm == NULL) goto fail;
if(copy_Realm((from)->prealm, (to)->prealm)) goto fail;
}else
(to)->prealm = NULL;
if((from)->pname) {
(to)->pname = malloc(sizeof(*(to)->pname));
if((to)->pname == NULL) goto fail;
if(copy_PrincipalName((from)->pname, (to)->pname)) goto fail;
}else
(to)->pname = NULL;
if((from)->flags) {
(to)->flags = malloc(sizeof(*(to)->flags));
if((to)->flags == NULL) goto fail;
if(copy_TicketFlags((from)->flags, (to)->flags)) goto fail;
}else
(to)->flags = NULL;
if((from)->authtime) {
(to)->authtime = malloc(sizeof(*(to)->authtime));
if((to)->authtime == NULL) goto fail;
if(copy_KerberosTime((from)->authtime, (to)->authtime)) goto fail;
}else
(to)->authtime = NULL;
if((from)->starttime) {
(to)->starttime = malloc(sizeof(*(to)->starttime));
if((to)->starttime == NULL) goto fail;
if(copy_KerberosTime((from)->starttime, (to)->starttime)) goto fail;
}else
(to)->starttime = NULL;
if((from)->endtime) {
(to)->endtime = malloc(sizeof(*(to)->endtime));
if((to)->endtime == NULL) goto fail;
if(copy_KerberosTime((from)->endtime, (to)->endtime)) goto fail;
}else
(to)->endtime = NULL;
if((from)->renew_till) {
(to)->renew_till = malloc(sizeof(*(to)->renew_till));
if((to)->renew_till == NULL) goto fail;
if(copy_KerberosTime((from)->renew_till, (to)->renew_till)) goto fail;
}else
(to)->renew_till = NULL;
if((from)->srealm) {
(to)->srealm = malloc(sizeof(*(to)->srealm));
if((to)->srealm == NULL) goto fail;
if(copy_Realm((from)->srealm, (to)->srealm)) goto fail;
}else
(to)->srealm = NULL;
if((from)->sname) {
(to)->sname = malloc(sizeof(*(to)->sname));
if((to)->sname == NULL) goto fail;
if(copy_PrincipalName((from)->sname, (to)->sname)) goto fail;
}else
(to)->sname = NULL;
if((from)->caddr) {
(to)->caddr = malloc(sizeof(*(to)->caddr));
if((to)->caddr == NULL) goto fail;
if(copy_HostAddresses((from)->caddr, (to)->caddr)) goto fail;
}else
(to)->caddr = NULL;
return 0;
fail:
free_KrbCredInfo(to);
return ENOMEM;
}

