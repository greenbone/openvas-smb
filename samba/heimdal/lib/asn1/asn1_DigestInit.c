/* Generated from digest.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <digest_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_DigestInit(unsigned char *p, size_t len, const DigestInit *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* hostname */
if((data)->hostname) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->hostname, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* channel */
if((data)->channel) {
size_t Top_tag_oldret = ret;
ret = 0;
/* cb-binding */
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &((data)->channel)->cb_binding, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += channel_tag_tag_oldret;
}
/* cb-type */
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &((data)->channel)->cb_type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += channel_tag_tag_oldret;
}
e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, CONS, UT_Sequence, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* type */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->type, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
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
decode_DigestInit(const unsigned char *p, size_t len, DigestInit *data, size_t *size)
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
size_t type_datalen, type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
type_oldlen = len;
if (type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = type_datalen;
e = der_get_utf8string(p, len, &(data)->type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = type_oldlen - type_datalen;
}
{
size_t channel_datalen, channel_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &channel_datalen, &l);
if(e) {
(data)->channel = NULL;
} else {
(data)->channel = calloc(1, sizeof(*(data)->channel));
if ((data)->channel == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
channel_oldlen = len;
if (channel_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = channel_datalen;
{
size_t channel_Tag_datalen, channel_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, CONS, UT_Sequence, &channel_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
channel_Tag_oldlen = len;
if (channel_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = channel_Tag_datalen;
{
size_t cb_type_datalen, cb_type_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &cb_type_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cb_type_oldlen = len;
if (cb_type_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cb_type_datalen;
e = der_get_utf8string(p, len, &((data)->channel)->cb_type, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cb_type_oldlen - cb_type_datalen;
}
{
size_t cb_binding_datalen, cb_binding_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &cb_binding_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
cb_binding_oldlen = len;
if (cb_binding_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = cb_binding_datalen;
e = der_get_utf8string(p, len, &((data)->channel)->cb_binding, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = cb_binding_oldlen - cb_binding_datalen;
}
len = channel_Tag_oldlen - channel_Tag_datalen;
}
len = channel_oldlen - channel_datalen;
}
}
{
size_t hostname_datalen, hostname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &hostname_datalen, &l);
if(e) {
(data)->hostname = NULL;
} else {
(data)->hostname = calloc(1, sizeof(*(data)->hostname));
if ((data)->hostname == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
hostname_oldlen = len;
if (hostname_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hostname_datalen;
{
size_t hostname_Tag_datalen, hostname_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &hostname_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
hostname_Tag_oldlen = len;
if (hostname_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = hostname_Tag_datalen;
e = der_get_utf8string(p, len, (data)->hostname, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = hostname_Tag_oldlen - hostname_Tag_datalen;
}
len = hostname_oldlen - hostname_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestInit(data);
return e;
}

void
free_DigestInit(DigestInit *data)
{
der_free_utf8string(&(data)->type);
if((data)->channel) {
der_free_utf8string(&((data)->channel)->cb_type);
der_free_utf8string(&((data)->channel)->cb_binding);
free((data)->channel);
(data)->channel = NULL;
}
if((data)->hostname) {
der_free_utf8string((data)->hostname);
free((data)->hostname);
(data)->hostname = NULL;
}
}

size_t
length_DigestInit(const DigestInit *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->channel){
size_t Top_tag_oldret = ret;
ret = 0;
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&((data)->channel)->cb_type);
ret += 1 + der_length_len (ret);
ret += channel_tag_tag_oldret;
}
{
size_t channel_tag_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&((data)->channel)->cb_binding);
ret += 1 + der_length_len (ret);
ret += channel_tag_tag_oldret;
}
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->hostname){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->hostname);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_DigestInit(const DigestInit *from, DigestInit *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_utf8string(&(from)->type, &(to)->type)) goto fail;
if((from)->channel) {
(to)->channel = malloc(sizeof(*(to)->channel));
if((to)->channel == NULL) goto fail;
if(der_copy_utf8string(&((from)->channel)->cb_type, &((to)->channel)->cb_type)) goto fail;
if(der_copy_utf8string(&((from)->channel)->cb_binding, &((to)->channel)->cb_binding)) goto fail;
}else
(to)->channel = NULL;
if((from)->hostname) {
(to)->hostname = malloc(sizeof(*(to)->hostname));
if((to)->hostname == NULL) goto fail;
if(der_copy_utf8string((from)->hostname, (to)->hostname)) goto fail;
}else
(to)->hostname = NULL;
return 0;
fail:
free_DigestInit(to);
return ENOMEM;
}

