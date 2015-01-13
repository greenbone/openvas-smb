/*
   Copyright (C) Andrzej Hajda 2006
   Contact: andrzej.hajda@wp.pl
   License: GNU General Public License version 2
*/

/* ver = $(VERSION / 100) . $(VERSION % 100) */
#define VERSION 80

#define SERVICE_NAME "winexesvc"

#define PIPE_NAME "ahexec"
#define PIPE_NAME_IO "ahexec_stdio%08X"
#define PIPE_NAME_ERR "ahexec_stderr%08X"

#define CMD_STD_IO_ERR "std_io_err"
#define CMD_RETURN_CODE "return_code"
