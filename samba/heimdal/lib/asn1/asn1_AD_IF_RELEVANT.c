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
encode_AD_IF_RELEVANT(unsigned char *p, size_t len, const AD_IF_RELEVANT *data, size_t *size)
{
size_t ret = 0;
size_t l;
int i, e;

i = 0;
e = encode_AuthorizationData(p, len, data, &l);
if (e) return e;
p -= l; len -= l; ret += l;

*size = ret;
return 0;
}

int
decode_AD_IF_RELEVANT(const unsigned char *p, size_t len, AD_IF_RELEVANT *data, size_t *size)
{
size_t ret = 0;
size_t l;
int e;

memset(data, 0, sizeof(*data));
e = decode_AuthorizationData(p, len, data, &l);
if(e) goto fail;
p += l; len -= l; ret += l;
if(size) *size = ret;
return 0;
fail:
free_AD_IF_RELEVANT(data);
return e;
}

void
free_AD_IF_RELEVANT(AD_IF_RELEVANT *data)
{
free_AuthorizationData(data);
}

size_t
length_AD_IF_RELEVANT(const AD_IF_RELEVANT *data)
{
size_t ret = 0;
ret += length_AuthorizationData(data);
return ret;
}

int
copy_AD_IF_RELEVANT(const AD_IF_RELEVANT *from, AD_IF_RELEVANT *to)
{
memset(to, 0, sizeof(*to));
if(copy_AuthorizationData(from, to)) goto fail;
return 0;
fail:
free_AD_IF_RELEVANT(to);
return ENOMEM;
}

