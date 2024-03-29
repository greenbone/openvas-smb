/*
 * SPDX-FileCopyrightText: Stefan (metze) Metzmacher	2002   
 * SPDX-FileCopyrightText: Andrew Tridgell 		1992-2004
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.
   time handling functions

   Copyright (C) Andrew Tridgell 		1992-2004
   Copyright (C) Stefan (metze) Metzmacher	2002   

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
#include "system/time.h"

/**
 * @file
 * @brief time handling functions
 */

#ifndef TIME_T_MIN
/* we use 0 here, because (time_t)-1 means error */
#define TIME_T_MIN 0
#endif

/*
 * we use the INT32_MAX here as on 64 bit systems,
 * gmtime() fails with INT64_MAX
 */

#ifndef TIME_T_MAX
#define TIME_T_MAX MIN(INT32_MAX,_TYPE_MAXIMUM(time_t))
#endif

/**
 External access to time_t_min and time_t_max.
**/
_PUBLIC_ time_t get_time_t_max(void)
{
	return TIME_T_MAX;
}

/**
a gettimeofday wrapper
**/
_PUBLIC_ void GetTimeOfDay(struct timeval *tval)
{
#ifdef HAVE_GETTIMEOFDAY_TZ
	gettimeofday(tval,NULL);
#else
	gettimeofday(tval);
#endif
}


#define TIME_FIXUP_CONSTANT 11644473600LL

/**
interpret an 8 byte "filetime" structure to a time_t
It's originally in "100ns units since jan 1st 1601"
**/
_PUBLIC_ time_t nt_time_to_unix(NTTIME nt)
{
	if (nt == 0) {
		return 0;
	}
	if (nt == -1LL) {
		return (time_t)-1;
	}
	nt += 1000*1000*10/2;
	nt /= 1000*1000*10;
	nt -= TIME_FIXUP_CONSTANT;

	if (TIME_T_MIN > nt || nt > TIME_T_MAX) {
		return 0;
	}

	return (time_t)nt;
}


/**
put a 8 byte filetime from a time_t
This takes GMT as input
**/
_PUBLIC_ void unix_to_nt_time(NTTIME *nt, time_t t)
{
	uint64_t t2; 

	if (t == (time_t)-1) {
		*nt = (NTTIME)-1LL;
		return;
	}		
	if (t == 0) {
		*nt = 0;
		return;
	}		

	t2 = t;
	t2 += TIME_FIXUP_CONSTANT;
	t2 *= 1000*1000*10;

	*nt = t2;
}


/**
check if it's a null unix time
**/
_PUBLIC_ BOOL null_time(time_t t)
{
	return t == 0 || 
		t == (time_t)0xFFFFFFFF || 
		t == (time_t)-1;
}


/**
check if it's a null NTTIME
**/
_PUBLIC_ BOOL null_nttime(NTTIME t)
{
	return t == 0 || t == (NTTIME)-1;
}

/*******************************************************************
  create a 16 bit dos packed date
********************************************************************/
static uint16_t make_dos_date1(struct tm *t)
{
	uint16_t ret=0;
	ret = (((unsigned int)(t->tm_mon+1)) >> 3) | ((t->tm_year-80) << 1);
	ret = ((ret&0xFF)<<8) | (t->tm_mday | (((t->tm_mon+1) & 0x7) << 5));
	return ret;
}

/*******************************************************************
  create a 16 bit dos packed time
********************************************************************/
static uint16_t make_dos_time1(struct tm *t)
{
	uint16_t ret=0;
	ret = ((((unsigned int)t->tm_min >> 3)&0x7) | (((unsigned int)t->tm_hour) << 3));
	ret = ((ret&0xFF)<<8) | ((t->tm_sec/2) | ((t->tm_min & 0x7) << 5));
	return ret;
}

/*******************************************************************
  create a 32 bit dos packed date/time from some parameters
  This takes a GMT time and returns a packed localtime structure
********************************************************************/
static uint32_t make_dos_date(time_t unixdate, int zone_offset)
{
	struct tm tm, *tm_p;
	uint32_t ret=0;

	if (unixdate == 0) {
		return 0;
	}

	unixdate -= zone_offset;

	tm_p = gmtime_r(&unixdate, &tm);
	if (!tm_p) {
		return 0xFFFFFFFF;
	}

	ret = make_dos_date1(&tm);
	ret = ((ret&0xFFFF)<<16) | make_dos_time1(&tm);

	return ret;
}

/**
put a dos date into a buffer (time/date format)
This takes GMT time and puts local time in the buffer
**/
_PUBLIC_ void push_dos_date(uint8_t *buf, int offset, time_t unixdate, int zone_offset)
{
	uint32_t x = make_dos_date(unixdate, zone_offset);
	SIVAL(buf,offset,x);
}

/**
put a dos date into a buffer (date/time format)
This takes GMT time and puts local time in the buffer
**/
_PUBLIC_ void push_dos_date2(uint8_t *buf,int offset,time_t unixdate, int zone_offset)
{
	uint32_t x;
	x = make_dos_date(unixdate, zone_offset);
	x = ((x&0xFFFF)<<16) | ((x&0xFFFF0000)>>16);
	SIVAL(buf,offset,x);
}

/**
put a dos 32 bit "unix like" date into a buffer. This routine takes
GMT and converts it to LOCAL time before putting it (most SMBs assume
localtime for this sort of date)
**/
_PUBLIC_ void push_dos_date3(uint8_t *buf,int offset,time_t unixdate, int zone_offset)
{
	if (!null_time(unixdate)) {
		unixdate -= zone_offset;
	}
	SIVAL(buf,offset,unixdate);
}

/*******************************************************************
  interpret a 32 bit dos packed date/time to some parameters
********************************************************************/
static void interpret_dos_date(uint32_t date,int *year,int *month,int *day,int *hour,int *minute,int *second)
{
	uint32_t p0,p1,p2,p3;

	p0=date&0xFF; p1=((date&0xFF00)>>8)&0xFF; 
	p2=((date&0xFF0000)>>16)&0xFF; p3=((date&0xFF000000)>>24)&0xFF;

	*second = 2*(p0 & 0x1F);
	*minute = ((p0>>5)&0xFF) + ((p1&0x7)<<3);
	*hour = (p1>>3)&0xFF;
	*day = (p2&0x1F);
	*month = ((p2>>5)&0xFF) + ((p3&0x1)<<3) - 1;
	*year = ((p3>>1)&0xFF) + 80;
}

/**
  create a unix date (int GMT) from a dos date (which is actually in
  localtime)
**/
_PUBLIC_ time_t pull_dos_date(const uint8_t *date_ptr, int zone_offset)
{
	uint32_t dos_date=0;
	struct tm t;
	time_t ret;

	dos_date = IVAL(date_ptr,0);

	if (dos_date == 0) return (time_t)0;
  
	interpret_dos_date(dos_date,&t.tm_year,&t.tm_mon,
			   &t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
	t.tm_isdst = -1;
  
	ret = timegm(&t);

	ret += zone_offset;

	return ret;
}

/**
like make_unix_date() but the words are reversed
**/
_PUBLIC_ time_t pull_dos_date2(const uint8_t *date_ptr, int zone_offset)
{
	uint32_t x,x2;

	x = IVAL(date_ptr,0);
	x2 = ((x&0xFFFF)<<16) | ((x&0xFFFF0000)>>16);
	SIVAL(&x,0,x2);

	return pull_dos_date((void *)&x, zone_offset);
}

/**
  create a unix GMT date from a dos date in 32 bit "unix like" format
  these generally arrive as localtimes, with corresponding DST
**/
_PUBLIC_ time_t pull_dos_date3(const uint8_t *date_ptr, int zone_offset)
{
	time_t t = (time_t)IVAL(date_ptr,0);
	if (!null_time(t)) {
		t += zone_offset;
	}
	return t;
}


/**
return a HTTP/1.0 time string
**/
_PUBLIC_ char *http_timestring(TALLOC_CTX *mem_ctx, time_t t)
{
	char *buf;
	char tempTime[60];
	struct tm tm, *tm_p; 
	
	tm_p = localtime_r(&t, &tm);

	if (!tm_p) {
		return talloc_asprintf(mem_ctx,"%ld seconds since the Epoch",(long)t);
	}

#ifndef HAVE_STRFTIME
	char tbuf[26]; // buffer for asctime_r
	buf = talloc_strdup(mem_ctx, asctime_r(&tm, tbuf));
	if (buf[strlen(buf)-1] == '\n') {
		buf[strlen(buf)-1] = 0;
	}
#else
	strftime(tempTime, sizeof(tempTime)-1, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	buf = talloc_strdup(mem_ctx, tempTime);
#endif /* !HAVE_STRFTIME */

	return buf;
}

/**
 Return the date and time as a string
**/
_PUBLIC_ char *timestring(TALLOC_CTX *mem_ctx, time_t t)
{
	char *TimeBuf;
	char tempTime[80];
	struct tm tm, *tm_p;

	tm_p = localtime_r(&t, &tm);
	if (!tm_p) {
		return talloc_asprintf(mem_ctx,
				       "%ld seconds since the Epoch",
				       (long)t);
	}

#ifdef HAVE_STRFTIME
	/* some versions of gcc complain about using %c. This is a bug
	   in the gcc warning, not a bug in this code. See a recent
	   strftime() manual page for details.
	 */
	strftime(tempTime,sizeof(tempTime)-1,"%c %Z",&tm);
	TimeBuf = talloc_strdup(mem_ctx, tempTime);
#else
	char *buf[26]; // buffer for asctime_r
	TimeBuf = talloc_strdup(mem_ctx, asctime_r(&tm, buf));
#endif

	return TimeBuf;
}

/**
  return a talloced string representing a NTTIME for human consumption
*/
_PUBLIC_ const char *nt_time_string(TALLOC_CTX *mem_ctx, NTTIME nt)
{
	time_t t;
	if (nt == 0) {
		return "NTTIME(0)";
	}
	t = nt_time_to_unix(nt);
	return timestring(mem_ctx, t);
}


/**
  put a NTTIME into a packet
*/
_PUBLIC_ void push_nttime(uint8_t *base, uint16_t offset, NTTIME t)
{
	SBVAL(base, offset,   t);
}

/**
  pull a NTTIME from a packet
*/
_PUBLIC_ NTTIME pull_nttime(uint8_t *base, uint16_t offset)
{
	NTTIME ret = BVAL(base, offset);
	return ret;
}

/**
  parse a nttime as a large integer in a string and return a NTTIME
*/
_PUBLIC_ NTTIME nttime_from_string(const char *s)
{
	return strtoull(s, NULL, 0);
}

/**
  return (tv1 - tv2) in microseconds
*/
_PUBLIC_ int64_t usec_time_diff(struct timeval *tv1, struct timeval *tv2)
{
	int64_t sec_diff = tv1->tv_sec - tv2->tv_sec;
	return (sec_diff * 1000000) + (int64_t)(tv1->tv_usec - tv2->tv_usec);
}


/**
  return a zero timeval
*/
_PUBLIC_ struct timeval timeval_zero(void)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return tv;
}

/**
  return True if a timeval is zero
*/
_PUBLIC_ BOOL timeval_is_zero(const struct timeval *tv)
{
	return tv->tv_sec == 0 && tv->tv_usec == 0;
}

/**
  return a timeval for the current time
*/
_PUBLIC_ struct timeval timeval_current(void)
{
	struct timeval tv;
	GetTimeOfDay(&tv);
	return tv;
}

/**
  return a timeval struct with the given elements
*/
_PUBLIC_ struct timeval timeval_set(uint32_t secs, uint32_t usecs)
{
	struct timeval tv;
	tv.tv_sec = secs;
	tv.tv_usec = usecs;
	return tv;
}


/**
  return a timeval ofs microseconds after tv
*/
_PUBLIC_ struct timeval timeval_add(const struct timeval *tv,
			   uint32_t secs, uint32_t usecs)
{
	struct timeval tv2 = *tv;
	const unsigned int million = 1000000;
	tv2.tv_sec += secs;
	tv2.tv_usec += usecs;
	tv2.tv_sec += tv2.tv_usec / million;
	tv2.tv_usec = tv2.tv_usec % million;
	return tv2;
}

/**
  return the sum of two timeval structures
*/
struct timeval timeval_sum(const struct timeval *tv1,
			   const struct timeval *tv2)
{
	return timeval_add(tv1, tv2->tv_sec, tv2->tv_usec);
}

/**
  return a timeval secs/usecs into the future
*/
_PUBLIC_ struct timeval timeval_current_ofs(uint32_t secs, uint32_t usecs)
{
	struct timeval tv = timeval_current();
	return timeval_add(&tv, secs, usecs);
}

/**
  compare two timeval structures. 
  Return -1 if tv1 < tv2
  Return 0 if tv1 == tv2
  Return 1 if tv1 > tv2
*/
_PUBLIC_ int timeval_compare(const struct timeval *tv1, const struct timeval *tv2)
{
	if (tv1->tv_sec  > tv2->tv_sec)  return 1;
	if (tv1->tv_sec  < tv2->tv_sec)  return -1;
	if (tv1->tv_usec > tv2->tv_usec) return 1;
	if (tv1->tv_usec < tv2->tv_usec) return -1;
	return 0;
}

/**
  return True if a timer is in the past
*/
_PUBLIC_ BOOL timeval_expired(const struct timeval *tv)
{
	struct timeval tv2 = timeval_current();
	if (tv2.tv_sec > tv->tv_sec) return True;
	if (tv2.tv_sec < tv->tv_sec) return False;
	return (tv2.tv_usec >= tv->tv_usec);
}

/**
  return the number of seconds elapsed between two times
*/
_PUBLIC_ double timeval_elapsed2(const struct timeval *tv1, const struct timeval *tv2)
{
	return (tv2->tv_sec - tv1->tv_sec) + 
	       (tv2->tv_usec - tv1->tv_usec)*1.0e-6;
}

/**
  return the number of seconds elapsed since a given time
*/
_PUBLIC_ double timeval_elapsed(const struct timeval *tv)
{
	struct timeval tv2 = timeval_current();
	return timeval_elapsed2(tv, &tv2);
}

/**
  return the lesser of two timevals
*/
_PUBLIC_ struct timeval timeval_min(const struct timeval *tv1,
			   const struct timeval *tv2)
{
	if (tv1->tv_sec < tv2->tv_sec) return *tv1;
	if (tv1->tv_sec > tv2->tv_sec) return *tv2;
	if (tv1->tv_usec < tv2->tv_usec) return *tv1;
	return *tv2;
}

/**
  return the greater of two timevals
*/
_PUBLIC_ struct timeval timeval_max(const struct timeval *tv1,
			   const struct timeval *tv2)
{
	if (tv1->tv_sec > tv2->tv_sec) return *tv1;
	if (tv1->tv_sec < tv2->tv_sec) return *tv2;
	if (tv1->tv_usec > tv2->tv_usec) return *tv1;
	return *tv2;
}

/**
  return the difference between two timevals as a timeval
  if tv1 comes after tv2, then return a zero timeval
  (this is *tv2 - *tv1)
*/
_PUBLIC_ struct timeval timeval_until(const struct timeval *tv1,
			     const struct timeval *tv2)
{
	struct timeval t;
	if (timeval_compare(tv1, tv2) >= 0) {
		return timeval_zero();
	}
	t.tv_sec = tv2->tv_sec - tv1->tv_sec;
	if (tv1->tv_usec > tv2->tv_usec) {
		t.tv_sec--;
		t.tv_usec = 1000000 - (tv1->tv_usec - tv2->tv_usec);
	} else {
		t.tv_usec = tv2->tv_usec - tv1->tv_usec;
	}
	return t;
}


/**
  convert a timeval to a NTTIME
*/
_PUBLIC_ NTTIME timeval_to_nttime(const struct timeval *tv)
{
	return 10*(tv->tv_usec + 
		  ((TIME_FIXUP_CONSTANT + (uint64_t)tv->tv_sec) * 1000000));
}

/**
  convert a NTTIME to a timeval
*/
_PUBLIC_ void nttime_to_timeval(struct timeval *tv, NTTIME t)
{
	if (tv == NULL) return;

	t += 10/2;
	t /= 10;
	t -= TIME_FIXUP_CONSTANT*1000*1000;

	tv->tv_sec  = t / 1000000;

	if (TIME_T_MIN > tv->tv_sec || tv->tv_sec > TIME_T_MAX) {
		tv->tv_sec  = 0;
		tv->tv_usec = 0;
		return;
	}
	
	tv->tv_usec = t - tv->tv_sec*1000000;
}

/*******************************************************************
yield the difference between *A and *B, in seconds, ignoring leap seconds
********************************************************************/
static int tm_diff(struct tm *a, struct tm *b)
{
	int ay = a->tm_year + (1900 - 1);
	int by = b->tm_year + (1900 - 1);
	int intervening_leap_days =
		(ay/4 - by/4) - (ay/100 - by/100) + (ay/400 - by/400);
	int years = ay - by;
	int days = 365*years + intervening_leap_days + (a->tm_yday - b->tm_yday);
	int hours = 24*days + (a->tm_hour - b->tm_hour);
	int minutes = 60*hours + (a->tm_min - b->tm_min);
	int seconds = 60*minutes + (a->tm_sec - b->tm_sec);

	return seconds;
}

/**
  return the UTC offset in seconds west of UTC, or 0 if it cannot be determined
 */
_PUBLIC_ int get_time_zone(time_t t)
{
	struct tm tm, *tm_p;
	struct tm tm_utc;

	tm_p = gmtime_r(&t, &tm);
	if (!tm_p)
		return 0;
	tm_utc = tm;
	tm_p = localtime_r(&t, &tm);
	if (!tm_p)
		return 0;
	return tm_diff(&tm_utc, &tm);
}
