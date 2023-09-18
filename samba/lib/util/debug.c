/*
 * SPDX-FileCopyrightText: James J Myers	 2003
 * SPDX-FileCopyrightText: Andrew Tridgell 2003
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
   Unix SMB/CIFS implementation.
   Samba debug functions
   Copyright (C) Andrew Tridgell 2003
   Copyright (C) James J Myers	 2003

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
#include "system/time.h"
#include "dynconfig.h"

/**
 * @file
 * @brief Debug logging
 **/

/** 
 * this global variable determines what messages are printed 
 */
_PUBLIC_ int DEBUGLEVEL;

/* the registered mutex handlers */
static struct {
	const char *name;
	struct debug_ops ops;
} debug_handlers;

/* state variables for the debug system */
static struct {
	int fd;
	enum debug_logtype logtype;
	const char *prog_name;
} state;

static BOOL reopen_logs_scheduled;
static BOOL check_reopen_logs(void)
{
	if (state.fd == 0 || reopen_logs_scheduled) {
		reopen_logs_scheduled = False;
		reopen_logs();
	}

	if (state.fd <= 0) return False;

	return True;
}

_PUBLIC_ void debug_schedule_reopen_logs(void)
{
	reopen_logs_scheduled = True;
}

static void log_timestring(int level, const char *location, const char *func)
{
	char *t = NULL;
	char *s = NULL;

	if (!check_reopen_logs()) return;

	if (state.logtype != DEBUG_FILE) return;

	t = timestring(NULL, time(NULL));
	if (!t) return;

	asprintf(&s, "[%s, %d %s:%s()]\n", t, level, location, func);
	talloc_free(t);
	if (!s) return;

	write(state.fd, s, strlen(s));
	free(s);
}

static void log_location(const char* location, const char* func)
{
	char *s = NULL;

	if (!check_reopen_logs()) return;

	if (strncmp(location, "lib/talloc/talloc.c", strlen("lib/talloc/talloc.c")) == 0) return;

	asprintf(&s, "[%s:%s()] ", location, func);
	if (!s) return;

	write(state.fd, s, strlen(s));
	free(s);
}

/**
  the backend for debug messages. Note that the DEBUG() macro has already
  ensured that the log level has been met before this is called
*/
_PUBLIC_ void do_debug_header(int level, const char *location, const char *func)
{
	// log_timestring(level, location, func);
	log_location(location, func);
	log_task_id();
}

/**
  the backend for debug messages. Note that the DEBUG() macro has already
  ensured that the log level has been met before this is called

  @note You should never have to call this function directly. Call the DEBUG()
  macro instead.
*/
_PUBLIC_ void do_debug(const char *format, ...) _PRINTF_ATTRIBUTE(1,2)
{
	va_list ap;
	char *s = NULL;

	if (!check_reopen_logs()) return;

	va_start(ap, format);
	vasprintf(&s, format, ap);
	va_end(ap);

	write(state.fd, s, strlen(s));
	free(s);
}

_PUBLIC_ const char *logfile = NULL;

/**
  reopen the log file (usually called because the log file name might have changed)
*/
_PUBLIC_ void reopen_logs(void)
{
	char *fname = NULL;
	int old_fd = state.fd;

	switch (state.logtype) {
	case DEBUG_STDOUT:
		state.fd = 1;
		break;

	case DEBUG_STDERR:
		state.fd = 2;
		break;

	case DEBUG_FILE:
		if ((*logfile) == '/') {
			fname = strdup(logfile);
		} else {
			asprintf(&fname, "%s/%s.log", dyn_LOGFILEBASE, state.prog_name);
		}
		if (fname) {
			int newfd = open(fname, O_CREAT|O_APPEND|O_WRONLY, 0600);
			if (newfd == -1) {
				DEBUG(1, ("Failed to open new logfile: %s\n", fname));
				old_fd = -1;
			} else {
				state.fd = newfd;
			}
			free(fname);
		} else {
			DEBUG(1, ("Failed to find name for file-based logfile!\n"));
		}

		break;
	}

	if (old_fd > 2) {
		close(old_fd);
	}
}

/**
  control the name of the logfile and whether logging will be to stdout, stderr
  or a file
*/
_PUBLIC_ void setup_logging(const char *prog_name, enum debug_logtype new_logtype)
{
	if (state.logtype < new_logtype) {
		state.logtype = new_logtype;
	}
	if (prog_name) {
		state.prog_name = prog_name;
	}
	reopen_logs();
}

/**
  return a string constant containing n tabs
  no more than 10 tabs are returned
*/
_PUBLIC_ const char *do_debug_tab(int n)
{
	const char *tabs[] = {"", "\t", "\t\t", "\t\t\t", "\t\t\t\t", "\t\t\t\t\t", 
			      "\t\t\t\t\t\t", "\t\t\t\t\t\t\t", "\t\t\t\t\t\t\t\t", 
			      "\t\t\t\t\t\t\t\t\t", "\t\t\t\t\t\t\t\t\t\t"};
	return tabs[MIN(n, 10)];
}


/**
  log suspicious usage - print comments and backtrace
*/	
_PUBLIC_ void log_suspicious_usage(const char *from, const char *info)
{
	if (!debug_handlers.ops.log_suspicious_usage) return;

	debug_handlers.ops.log_suspicious_usage(from, info);
}


/**
  print suspicious usage - print comments and backtrace
*/	
_PUBLIC_ void print_suspicious_usage(const char* from, const char* info)
{
	if (!debug_handlers.ops.print_suspicious_usage) return;

	debug_handlers.ops.print_suspicious_usage(from, info);
}

_PUBLIC_ uint32_t get_task_id(void)
{
	if (debug_handlers.ops.get_task_id) {
		return debug_handlers.ops.get_task_id();
	}
	return getpid();
}

_PUBLIC_ void log_task_id(void)
{
	if (!debug_handlers.ops.log_task_id) return;

	if (!check_reopen_logs()) return;

	debug_handlers.ops.log_task_id(state.fd);
}

/**
  register a set of debug handlers. 
*/
_PUBLIC_ void register_debug_handlers(const char *name, struct debug_ops *ops)
{
	debug_handlers.name = name;
	debug_handlers.ops = *ops;
}
