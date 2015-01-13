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
encode_HDBFlags(unsigned char *p, size_t len, const HDBFlags *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
{
unsigned char c = 0;
int bit_set = 0;
if((data)->allow_digest) {
c |= 1<<7;
}
if (c != 0 || bit_set) {
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
c = 0;
bit_set = 1;
}
if((data)->allow_kerberos4) {
c |= 1<<0;
}
if((data)->trusted_for_delegation) {
c |= 1<<1;
}
if((data)->immutable) {
c |= 1<<2;
}
if((data)->user_to_user) {
c |= 1<<3;
}
if((data)->ok_as_delegate) {
c |= 1<<4;
}
if((data)->require_hwauth) {
c |= 1<<5;
}
if((data)->change_pw) {
c |= 1<<6;
}
if((data)->require_preauth) {
c |= 1<<7;
}
if (c != 0 || bit_set) {
if (len < 1) return ASN1_OVERFLOW;
*p-- = c; len--; ret++;
c = 0;
bit_set = 1;
}
if((data)->invalid) {
c |= 1<<0;
}
if((data)->client) {
c |= 1<<1;
}
if((data)->server) {
c |= 1<<2;
}
if((data)->postdate) {
c |= 1<<3;
}
if((data)->renewable) {
c |= 1<<4;
}
if((data)->proxiable) {
c |= 1<<5;
}
if((data)->forwardable) {
c |= 1<<6;
}
if((data)->initial) {
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
decode_HDBFlags(const unsigned char *p, size_t len, HDBFlags *data, size_t *size)
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
(data)->initial = (*p >> 7) & 1;
(data)->forwardable = (*p >> 6) & 1;
(data)->proxiable = (*p >> 5) & 1;
(data)->renewable = (*p >> 4) & 1;
(data)->postdate = (*p >> 3) & 1;
(data)->server = (*p >> 2) & 1;
(data)->client = (*p >> 1) & 1;
(data)->invalid = (*p >> 0) & 1;
p++; len--; ret++;
if (len < 1) break;
(data)->require_preauth = (*p >> 7) & 1;
(data)->change_pw = (*p >> 6) & 1;
(data)->require_hwauth = (*p >> 5) & 1;
(data)->ok_as_delegate = (*p >> 4) & 1;
(data)->user_to_user = (*p >> 3) & 1;
(data)->immutable = (*p >> 2) & 1;
(data)->trusted_for_delegation = (*p >> 1) & 1;
(data)->allow_kerberos4 = (*p >> 0) & 1;
p++; len--; ret++;
if (len < 1) break;
(data)->allow_digest = (*p >> 7) & 1;
} while(0);
p += len; ret += len;
len = Top_oldlen - Top_datalen;
}
if(size) *size = ret;
return 0;
fail:
free_HDBFlags(data);
return e;
}

void
free_HDBFlags(HDBFlags *data)
{
}

size_t
length_HDBFlags(const HDBFlags *data)
{
size_t ret = 0;
do {
if((data)->allow_digest) { ret += 3; break; }
if((data)->allow_kerberos4) { ret += 2; break; }
if((data)->trusted_for_delegation) { ret += 2; break; }
if((data)->immutable) { ret += 2; break; }
if((data)->user_to_user) { ret += 2; break; }
if((data)->ok_as_delegate) { ret += 2; break; }
if((data)->require_hwauth) { ret += 2; break; }
if((data)->change_pw) { ret += 2; break; }
if((data)->require_preauth) { ret += 2; break; }
if((data)->invalid) { ret += 1; break; }
if((data)->client) { ret += 1; break; }
if((data)->server) { ret += 1; break; }
if((data)->postdate) { ret += 1; break; }
if((data)->renewable) { ret += 1; break; }
if((data)->proxiable) { ret += 1; break; }
if((data)->forwardable) { ret += 1; break; }
if((data)->initial) { ret += 1; break; }
} while(0);
ret += 1;
ret += 1 + der_length_len (ret);
return ret;
}

int
copy_HDBFlags(const HDBFlags *from, HDBFlags *to)
{
memset(to, 0, sizeof(*to));
*(to) = *(from);
return 0;
}

unsigned HDBFlags2int(HDBFlags f)
{
unsigned r = 0;
if(f.initial) r |= (1U << 0);
if(f.forwardable) r |= (1U << 1);
if(f.proxiable) r |= (1U << 2);
if(f.renewable) r |= (1U << 3);
if(f.postdate) r |= (1U << 4);
if(f.server) r |= (1U << 5);
if(f.client) r |= (1U << 6);
if(f.invalid) r |= (1U << 7);
if(f.require_preauth) r |= (1U << 8);
if(f.change_pw) r |= (1U << 9);
if(f.require_hwauth) r |= (1U << 10);
if(f.ok_as_delegate) r |= (1U << 11);
if(f.user_to_user) r |= (1U << 12);
if(f.immutable) r |= (1U << 13);
if(f.trusted_for_delegation) r |= (1U << 14);
if(f.allow_kerberos4) r |= (1U << 15);
if(f.allow_digest) r |= (1U << 16);
return r;
}

HDBFlags int2HDBFlags(unsigned n)
{
	HDBFlags flags;

	flags.initial = (n >> 0) & 1;
	flags.forwardable = (n >> 1) & 1;
	flags.proxiable = (n >> 2) & 1;
	flags.renewable = (n >> 3) & 1;
	flags.postdate = (n >> 4) & 1;
	flags.server = (n >> 5) & 1;
	flags.client = (n >> 6) & 1;
	flags.invalid = (n >> 7) & 1;
	flags.require_preauth = (n >> 8) & 1;
	flags.change_pw = (n >> 9) & 1;
	flags.require_hwauth = (n >> 10) & 1;
	flags.ok_as_delegate = (n >> 11) & 1;
	flags.user_to_user = (n >> 12) & 1;
	flags.immutable = (n >> 13) & 1;
	flags.trusted_for_delegation = (n >> 14) & 1;
	flags.allow_kerberos4 = (n >> 15) & 1;
	flags.allow_digest = (n >> 16) & 1;
	return flags;
}

static struct units HDBFlags_units[] = {
	{"allow_digest",	1U << 16},
	{"allow_kerberos4",	1U << 15},
	{"trusted_for_delegation",	1U << 14},
	{"immutable",	1U << 13},
	{"user_to_user",	1U << 12},
	{"ok_as_delegate",	1U << 11},
	{"require_hwauth",	1U << 10},
	{"change_pw",	1U << 9},
	{"require_preauth",	1U << 8},
	{"invalid",	1U << 7},
	{"client",	1U << 6},
	{"server",	1U << 5},
	{"postdate",	1U << 4},
	{"renewable",	1U << 3},
	{"proxiable",	1U << 2},
	{"forwardable",	1U << 1},
	{"initial",	1U << 0},
	{NULL,	0}
};

const struct units * asn1_HDBFlags_units(void){
return HDBFlags_units;
}

