/* Generated from spnego.asn1 */
/* Do not edit */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <krb5-types.h>
#include <spnego_asn1.h>
#include <asn1_err.h>
#include <der.h>
#include <parse_units.h>

int
encode_ContextFlags(unsigned char *p, size_t len, const ContextFlags *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
{
unsigned char c = 0;
int bit_set = 0;
if((data)->integFlag) {
c |= 1<<1;
}
if((data)->confFlag) {
c |= 1<<2;
}
if((data)->anonFlag) {
c |= 1<<3;
}
if((data)->sequenceFlag) {
c |= 1<<4;
}
if((data)->replayFlag) {
c |= 1<<5;
}
if((data)->mutualFlag) {
c |= 1<<6;
}
if((data)->delegFlag) {
c |= 1<<7;
}
if (c != 0 || bit_set) {
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
}
if (len < 1) return ASN1_OVERFLOW;
*p-- = 7;
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
decode_ContextFlags(const unsigned char *p, size_t len, ContextFlags *data, size_t *size)
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
(data)->delegFlag = (*p >> 7) & 1;
(data)->mutualFlag = (*p >> 6) & 1;
(data)->replayFlag = (*p >> 5) & 1;
(data)->sequenceFlag = (*p >> 4) & 1;
(data)->anonFlag = (*p >> 3) & 1;
(data)->confFlag = (*p >> 2) & 1;
(data)->integFlag = (*p >> 1) & 1;
} while(0);
p += len; ret += len;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_ContextFlags(data);
return e;
}

void
free_ContextFlags(ContextFlags *data)
{
}

size_t
length_ContextFlags(const ContextFlags *data)
{
size_t ret = 0;
do {
if((data)->integFlag) { ret += 1; break; }
if((data)->confFlag) { ret += 1; break; }
if((data)->anonFlag) { ret += 1; break; }
if((data)->sequenceFlag) { ret += 1; break; }
if((data)->replayFlag) { ret += 1; break; }
if((data)->mutualFlag) { ret += 1; break; }
if((data)->delegFlag) { ret += 1; break; }
} while(0);
ret += 1;
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_ContextFlags(const ContextFlags *from, ContextFlags *to)
{
memset(to, 0, sizeof(*to));
*(to) = *(from);
return 0;
}

unsigned ContextFlags2int(ContextFlags f)
{
unsigned r = 0;
if(f.delegFlag) r |= (1U << 0);
if(f.mutualFlag) r |= (1U << 1);
if(f.replayFlag) r |= (1U << 2);
if(f.sequenceFlag) r |= (1U << 3);
if(f.anonFlag) r |= (1U << 4);
if(f.confFlag) r |= (1U << 5);
if(f.integFlag) r |= (1U << 6);
return r;
}

ContextFlags int2ContextFlags(unsigned n)
{
	ContextFlags flags;

	flags.delegFlag = (n >> 0) & 1;
	flags.mutualFlag = (n >> 1) & 1;
	flags.replayFlag = (n >> 2) & 1;
	flags.sequenceFlag = (n >> 3) & 1;
	flags.anonFlag = (n >> 4) & 1;
	flags.confFlag = (n >> 5) & 1;
	flags.integFlag = (n >> 6) & 1;
	return flags;
}

static struct units ContextFlags_units[] = {
	{"integFlag",	1U << 6},
	{"confFlag",	1U << 5},
	{"anonFlag",	1U << 4},
	{"sequenceFlag",	1U << 3},
	{"replayFlag",	1U << 2},
	{"mutualFlag",	1U << 1},
	{"delegFlag",	1U << 0},
	{NULL,	0}
};

const struct units * asn1_ContextFlags_units(void){
return ContextFlags_units;
}

