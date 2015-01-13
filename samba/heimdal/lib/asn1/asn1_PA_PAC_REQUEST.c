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
encode_PA_PAC_REQUEST(unsigned char *p, size_t len, const PA_PAC_REQUEST *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
/* include-pac */
{
size_t Top_tag_oldret = ret;
ret = 0;
e = der_put_boolean(p, len, &(data)->include_pac, &l);
if (e) return e;
p -= l; len -= l; ret += l;

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_Boolean, &l);
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
decode_PA_PAC_REQUEST(const unsigned char *p, size_t len, PA_PAC_REQUEST *data, size_t *size)
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
size_t include_pac_datalen, include_pac_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_CONTEXT, CONS, 0, &include_pac_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
include_pac_oldlen = len;
if (include_pac_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = include_pac_datalen;
{
size_t include_pac_Tag_datalen, include_pac_Tag_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_Boolean, &include_pac_Tag_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
include_pac_Tag_oldlen = len;
if (include_pac_Tag_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = include_pac_Tag_datalen;
e = der_get_boolean(p, len, &(data)->include_pac, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
len = include_pac_Tag_oldlen - include_pac_Tag_datalen;
}
len = include_pac_oldlen - include_pac_datalen;
}
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_PA_PAC_REQUEST(data);
return e;
}

void
free_PA_PAC_REQUEST(PA_PAC_REQUEST *data)
{
}

size_t
length_PA_PAC_REQUEST(const PA_PAC_REQUEST *data)
{
size_t ret = 0;
{
size_t Top_tag_oldret = ret;
ret = 0;
ret += 1;
ret += 1 + der_length_len (ret);
ret += 1 + der_length_len (ret);
ret += Top_tag_oldret;
}
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_PA_PAC_REQUEST(const PA_PAC_REQUEST *from, PA_PAC_REQUEST *to)
{
memset(to, 0, sizeof(*to));
*(&(to)->include_pac) = *(&(from)->include_pac);
return 0;
}

