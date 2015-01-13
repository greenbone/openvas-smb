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
encode_GENERATION(unsigned char *p, size_t len, const GENERATION *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* gen */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, &(data)->gen, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_CONTEXT, CONS, 2, &l);
if (e) return e;
p -= l; len -= l; ret += l;

ret += Top_tag_oldret;
}
/* usec */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_unsigned(p, len, &(data)->usec, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Integer, &l);
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
decode_GENERATION(const unsigned char *p, size_t len, GENERATION *data, size_t *size)
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
size_t usec_datalen, usec_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 1, &usec_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
usec_oldlen = len;
if (usec_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = usec_datalen;
{
size_t usec_Tag_datalen, usec_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &usec_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
usec_Tag_oldlen = len;
if (usec_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = usec_Tag_datalen;
e = der_get_unsigned(p, len, &(data)->usec, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = usec_Tag_oldlen - usec_Tag_datalen;
}
len = usec_oldlen - usec_datalen;
}
{
size_t gen_datalen, gen_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 2, &gen_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
gen_oldlen = len;
if (gen_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = gen_datalen;
{
size_t gen_Tag_datalen, gen_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Integer, &gen_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
gen_Tag_oldlen = len;
if (gen_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = gen_Tag_datalen;
e = der_get_unsigned(p, len, &(data)->gen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = gen_Tag_oldlen - gen_Tag_datalen;
}
len = gen_oldlen - gen_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_GENERATION(data);
return e;
}

void
free_GENERATION(GENERATION *data)
{
free_KerberosTime(&(data)->time);
}

size_t
length_GENERATION(const GENERATION *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += length_KerberosTime(&(data)->time);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned(&(data)->usec);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += der_length_unsigned(&(data)->gen);
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_GENERATION(const GENERATION *from, GENERATION *to)
{
memset(to, 0, sizeof(*to));
if(copy_KerberosTime(&(from)->time, &(to)->time)) goto fail;
*(&(to)->usec) = *(&(from)->usec);
*(&(to)->gen) = *(&(from)->gen);
return 0;
fail:
free_GENERATION(to);
return ENOMEM;
}

