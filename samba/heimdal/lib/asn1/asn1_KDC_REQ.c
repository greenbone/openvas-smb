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
encode_KDC_REQ(unsigned char *p, size_t len, const KDC_REQ *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* req-body */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KDC_REQ_BODY(p, len, &(data)->req_body, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* padata */
if((data)->padata) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_METHOD_DATA(p, len, (data)->padata, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* msg-type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* pvno */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_krb5int32(p, len, &(data)->pvno, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
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
decode_KDC_REQ(const unsigned char *p, size_t len, KDC_REQ *data, size_t *size)
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
size_t pvno_datalen, pvno_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &pvno_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
pvno_oldlen = len;
if (pvno_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = pvno_datalen;
e = decode_krb5int32(p, len, &(data)->pvno, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = pvno_oldlen - pvno_datalen;
}
{
size_t msg_type_datalen, msg_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &msg_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
msg_type_oldlen = len;
if (msg_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = msg_type_datalen;
e = decode_MESSAGE_TYPE(p, len, &(data)->msg_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = msg_type_oldlen - msg_type_datalen;
}
{
size_t padata_datalen, padata_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &padata_datalen, &l);
if(e) {
(data)->padata = NULL;
} else {
(data)->padata = calloc(1, sizeof(*(data)->padata));
if ((data)->padata == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
padata_oldlen = len;
if (padata_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = padata_datalen;
e = decode_METHOD_DATA(p, len, (data)->padata, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = padata_oldlen - padata_datalen;
}
}
{
size_t req_body_datalen, req_body_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &req_body_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
req_body_oldlen = len;
if (req_body_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = req_body_datalen;
e = decode_KDC_REQ_BODY(p, len, &(data)->req_body, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = req_body_oldlen - req_body_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KDC_REQ(data);
return e;
}

void
free_KDC_REQ(KDC_REQ *data)
{
free_krb5int32(&(data)->pvno);
free_MESSAGE_TYPE(&(data)->msg_type);
if((data)->padata) {
free_METHOD_DATA((data)->padata);
free((data)->padata);
(data)->padata = NULL;
}
free_KDC_REQ_BODY(&(data)->req_body);
}

size_t
length_KDC_REQ(const KDC_REQ *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_krb5int32(&(data)->pvno);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_MESSAGE_TYPE(&(data)->msg_type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->padata){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_METHOD_DATA((data)->padata);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KDC_REQ_BODY(&(data)->req_body);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KDC_REQ(const KDC_REQ *from, KDC_REQ *to)
{
memset(to, 0, sizeof(*to));
if(copy_krb5int32(&(from)->pvno, &(to)->pvno)) goto fail;
if(copy_MESSAGE_TYPE(&(from)->msg_type, &(to)->msg_type)) goto fail;
if((from)->padata) {
(to)->padata = malloc(sizeof(*(to)->padata));
if((to)->padata == NULL) goto fail;
if(copy_METHOD_DATA((from)->padata, (to)->padata)) goto fail;
}else
(to)->padata = NULL;
if(copy_KDC_REQ_BODY(&(from)->req_body, &(to)->req_body)) goto fail;
return 0;
fail:
free_KDC_REQ(to);
return ENOMEM;
}

