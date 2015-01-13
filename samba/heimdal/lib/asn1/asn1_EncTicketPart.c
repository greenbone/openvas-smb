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
encode_EncTicketPart(unsigned char *p, size_t len, const EncTicketPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* authorization-data */
if((data)->authorization_data) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_AuthorizationData(p, len, (data)->authorization_data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 10, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* caddr */
if((data)->caddr) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_HostAddresses(p, len, (data)->caddr, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* renew-till */
if((data)->renew_till) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->renew_till, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* endtime */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->endtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* starttime */
if((data)->starttime) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, (data)->starttime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* authtime */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->authtime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* transited */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_TransitedEncoding(p, len, &(data)->transited, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cname */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_PrincipalName(p, len, &(data)->cname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* crealm */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_Realm(p, len, &(data)->crealm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* key */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptionKey(p, len, &(data)->key, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* flags */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_TicketFlags(p, len, &(data)->flags, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_EncTicketPart(const unsigned char *p, size_t len, EncTicketPart *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 3, &Top_datalen, &l);
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
size_t flags_datalen, flags_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &flags_datalen, &l);
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
size_t key_datalen, key_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &key_datalen, &l);
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
size_t crealm_datalen, crealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &crealm_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
crealm_oldlen = len;
if (crealm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = crealm_datalen;
e = decode_Realm(p, len, &(data)->crealm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = crealm_oldlen - crealm_datalen;
}
{
size_t cname_datalen, cname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &cname_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cname_oldlen = len;
if (cname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cname_datalen;
e = decode_PrincipalName(p, len, &(data)->cname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cname_oldlen - cname_datalen;
}
{
size_t transited_datalen, transited_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &transited_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
transited_oldlen = len;
if (transited_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = transited_datalen;
e = decode_TransitedEncoding(p, len, &(data)->transited, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = transited_oldlen - transited_datalen;
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
size_t caddr_datalen, caddr_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &caddr_datalen, &l);
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
{
size_t authorization_data_datalen, authorization_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 10, &authorization_data_datalen, &l);
if(e) {
(data)->authorization_data = NULL;
} else {
(data)->authorization_data = calloc(1, sizeof(*(data)->authorization_data));
if ((data)->authorization_data == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
authorization_data_oldlen = len;
if (authorization_data_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authorization_data_datalen;
e = decode_AuthorizationData(p, len, (data)->authorization_data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authorization_data_oldlen - authorization_data_datalen;
}
}
len = Top_Tag_oldlen - Top_Tag_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_EncTicketPart(data);
return e;
}

void
free_EncTicketPart(EncTicketPart *data)
{
free_TicketFlags(&(data)->flags);
free_EncryptionKey(&(data)->key);
free_Realm(&(data)->crealm);
free_PrincipalName(&(data)->cname);
free_TransitedEncoding(&(data)->transited);
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
if((data)->caddr) {
free_HostAddresses((data)->caddr);
free((data)->caddr);
(data)->caddr = NULL;
}
if((data)->authorization_data) {
free_AuthorizationData((data)->authorization_data);
free((data)->authorization_data);
(data)->authorization_data = NULL;
}
}

size_t
length_EncTicketPart(const EncTicketPart *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_TicketFlags(&(data)->flags);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey(&(data)->key);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_Realm(&(data)->crealm);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_PrincipalName(&(data)->cname);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_TransitedEncoding(&(data)->transited);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->authtime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->starttime){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->starttime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->endtime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->renew_till){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime((data)->renew_till);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->caddr){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_HostAddresses((data)->caddr);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->authorization_data){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_AuthorizationData((data)->authorization_data);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_EncTicketPart(const EncTicketPart *from, EncTicketPart *to)
{
memset(to, 0, sizeof(*to));
if(copy_TicketFlags(&(from)->flags, &(to)->flags)) goto fail;
if(copy_EncryptionKey(&(from)->key, &(to)->key)) goto fail;
if(copy_Realm(&(from)->crealm, &(to)->crealm)) goto fail;
if(copy_PrincipalName(&(from)->cname, &(to)->cname)) goto fail;
if(copy_TransitedEncoding(&(from)->transited, &(to)->transited)) goto fail;
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
if((from)->caddr) {
(to)->caddr = malloc(sizeof(*(to)->caddr));
if((to)->caddr == NULL) goto fail;
if(copy_HostAddresses((from)->caddr, (to)->caddr)) goto fail;
}else
(to)->caddr = NULL;
if((from)->authorization_data) {
(to)->authorization_data = malloc(sizeof(*(to)->authorization_data));
if((to)->authorization_data == NULL) goto fail;
if(copy_AuthorizationData((from)->authorization_data, (to)->authorization_data)) goto fail;
}else
(to)->authorization_data = NULL;
return 0;
fail:
free_EncTicketPart(to);
return ENOMEM;
}

