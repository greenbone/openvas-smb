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
encode_Authenticator(unsigned char *p, size_t len, const Authenticator *data, size_t *size)
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* seq-number */
if((data)->seq_number) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5uint32(p, len, (data)->seq_number, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* subkey */
if((data)->subkey) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_EncryptionKey(p, len, (data)->subkey, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* ctime */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->ctime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cusec */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->cusec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* cksum */
if((data)->cksum) {
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_Checksum(p, len, (data)->cksum, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_tag_oldret;
}
/* authenticator-vno */
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->authenticator_vno, &l);
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_APPL, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_Authenticator(const unsigned char *p, size_t len, Authenticator *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_APPL, CONS, 2, &Top_datalen, &l);
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
size_t authenticator_vno_datalen, authenticator_vno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &authenticator_vno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
authenticator_vno_oldlen = len;
if (authenticator_vno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authenticator_vno_datalen;
e = decode_krb5int32(p, len, &(data)->authenticator_vno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authenticator_vno_oldlen - authenticator_vno_datalen;
}
{
size_t crealm_datalen, crealm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &crealm_datalen, &l);
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
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &cname_datalen, &l);
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
size_t cksum_datalen, cksum_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &cksum_datalen, &l);
if(e) {
(data)->cksum = NULL;
} else {
(data)->cksum = calloc(1, sizeof(*(data)->cksum));
if ((data)->cksum == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
cksum_oldlen = len;
if (cksum_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cksum_datalen;
e = decode_Checksum(p, len, (data)->cksum, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cksum_oldlen - cksum_datalen;
}
}
{
size_t cusec_datalen, cusec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &cusec_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cusec_oldlen = len;
if (cusec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cusec_datalen;
e = decode_krb5int32(p, len, &(data)->cusec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cusec_oldlen - cusec_datalen;
}
{
size_t ctime_datalen, ctime_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &ctime_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
ctime_oldlen = len;
if (ctime_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = ctime_datalen;
e = decode_KerberosTime(p, len, &(data)->ctime, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = ctime_oldlen - ctime_datalen;
}
{
size_t subkey_datalen, subkey_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &subkey_datalen, &l);
if(e) {
(data)->subkey = NULL;
} else {
(data)->subkey = calloc(1, sizeof(*(data)->subkey));
if ((data)->subkey == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
subkey_oldlen = len;
if (subkey_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = subkey_datalen;
e = decode_EncryptionKey(p, len, (data)->subkey, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = subkey_oldlen - subkey_datalen;
}
}
{
size_t seq_number_datalen, seq_number_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &seq_number_datalen, &l);
if(e) {
(data)->seq_number = NULL;
} else {
(data)->seq_number = calloc(1, sizeof(*(data)->seq_number));
if ((data)->seq_number == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
seq_number_oldlen = len;
if (seq_number_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = seq_number_datalen;
e = decode_krb5uint32(p, len, (data)->seq_number, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = seq_number_oldlen - seq_number_datalen;
}
}
{
size_t authorization_data_datalen, authorization_data_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &authorization_data_datalen, &l);
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
free_Authenticator(data);
return e;
}

void
free_Authenticator(Authenticator *data)
{
free_krb5int32(&(data)->authenticator_vno);
free_Realm(&(data)->crealm);
free_PrincipalName(&(data)->cname);
if((data)->cksum) {
free_Checksum((data)->cksum);
free((data)->cksum);
(data)->cksum = NULL;
}
free_krb5int32(&(data)->cusec);
free_KerberosTime(&(data)->ctime);
if((data)->subkey) {
free_EncryptionKey((data)->subkey);
free((data)->subkey);
(data)->subkey = NULL;
}
if((data)->seq_number) {
free_krb5uint32((data)->seq_number);
free((data)->seq_number);
(data)->seq_number = NULL;
}
if((data)->authorization_data) {
free_AuthorizationData((data)->authorization_data);
free((data)->authorization_data);
(data)->authorization_data = NULL;
}
}

size_t
length_Authenticator(const Authenticator *data)
{
size_t ret = 0;
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->authenticator_vno);
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
if((data)->cksum){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_Checksum((data)->cksum);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->cusec);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
{
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->ctime);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->subkey){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_EncryptionKey((data)->subkey);
ret += 1 + der_length_len (ret);
ret += Top_tag_tag_oldret;
}
if((data)->seq_number){
size_t Top_tag_tag_oldret = ret;
ret = 0;
ret += length_krb5uint32((data)->seq_number);
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
copy_Authenticator(const Authenticator *from, Authenticator *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->authenticator_vno, &(to)->authenticator_vno)) goto fail;
if(copy_Realm(&(from)->crealm, &(to)->crealm)) goto fail;
if(copy_PrincipalName(&(from)->cname, &(to)->cname)) goto fail;
if((from)->cksum) {
(to)->cksum = malloc(sizeof(*(to)->cksum));
if((to)->cksum == NULL) goto fail;
if(copy_Checksum((from)->cksum, (to)->cksum)) goto fail;
}else
(to)->cksum = NULL;
if(copy_krb5int32(&(from)->cusec, &(to)->cusec)) goto fail;
if(copy_KerberosTime(&(from)->ctime, &(to)->ctime)) goto fail;
if((from)->subkey) {
(to)->subkey = malloc(sizeof(*(to)->subkey));
if((to)->subkey == NULL) goto fail;
if(copy_EncryptionKey((from)->subkey, (to)->subkey)) goto fail;
}else
(to)->subkey = NULL;
if((from)->seq_number) {
(to)->seq_number = malloc(sizeof(*(to)->seq_number));
if((to)->seq_number == NULL) goto fail;
if(copy_krb5uint32((from)->seq_number, (to)->seq_number)) goto fail;
}else
(to)->seq_number = NULL;
if((from)->authorization_data) {
(to)->authorization_data = malloc(sizeof(*(to)->authorization_data));
if((to)->authorization_data == NULL) goto fail;
if(copy_AuthorizationData((from)->authorization_data, (to)->authorization_data)) goto fail;
}else
(to)->authorization_data = NULL;
return 0;
fail:
free_Authenticator(to);
return ENOMEM;
}

