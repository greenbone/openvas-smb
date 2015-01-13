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
encode_KerberosTime(unsigned char *p, size_t len, const KerberosTime *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
e = der_put_generalized_time(p, len, data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_GeneralizedTime, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_KerberosTime(const unsigned char *p, size_t len, KerberosTime *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_GeneralizedTime, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
e = der_get_generalized_time(p, len, data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_KerberosTime(data);
return e;
}

void
free_KerberosTime(KerberosTime *data)
{
}

size_t
length_KerberosTime(const KerberosTime *data)
{
size_t ret = 0;
ret += der_length_generalized_time(data);
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_KerberosTime(const KerberosTime *from, KerberosTime *to)
{
memset(to, 0, sizeof(*to));
*(to) = *(from);
return 0;
}

