/*
 * SPDX-FileCopyrightText: Jeremy Allison 2001
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   Functions to create reasonable random numbers for crypto use.

   Copyright (C) Jeremy Allison 2001
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "includes.h"
#include "system/filesys.h"
#include "lib/crypto/crypto.h"
#include "system/locale.h"

/**
 * @file
 * @brief Random number generation
 */

static unsigned char hash[258];
static uint32_t counter;

static BOOL done_reseed = False;
static void (*reseed_callback)(int *newseed);

/**
 Copy any user given reseed data.
**/

_PUBLIC_ void set_rand_reseed_callback(void (*fn)(int *))
{
	reseed_callback = fn;
	set_need_random_reseed();
}

/**
 * Tell the random number generator it needs to reseed.
 */
_PUBLIC_ void set_need_random_reseed(void)
{
	done_reseed = False;
}

static void get_rand_reseed_data(int *reseed_data)
{
	if (reseed_callback) {
		reseed_callback(reseed_data);
	} else {
		*reseed_data = 0;
	}
}

/**************************************************************** 
 Setup the seed.
*****************************************************************/

static void seed_random_stream(unsigned char *seedval, size_t seedlen)
{
	unsigned char j = 0;
	size_t ind;

	for (ind = 0; ind < 256; ind++)
		hash[ind] = (unsigned char)ind;

	for( ind = 0; ind < 256; ind++) {
		unsigned char tc;

		j += (hash[ind] + seedval[ind%seedlen]);

		tc = hash[ind];
		hash[ind] = hash[j];
		hash[j] = tc;
	}

	hash[256] = 0;
	hash[257] = 0;
}

/**************************************************************** 
 Get datasize bytes worth of random data.
*****************************************************************/

static void get_random_stream(unsigned char *data, size_t datasize)
{
	unsigned char index_i = hash[256];
	unsigned char index_j = hash[257];
	size_t ind;

	for( ind = 0; ind < datasize; ind++) {
		unsigned char tc;
		unsigned char t;

		index_i++;
		index_j += hash[index_i];

		tc = hash[index_i];
		hash[index_i] = hash[index_j];
		hash[index_j] = tc;

		t = hash[index_i] + hash[index_j];
		data[ind] = hash[t];
	}

	hash[256] = index_i;
	hash[257] = index_j;
}

/****************************************************************
 Get a 16 byte hash from the contents of a file.  

 Note that the hash is initialised, because the extra entropy is not
 worth the valgrind pain.
*****************************************************************/

static void do_filehash(const char *fname, unsigned char *the_hash)
{
	unsigned char buf[1011]; /* deliberate weird size */
	unsigned char tmp_md4[16];
	int fd, n;

	ZERO_STRUCT(tmp_md4);

	fd = open(fname,O_RDONLY,0);
	if (fd == -1)
		return;

	while ((n = read(fd, (char *)buf, sizeof(buf))) > 0) {
		mdfour(tmp_md4, buf, n);
		for (n=0;n<16;n++)
			the_hash[n] ^= tmp_md4[n];
	}
	close(fd);
}

/**************************************************************
 Try and get a good random number seed. Try a number of
 different factors. Firstly, try /dev/urandom - use if exists.

 We use /dev/urandom as a read of /dev/random can block if
 the entropy pool dries up. This leads clients to timeout
 or be very slow on connect.

 If we can't use /dev/urandom then seed the stream random generator
 above...
**************************************************************/

static int do_reseed(BOOL use_fd, int fd)
{
	unsigned char seed_inbuf[40];
	uint32_t v1, v2; struct timeval tval; pid_t mypid;
	int reseed_data = 0;

	if (use_fd) {
		if (fd != -1)
			return fd;

		fd = open( "/dev/urandom", O_RDONLY,0);
		if(fd >= 0)
			return fd;
	}

	/* Add in some secret file contents */

	do_filehash("/etc/shadow", &seed_inbuf[0]);

	/*
	 * Add the counter, time of day, and pid.
	 */

	GetTimeOfDay(&tval);
	mypid = getpid();
	v1 = (counter++) + mypid + tval.tv_sec;
	v2 = (counter++) * mypid + tval.tv_usec;

	SIVAL(seed_inbuf, 32, v1 ^ IVAL(seed_inbuf, 32));
	SIVAL(seed_inbuf, 36, v2 ^ IVAL(seed_inbuf, 36));

	/*
	 * Add any user-given reseed data.
	 */

	get_rand_reseed_data(&reseed_data);
	if (reseed_data) {
		size_t i;
		for (i = 0; i < sizeof(seed_inbuf); i++)
			seed_inbuf[i] ^= ((char *)(&reseed_data))[i % sizeof(reseed_data)];
	}

	seed_random_stream(seed_inbuf, sizeof(seed_inbuf));

	return -1;
}

/**
 Interface to the (hopefully) good crypto random number generator.
**/
_PUBLIC_ void generate_random_buffer(uint8_t *out, int len)
{
	static int urand_fd = -1;
	unsigned char md4_buf[64];
	unsigned char tmp_buf[16];
	unsigned char *p;

	if(!done_reseed) {
		urand_fd = do_reseed(True, urand_fd);
		done_reseed = True;
	}

	if (urand_fd != -1 && len > 0) {

		if (read(urand_fd, out, len) == len)
			return; /* len bytes of random data read from urandom. */

		/* Read of urand error, drop back to non urand method. */
		close(urand_fd);
		urand_fd = -1;
		do_reseed(False, -1);
		done_reseed = True;
	}

	/*
	 * Generate random numbers in chunks of 64 bytes,
	 * then md4 them & copy to the output buffer.
	 * This way the raw state of the stream is never externally
	 * seen.
	 */

	p = out;
	while(len > 0) {
		int copy_len = len > 16 ? 16 : len;

		get_random_stream(md4_buf, sizeof(md4_buf));
		mdfour(tmp_buf, md4_buf, sizeof(md4_buf));
		memcpy(p, tmp_buf, copy_len);
		p += copy_len;
		len -= copy_len;
	}
}

/**
  generate a single random uint32_t
**/
_PUBLIC_ uint32_t generate_random(void)
{
	uint8_t v[4];
	generate_random_buffer(v, 4);
	return IVAL(v, 0);
}


/**
  very basic password quality checker
**/
_PUBLIC_ BOOL check_password_quality(const char *s)
{
	int has_digit=0, has_capital=0, has_lower=0;
	while (*s) {
		if (isdigit((unsigned char)*s)) {
			has_digit++;
		} else if (isupper((unsigned char)*s)) {
			has_capital++;
		} else if (islower((unsigned char)*s)) {
			has_lower++;
		}
		s++;
	}

	return has_digit && has_lower && has_capital;
}

/**
 Use the random number generator to generate a random string.
**/

_PUBLIC_ char *generate_random_str_list(TALLOC_CTX *mem_ctx, size_t len, const char *list)
{
	size_t i;
	size_t list_len = strlen(list);

	char *retstr = talloc_array(mem_ctx, char, len + 1);
	if (!retstr) return NULL;

	generate_random_buffer((uint8_t *)retstr, len);
	for (i = 0; i < len; i++) {
		retstr[i] = list[retstr[i] % list_len];
	}
	retstr[i] = '\0';

	return retstr;
}

/**
 * Generate a random text string consisting of the specified length.
 * The returned string will be allocated.
 *
 * Characters used are: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+_-#.,
 */

_PUBLIC_ char *generate_random_str(TALLOC_CTX *mem_ctx, size_t len)
{
	char *retstr;
	const char *c_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+_-#.,";

again:
	retstr = generate_random_str_list(mem_ctx, len, c_list);
	if (!retstr) return NULL;

	/* we need to make sure the random string passes basic quality tests
	   or it might be rejected by windows as a password */
	if (len >= 7 && !check_password_quality(retstr)) {
		talloc_free(retstr);
		goto again;
	}

	return retstr;
}
