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
encode_EncKDCRepPart(unsigned char *p, size_t len, const EncKDCRepPart *data, size_t *size)
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 11, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* sname */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, &(data)->sname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* srealm */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->srealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* endtime */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->endtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* authtime */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->authtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* flags */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_TicketFlags(p, len, &(data)->flags, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* key-expiration */
if((data)->key_expiration) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->key_expiration, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* nonce */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->nonce, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* last-req */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_LastReq(p, len, &(data)->last_req, &l);
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
decode_EncKDCRepPart(const unsigned char *p, size_t len, EncKDCRepPart *data, size_t *size)
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
size_t last_req_datalen, last_req_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &last_req_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
last_req_oldlen = len;
if (last_req_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = last_req_datalen;
e = decode_LastReq(p, len, &(data)->last_req, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = last_req_oldlen - last_req_datalen;
}
{
size_t nonce_datalen, nonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &nonce_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
nonce_oldlen = len;
if (nonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonce_datalen;
e = decode_krb5int32(p, len, &(data)->nonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = nonce_oldlen - nonce_datalen;
}
{
size_t key_expiration_datalen, key_expiration_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &key_expiration_datalen, &l);
if(e) {
(data)->key_expiration = NULL;
} else {
(data)->key_expiration = calloc(1, sizeof(*(data)->key_expiration));
if ((data)->key_expiration == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
key_expiration_oldlen = len;
if (key_expiration_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = key_expiration_datalen;
e = decode_KerberosTime(p, len, (data)->key_expiration, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = key_expiration_oldlen - key_expiration_datalen;
}
}
{
size_t flags_datalen, flags_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &flags_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
flags_oldlen = len;
if (flags_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = flags_datalen;
e = decode_TicketFlags(p, len, &(data)->flags, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = flags_oldlen - flags_datalen;
}
{
size_t authtime_datalen, authtime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &authtime_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
authtime_oldlen = len;
if (authtime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authtime_datalen;
e = decode_KerberosTime(p, len, &(data)->authtime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authtime_oldlen - authtime_datalen;
}
{
size_t starttime_datalen, starttime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &starttime_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &endtime_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
endtime_oldlen = len;
if (endtime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = endtime_datalen;
e = decode_KerberosTime(p, len, &(data)->endtime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = endtime_oldlen - endtime_datalen;
}
{
size_t renew_till_datalen, renew_till_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &renew_till_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &srealm_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
srealm_oldlen = len;
if (srealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = srealm_datalen;
e = decode_Realm(p, len, &(data)->srealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = srealm_oldlen - srealm_datalen;
}
{
size_t sname_datalen, sname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &sname_datalen, &l);
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
size_t caddr_datalen, caddr_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 11, &caddr_datalen, &l);
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
free_EncKDCRepPart(data);
return e;
}

void
free_EncKDCRepPart(EncKDCRepPart *data)
{
free_EncryptionKey(&(data)->key);
free_LastReq(&(data)->last_req);
free_krb5int32(&(data)->nonce);
if((data)->key_expiration) {
free_KerberosTime((data)->key_expiration);
free((data)->key_expiration);
(data)->key_expiration = NULL;
}
free_TicketFlags(&(data)->flags);
free_KerberosTime(&(data)->authtime);
if((data)->starttime) {
free_KerberosTime((data)->starttime);
free((data)->starttime);
(data)->starttime = NULL;
}
free_KerberosTime(&(data)->endtime);
if((data)->renew_till) {
free_KerberosTime((data)->renew_till);
free((data)->renew_till);
(data)->renew_till = NULL;
}
free_Realm(&(data)->srealm);
free_PrincipalName(&(data)->sname);
if((data)->caddr) {
free_HostAddresses((data)->caddr);
free((data)->caddr);
(data)->caddr = NULL;
}
}

size_t
length_EncKDCRepPart(const EncKDCRepPart *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey(&(data)->key);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_LastReq(&(data)->last_req);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->nonce);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->key_expiration){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->key_expiration);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_TicketFlags(&(data)->flags);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->authtime);
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
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->endtime);
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
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->srealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName(&(data)->sname);
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
copy_EncKDCRepPart(const EncKDCRepPart *from, EncKDCRepPart *to)
{
memset(to, 0, sizeof(*to));
if(copy_EncryptionKey(&(from)->key, &(to)->key)) goto fail;
if(copy_LastReq(&(from)->last_req, &(to)->last_req)) goto fail;
if(copy_krb5int32(&(from)->nonce, &(to)->nonce)) goto fail;
if((from)->key_expiration) {
(to)->key_expiration = malloc(sizeof(*(to)->key_expiration));
if((to)->key_expiration == NULL) goto fail;
if(copy_KerberosTime((from)->key_expiration, (to)->key_expiration)) goto fail;
}else
(to)->key_expiration = NULL;
if(copy_TicketFlags(&(from)->flags, &(to)->flags)) goto fail;
if(copy_KerberosTime(&(from)->authtime, &(to)->authtime)) goto fail;
if((from)->starttime) {
(to)->starttime = malloc(sizeof(*(to)->starttime));
if((to)->starttime == NULL) goto fail;
if(copy_KerberosTime((from)->starttime, (to)->starttime)) goto fail;
}else
(to)->starttime = NULL;
if(copy_KerberosTime(&(from)->endtime, &(to)->endtime)) goto fail;
if((from)->renew_till) {
(to)->renew_till = malloc(sizeof(*(to)->renew_till));
if((to)->renew_till == NULL) goto fail;
if(copy_KerberosTime((from)->renew_till, (to)->renew_till)) goto fail;
}else
(to)->renew_till = NULL;
if(copy_Realm(&(from)->srealm, &(to)->srealm)) goto fail;
if(copy_PrincipalName(&(from)->sname, &(to)->sname)) goto fail;
if((from)->caddr) {
(to)->caddr = malloc(sizeof(*(to)->caddr));
if((to)->caddr == NULL) goto fail;
if(copy_HostAddresses((from)->caddr, (to)->caddr)) goto fail;
}else
(to)->caddr = NULL;
return 0;
fail:
free_EncKDCRepPart(to);
return ENOMEM;
}

