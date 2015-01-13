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
encode_APOptions(unsigned char *p, size_t len, const APOptions *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
{
unsigned char c = 0;
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
c = 0;
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
c = 0;
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
c = 0;
if((data)->mutual_required) {
c |= 1<<5;
}
if((data)->use_session_key) {
c |= 1<<6;
}
if((data)->reserved) {
c |= 1<<7;
}
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
if (len < 1) return ASN1_OVERFLOW;
*p-- = 0;
len -= 1;
ret += 1;
}

e = der_put_length_and_tag (p, len, ret, ASN1_C_UNIV, PRIM, UT_BitString, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_APOptions(const unsigned char *p, size_t len, APOptions *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
{
size_t Top_datalen, Top_oldlen;
e = der_match_tag_and_length(p, len, ASN1_C_UNIV, PRIM, UT_BitString, &Top_datalen, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
Top_oldlen = len;
if (Top_datalen > len) { e = ASN1_OVERRUN; goto fail; }
len = Top_datalen;
if (len < 1) return ASN1_OVERRUN;
p++; len--; ret++;
do {
if (len < 1) break;
(data)->reserved = (*p >> 7) & 1;
(data)->use_session_key = (*p >> 6) & 1;
(data)->mutual_required = (*p >> 5) & 1;
} while(0);
p += len; ret += len;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_APOptions(data);
return e;
}

void
free_APOptions(APOptions *data)
{
}

size_t
length_APOptions(const APOptions *data)
{
size_t ret = 0;
ret += 5;
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_APOptions(const APOptions *from, APOptions *to)
{
memset(to, 0, sizeof(*to));
*(to) = *(from);
return 0;
}

unsigned APOptions2int(APOptions f)
{
unsigned r = 0;
if(f.reserved) r |= (1U << 0);
if(f.use_session_key) r |= (1U << 1);
if(f.mutual_required) r |= (1U << 2);
return r;
}

APOptions int2APOptions(unsigned n)
{
	APOptions flags;

	flags.reserved = (n >> 0) & 1;
	flags.use_session_key = (n >> 1) & 1;
	flags.mutual_required = (n >> 2) & 1;
	return flags;
}

static struct units APOptions_units[] = {
	{"mutual_required",	1U << 2},
	{"use_session_key",	1U << 1},
	{"reserved",	1U << 0},
	{NULL,	0}
};

const struct units * asn1_APOptions_units(void){
return APOptions_units;
}

