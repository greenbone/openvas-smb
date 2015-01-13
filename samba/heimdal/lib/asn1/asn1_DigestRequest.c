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
encode_DigestRequest(unsigned char *p, size_t len, const DigestRequest *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* opaque */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->opaque, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
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

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 9, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* identifier */
if((data)->identifier) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->identifier, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 8, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* qop */
if((data)->qop) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->qop, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 7, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* nonceCount */
if((data)->nonceCount) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->nonceCount, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 6, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* clientNonce */
if((data)->clientNonce) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->clientNonce, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 5, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* serverNonce */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->serverNonce, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* uri */
if((data)->uri) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->uri, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 4, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* method */
if((data)->method) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->method, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 3, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* realm */
if((data)->realm) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->realm, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* authentication-user */
if((data)->authentication_user) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Principal(p, len, (data)->authentication_user, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* authid */
if((data)->authid) {
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, (data)->authid, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 0, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* username */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->username, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* digest */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_utf8string(p, len, &(data)->digest, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_UTF8String, &l);
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
decode_DigestRequest(const unsigned char *p, size_t len, DigestRequest *data, size_t *size)
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
size_t digest_datalen, digest_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &digest_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
digest_oldlen = len;
if (digest_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = digest_datalen;
e = der_get_utf8string(p, len, &(data)->digest, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = digest_oldlen - digest_datalen;
}
{
size_t username_datalen, username_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &username_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
username_oldlen = len;
if (username_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = username_datalen;
e = der_get_utf8string(p, len, &(data)->username, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = username_oldlen - username_datalen;
}
{
size_t authid_datalen, authid_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &authid_datalen, &l);
if(e) {
(data)->authid = NULL;
} else {
(data)->authid = calloc(1, sizeof(*(data)->authid));
if ((data)->authid == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
authid_oldlen = len;
if (authid_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authid_datalen;
{
size_t authid_Tag_datalen, authid_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &authid_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
authid_Tag_oldlen = len;
if (authid_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authid_Tag_datalen;
e = der_get_utf8string(p, len, (data)->authid, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authid_Tag_oldlen - authid_Tag_datalen;
}
len = authid_oldlen - authid_datalen;
}
}
{
size_t authentication_user_datalen, authentication_user_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &authentication_user_datalen, &l);
if(e) {
(data)->authentication_user = NULL;
} else {
(data)->authentication_user = calloc(1, sizeof(*(data)->authentication_user));
if ((data)->authentication_user == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
authentication_user_oldlen = len;
if (authentication_user_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = authentication_user_datalen;
e = decode_Principal(p, len, (data)->authentication_user, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = authentication_user_oldlen - authentication_user_datalen;
}
}
{
size_t realm_datalen, realm_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &realm_datalen, &l);
if(e) {
(data)->realm = NULL;
} else {
(data)->realm = calloc(1, sizeof(*(data)->realm));
if ((data)->realm == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
realm_oldlen = len;
if (realm_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = realm_datalen;
{
size_t realm_Tag_datalen, realm_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &realm_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
realm_Tag_oldlen = len;
if (realm_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = realm_Tag_datalen;
e = der_get_utf8string(p, len, (data)->realm, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = realm_Tag_oldlen - realm_Tag_datalen;
}
len = realm_oldlen - realm_datalen;
}
}
{
size_t method_datalen, method_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 3, &method_datalen, &l);
if(e) {
(data)->method = NULL;
} else {
(data)->method = calloc(1, sizeof(*(data)->method));
if ((data)->method == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
method_oldlen = len;
if (method_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = method_datalen;
{
size_t method_Tag_datalen, method_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &method_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
method_Tag_oldlen = len;
if (method_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = method_Tag_datalen;
e = der_get_utf8string(p, len, (data)->method, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = method_Tag_oldlen - method_Tag_datalen;
}
len = method_oldlen - method_datalen;
}
}
{
size_t uri_datalen, uri_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 4, &uri_datalen, &l);
if(e) {
(data)->uri = NULL;
} else {
(data)->uri = calloc(1, sizeof(*(data)->uri));
if ((data)->uri == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
uri_oldlen = len;
if (uri_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = uri_datalen;
{
size_t uri_Tag_datalen, uri_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &uri_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
uri_Tag_oldlen = len;
if (uri_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = uri_Tag_datalen;
e = der_get_utf8string(p, len, (data)->uri, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = uri_Tag_oldlen - uri_Tag_datalen;
}
len = uri_oldlen - uri_datalen;
}
}
{
size_t serverNonce_datalen, serverNonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &serverNonce_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
serverNonce_oldlen = len;
if (serverNonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = serverNonce_datalen;
e = der_get_utf8string(p, len, &(data)->serverNonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = serverNonce_oldlen - serverNonce_datalen;
}
{
size_t clientNonce_datalen, clientNonce_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 5, &clientNonce_datalen, &l);
if(e) {
(data)->clientNonce = NULL;
} else {
(data)->clientNonce = calloc(1, sizeof(*(data)->clientNonce));
if ((data)->clientNonce == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
clientNonce_oldlen = len;
if (clientNonce_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = clientNonce_datalen;
{
size_t clientNonce_Tag_datalen, clientNonce_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &clientNonce_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
clientNonce_Tag_oldlen = len;
if (clientNonce_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = clientNonce_Tag_datalen;
e = der_get_utf8string(p, len, (data)->clientNonce, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = clientNonce_Tag_oldlen - clientNonce_Tag_datalen;
}
len = clientNonce_oldlen - clientNonce_datalen;
}
}
{
size_t nonceCount_datalen, nonceCount_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 6, &nonceCount_datalen, &l);
if(e) {
(data)->nonceCount = NULL;
} else {
(data)->nonceCount = calloc(1, sizeof(*(data)->nonceCount));
if ((data)->nonceCount == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
nonceCount_oldlen = len;
if (nonceCount_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonceCount_datalen;
{
size_t nonceCount_Tag_datalen, nonceCount_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &nonceCount_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
nonceCount_Tag_oldlen = len;
if (nonceCount_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = nonceCount_Tag_datalen;
e = der_get_utf8string(p, len, (data)->nonceCount, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = nonceCount_Tag_oldlen - nonceCount_Tag_datalen;
}
len = nonceCount_oldlen - nonceCount_datalen;
}
}
{
size_t qop_datalen, qop_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 7, &qop_datalen, &l);
if(e) {
(data)->qop = NULL;
} else {
(data)->qop = calloc(1, sizeof(*(data)->qop));
if ((data)->qop == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
qop_oldlen = len;
if (qop_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = qop_datalen;
{
size_t qop_Tag_datalen, qop_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &qop_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
qop_Tag_oldlen = len;
if (qop_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = qop_Tag_datalen;
e = der_get_utf8string(p, len, (data)->qop, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = qop_Tag_oldlen - qop_Tag_datalen;
}
len = qop_oldlen - qop_datalen;
}
}
{
size_t identifier_datalen, identifier_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 8, &identifier_datalen, &l);
if(e) {
(data)->identifier = NULL;
} else {
(data)->identifier = calloc(1, sizeof(*(data)->identifier));
if ((data)->identifier == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
identifier_oldlen = len;
if (identifier_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = identifier_datalen;
{
size_t identifier_Tag_datalen, identifier_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &identifier_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
identifier_Tag_oldlen = len;
if (identifier_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = identifier_Tag_datalen;
e = der_get_utf8string(p, len, (data)->identifier, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = identifier_Tag_oldlen - identifier_Tag_datalen;
}
len = identifier_oldlen - identifier_datalen;
}
}
{
size_t hostname_datalen, hostname_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 9, &hostname_datalen, &l);
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
{
size_t opaque_datalen, opaque_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_UTF8String, &opaque_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
opaque_oldlen = len;
if (opaque_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = opaque_datalen;
e = der_get_utf8string(p, len, &(data)->opaque, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = opaque_oldlen - opaque_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_DigestRequest(data);
return e;
}

void
free_DigestRequest(DigestRequest *data)
{
der_free_utf8string(&(data)->type);
der_free_utf8string(&(data)->digest);
der_free_utf8string(&(data)->username);
if((data)->authid) {
der_free_utf8string((data)->authid);
free((data)->authid);
(data)->authid = NULL;
}
if((data)->authentication_user) {
free_Principal((data)->authentication_user);
free((data)->authentication_user);
(data)->authentication_user = NULL;
}
if((data)->realm) {
der_free_utf8string((data)->realm);
free((data)->realm);
(data)->realm = NULL;
}
if((data)->method) {
der_free_utf8string((data)->method);
free((data)->method);
(data)->method = NULL;
}
if((data)->uri) {
der_free_utf8string((data)->uri);
free((data)->uri);
(data)->uri = NULL;
}
der_free_utf8string(&(data)->serverNonce);
if((data)->clientNonce) {
der_free_utf8string((data)->clientNonce);
free((data)->clientNonce);
(data)->clientNonce = NULL;
}
if((data)->nonceCount) {
der_free_utf8string((data)->nonceCount);
free((data)->nonceCount);
(data)->nonceCount = NULL;
}
if((data)->qop) {
der_free_utf8string((data)->qop);
free((data)->qop);
(data)->qop = NULL;
}
if((data)->identifier) {
der_free_utf8string((data)->identifier);
free((data)->identifier);
(data)->identifier = NULL;
}
if((data)->hostname) {
der_free_utf8string((data)->hostname);
free((data)->hostname);
(data)->hostname = NULL;
}
der_free_utf8string(&(data)->opaque);
}

size_t
length_DigestRequest(const DigestRequest *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->type);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->digest);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->username);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->authid){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->authid);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->authentication_user){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Principal((data)->authentication_user);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->realm){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->realm);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->method){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->method);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->uri){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->uri);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->serverNonce);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->clientNonce){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->clientNonce);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->nonceCount){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->nonceCount);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->qop){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->qop);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->identifier){
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string((data)->identifier);
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
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_utf8string(&(data)->opaque);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_DigestRequest(const DigestRequest *from, DigestRequest *to)
{
memset(to, 0, sizeof(*to));
if(der_copy_utf8string(&(from)->type, &(to)->type)) goto fail;
if(der_copy_utf8string(&(from)->digest, &(to)->digest)) goto fail;
if(der_copy_utf8string(&(from)->username, &(to)->username)) goto fail;
if((from)->authid) {
(to)->authid = malloc(sizeof(*(to)->authid));
if((to)->authid == NULL) goto fail;
if(der_copy_utf8string((from)->authid, (to)->authid)) goto fail;
}else
(to)->authid = NULL;
if((from)->authentication_user) {
(to)->authentication_user = malloc(sizeof(*(to)->authentication_user));
if((to)->authentication_user == NULL) goto fail;
if(copy_Principal((from)->authentication_user, (to)->authentication_user)) goto fail;
}else
(to)->authentication_user = NULL;
if((from)->realm) {
(to)->realm = malloc(sizeof(*(to)->realm));
if((to)->realm == NULL) goto fail;
if(der_copy_utf8string((from)->realm, (to)->realm)) goto fail;
}else
(to)->realm = NULL;
if((from)->method) {
(to)->method = malloc(sizeof(*(to)->method));
if((to)->method == NULL) goto fail;
if(der_copy_utf8string((from)->method, (to)->method)) goto fail;
}else
(to)->method = NULL;
if((from)->uri) {
(to)->uri = malloc(sizeof(*(to)->uri));
if((to)->uri == NULL) goto fail;
if(der_copy_utf8string((from)->uri, (to)->uri)) goto fail;
}else
(to)->uri = NULL;
if(der_copy_utf8string(&(from)->serverNonce, &(to)->serverNonce)) goto fail;
if((from)->clientNonce) {
(to)->clientNonce = malloc(sizeof(*(to)->clientNonce));
if((to)->clientNonce == NULL) goto fail;
if(der_copy_utf8string((from)->clientNonce, (to)->clientNonce)) goto fail;
}else
(to)->clientNonce = NULL;
if((from)->nonceCount) {
(to)->nonceCount = malloc(sizeof(*(to)->nonceCount));
if((to)->nonceCount == NULL) goto fail;
if(der_copy_utf8string((from)->nonceCount, (to)->nonceCount)) goto fail;
}else
(to)->nonceCount = NULL;
if((from)->qop) {
(to)->qop = malloc(sizeof(*(to)->qop));
if((to)->qop == NULL) goto fail;
if(der_copy_utf8string((from)->qop, (to)->qop)) goto fail;
}else
(to)->qop = NULL;
if((from)->identifier) {
(to)->identifier = malloc(sizeof(*(to)->identifier));
if((to)->identifier == NULL) goto fail;
if(der_copy_utf8string((from)->identifier, (to)->identifier)) goto fail;
}else
(to)->identifier = NULL;
if((from)->hostname) {
(to)->hostname = malloc(sizeof(*(to)->hostname));
if((to)->hostname == NULL) goto fail;
if(der_copy_utf8string((from)->hostname, (to)->hostname)) goto fail;
}else
(to)->hostname = NULL;
if(der_copy_utf8string(&(from)->opaque, &(to)->opaque)) goto fail;
return 0;
fail:
free_DigestRequest(to);
return ENOMEM;
}

