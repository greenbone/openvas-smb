/* Generated from hdb.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <hdb_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_Event(unsigned char *p, size_t len, const Event *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* principal */
if((data)->principal) {
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_Principal(p, len, (data)->principal, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 1, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* time */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = encode_KerberosTime(p, len, &(data)->time, &l);
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
decode_Event(const unsigned char *p, size_t len, Event *data, size_t *size)
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
size_t time_datalen, time_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &time_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
time_oldlen = len;
if (time_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = time_datalen;
e = decode_KerberosTime(p, len, &(data)->time, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = time_oldlen - time_datalen;
}
{
size_t principal_datalen, principal_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &principal_datalen, &l);
if(e) {
(data)->principal = NULL;
} else {
(data)->principal = calloc(1, sizeof(*(data)->principal));
if ((data)->principal == NULL) { e = ENOMEM; goto fail; }
p += l; len -= l; ret += l;
principal_oldlen = len;
if (principal_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = principal_datalen;
e = decode_Principal(p, len, (data)->principal, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = principal_oldlen - principal_datalen;
}
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_Event(data);
return e;
}

void
free_Event(Event *data)
{
free_KerberosTime(&(data)->time);
if((data)->principal) {
free_Principal((data)->principal);
free((data)->principal);
(data)->principal = NULL;
}
}

size_t
length_Event(const Event *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->time);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
if((data)->principal){
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_Principal((data)->principal);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_Event(const Event *from, Event *to)
{
memset(to, 0, sizeof(*to));
if(copy_KerberosTime(&(from)->time, &(to)->time)) goto fail;
if((from)->principal) {
(to)->principal = malloc(sizeof(*(to)->principal));
if((to)->principal == NULL) goto fail;
if(copy_Principal((from)->principal, (to)->principal)) goto fail;
}else
(to)->principal = NULL;
return 0;
fail:
free_Event(to);
return ENOMEM;
}

