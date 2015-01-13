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
encode_PA_ENC_TS_ENC(unsigned char *p, size_t len, const PA_ENC_TS_ENC *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* pausec */
if((data)->pausec) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, (data)->pausec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* patimestamp */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->patimestamp, &l);
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
decode_PA_ENC_TS_ENC(const unsigned char *p, size_t len, PA_ENC_TS_ENC *data, size_t *size)
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
size_t patimestamp_datalen, patimestamp_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &patimestamp_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
patimestamp_oldlen = len;
if (patimestamp_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = patimestamp_datalen;
e = decode_KerberosTime(p, len, &(data)->patimestamp, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = patimestamp_oldlen - patimestamp_datalen;
}
{
size_t pausec_datalen, pausec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &pausec_datalen, &l);
if(e) {
(data)->pausec = NULL;
} else {
(data)->pausec = calloc(1, sizeof(*(data)->pausec));
if ((data)->pausec == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
pausec_oldlen = len;
if (pausec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pausec_datalen;
e = decode_krb5int32(p, len, (data)->pausec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pausec_oldlen - pausec_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_PA_ENC_TS_ENC(data);
return e;
}

void
free_PA_ENC_TS_ENC(PA_ENC_TS_ENC *data)
{
free_KerberosTime(&(data)->patimestamp);
if((data)->pausec) {
free_krb5int32((data)->pausec);
free((data)->pausec);
(data)->pausec = NULL;
}
}

size_t
length_PA_ENC_TS_ENC(const PA_ENC_TS_ENC *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->patimestamp);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->pausec){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32((data)->pausec);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_PA_ENC_TS_ENC(const PA_ENC_TS_ENC *from, PA_ENC_TS_ENC *to)
{
memset(to, 0, sizeof(*to));
if(copy_KerberosTime(&(from)->patimestamp, &(to)->patimestamp)) goto fail;
if((from)->pausec) {
(to)->pausec = malloc(sizeof(*(to)->pausec));
if((to)->pausec == NULL) goto fail;
if(copy_krb5int32((from)->pausec, (to)->pausec)) goto fail;
}else
(to)->pausec = NULL;
return 0;
fail:
free_PA_ENC_TS_ENC(to);
return ENOMEM;
}

