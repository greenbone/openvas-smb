/*
###############################################################################
# WinCMD lib
#
# Authors:
# Thanga Prakash S <tprakash@secpod.com>
#
# Copyright:
# Copyright (c) 2013 Intevation GmbH, http://www.intevation.net
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2
# (or any later version), as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
###############################################################################

*/

/**
 * @file wincmd.c
 *
 * @brief WinCMD function
 *
 * Provides Windows Command Execution functionalities.
 */

#include "includes.h"
#include "lib/cmdline/popt_common.h"
#include "librpc/rpc/dcerpc.h"
#include "librpc/gen_ndr/ndr_svcctl_c.h"
#include "librpc/gen_ndr/ndr_security.h"
#include "libcli/libcli.h"
#include "lib/events/events.h"

#include "winexe.h"
#include "winexesvc/shared.h"

#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/termios.h>
#include <signal.h>

struct program_args {
  char *hostname;
  char *cmd;
  struct cli_credentials *credentials;
  int reinstall;
  int uninstall;
  int system;
  char *runas;
  int interactive;
};

/* We could dynamically allocate the result but to have a limit
   prevents hangs if we are fed with too much data. */
#define MAX_RESULTS 512

char *result[MAX_RESULTS];
int result_inc = 0;
int result_len = 0;
int exit_flag = 0;
int abort_requested = 0;

int parse_args(int argc, char *argv[], struct program_args *pmyargs)
{
  poptContext pc;
  int opt, i;

  int argc_new;
  char **argv_new;

  struct poptOption long_options[] = {
    POPT_AUTOHELP
    POPT_COMMON_SAMBA
    POPT_COMMON_CONNECTION
    POPT_COMMON_CREDENTIALS
    POPT_COMMON_VERSION
    {"uninstall", 0, POPT_ARG_NONE, &pmyargs->uninstall, 0,
     "Uninstall winexe service after remote execution", NULL},
    {"reinstall", 0, POPT_ARG_NONE, &pmyargs->reinstall, 0,
     "Reinstall winexe service before remote execution", NULL},
    {"system", 0, POPT_ARG_NONE, &pmyargs->system, 0,
     "Use SYSTEM account" , NULL},
    {"runas", 0, POPT_ARG_STRING, &pmyargs->runas, 0,
     "Run as user (BEWARE: password is sent in cleartext over net)" , "[DOMAIN\\]USERNAME%PASSWORD"},
    {"interactive", 0, POPT_ARG_INT, &pmyargs->interactive, 0,
     "Desktop interaction: 0 - disallow, 1 - allow. If you allow use also --system switch (Win requirement). Vista do not support this option.", NULL},
    POPT_TABLEEND
  };

  pc = poptGetContext(argv[0], argc, (const char **) argv,
          long_options, 0);

  poptSetOtherOptionHelp(pc, "//host command");

  while ((opt = poptGetNextOpt(pc)) != -1) {
    poptFreeContext(pc);
    return 1;
  }

  argv_new = discard_const_p(char *, poptGetArgs(pc));

  argc_new = argc;
  for (i = 0; i < argc - 1; i++) {
    if (argv_new[i] == NULL) {
      argc_new = i;
      break;
    }
  }

  if (argc_new != 2 || argv_new[0][0] != '/'
      || argv_new[0][1] != '/') {
    poptFreeContext(pc);
    return 1;
  }

  pmyargs->hostname = argv_new[0] + 2;
  pmyargs->cmd = argv_new[1];
  poptFreeContext(pc);
  return 0;
}

enum {STATE_OPENING, STATE_GETTING_VERSION, STATE_RUNNING, STATE_CLOSING, STATE_CLOSING_FOR_REINSTALL };

struct winexe_context {
  int state;
  struct program_args *args;
  struct smbcli_tree *tree;
  struct async_context *ac_ctrl;
  struct async_context *ac_io;
  struct async_context *ac_err;
  int return_code;
};

void exit_program(struct winexe_context *c);

void on_ctrl_pipe_error(struct winexe_context *c, int func, NTSTATUS status)
{
  DEBUG(1, ("ERROR: on_ctrl_pipe_error - %s\n", nt_errstr(status)));
  static int activated = 0;
  if (!activated
      && NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
    status =
        svc_install(c->args->hostname, c->args->credentials, c->args->interactive);
    if (!NT_STATUS_IS_OK(status)) {
      DEBUG(0,
            ("ERROR: Failed to install service winexesvc - %s\n",
             nt_errstr(status)));
      c->return_code = -1;
      exit_program(c);
    }
    activated = 1;
    async_open(c->ac_ctrl, "\\" PIPE_NAME, OPENX_MODE_ACCESS_RDWR);
  } else if (func == ASYNC_OPEN_RECV) {
    DEBUG(0,
          ("ERROR: Cannot open control pipe - %s\n",
           nt_errstr(status)));
    c->return_code = -1;
    exit_program(c);
  } else if (func == ASYNC_READ_RECV && c->state == STATE_OPENING) {
    ;
  } else
    exit_program(c);
}

void on_io_pipe_open(struct winexe_context *c);
void on_io_pipe_read(struct winexe_context *c, const char *data, int len);
void on_io_pipe_error(struct winexe_context *c, int func, NTSTATUS status);
void on_err_pipe_read(struct winexe_context *c, const char *data, int len);
void on_err_pipe_error(struct winexe_context *c, int func, NTSTATUS status);

const char *cmd_check(const char *data, const char *cmd, int len)
{
  int lcmd = strlen(cmd);
  if (lcmd >= len)
    return 0;
  if (!strncmp(data, cmd, lcmd)
      && (data[lcmd] == ' ' || data[lcmd] == '\n')) {
    return data + lcmd + 1;
  }
  return 0;
}

void catch_alarm(int sig)
{
  abort_requested = 1;
  signal(sig, SIG_DFL);
}

static void timer(struct event_context *ev, struct timed_event *te, struct timeval t, void *private)
{
  struct winexe_context *c = talloc_get_type(private, struct winexe_context);
  if (abort_requested) {
    async_write(c->ac_ctrl, "abort\n", 6);
  } else
    event_add_timed(c->tree->session->transport->socket->event.ctx, c, timeval_current_ofs(0, 10000), timer, c);
}

void on_ctrl_pipe_open(struct winexe_context *c)
{
  char *str = "get version\n";

  DEBUG(1, ("CTRL: Sending command: %s", str));
  c->state = STATE_GETTING_VERSION;
  async_write(c->ac_ctrl, str, strlen(str));
  signal(SIGINT, catch_alarm);
  signal(SIGTERM, catch_alarm);
  event_add_timed(c->tree->session->transport->socket->event.ctx, c, timeval_current_ofs(0, 10000), timer, c);
}

void on_ctrl_pipe_read(struct winexe_context *c, const char *data, int len)
{
  const char *p;
  if ((p = cmd_check(data, CMD_STD_IO_ERR, len))) {
    DEBUG(1, ("CTRL: Recieved command: %.*s", len, data));
    unsigned int npipe = strtoul(p, 0, 16);
    c->ac_io = talloc_zero(c, struct async_context);
    c->ac_io->tree = c->tree;
    c->ac_io->cb_ctx = c;
    c->ac_io->cb_open = (async_cb_open) on_io_pipe_open;
    c->ac_io->cb_read = (async_cb_read) on_io_pipe_read;
    c->ac_io->cb_error = (async_cb_error) on_io_pipe_error;
    char *fn = talloc_asprintf(c->ac_io, "\\" PIPE_NAME_IO, npipe);
    async_open(c->ac_io, fn, OPENX_MODE_ACCESS_RDWR);
    c->ac_err = talloc_zero(c, struct async_context);
    c->ac_err->tree = c->tree;
    c->ac_err->cb_ctx = c;
    c->ac_err->cb_read = (async_cb_read) on_err_pipe_read;
    c->ac_err->cb_error = (async_cb_error) on_err_pipe_error;
    fn = talloc_asprintf(c->ac_err, "\\" PIPE_NAME_ERR, npipe);
    async_open(c->ac_err, fn, OPENX_MODE_ACCESS_RDWR);
  } else if ((p = cmd_check(data, CMD_RETURN_CODE, len))) {
      c->return_code = 0;
  } else if ((p = cmd_check(data, "version", len))) {
    int ver = strtoul(p, 0, 0);
    if (ver/10 != VERSION/10) {
      DEBUG(1, ("CTRL: Bad version of service (is %d.%02d, expected %d.%02d), reinstalling.\n", ver/100, ver%100, VERSION/100, VERSION%100));
      async_close(c->ac_ctrl);
      c->state = STATE_CLOSING_FOR_REINSTALL;
    } else {
      char *str;
      if (c->args->runas)
        str = talloc_asprintf(c, "set runas %s\nrun %s\n", c->args->runas, c->args->cmd);
      else
        str = talloc_asprintf(c, "%srun %s\n", c->args->system ? "set system 1\n" : "" , c->args->cmd);
      DEBUG(1, ("CTRL: Sending command: %s", str));
      async_write(c->ac_ctrl, str, strlen(str));
      talloc_free(str);
      c->state = STATE_RUNNING;
    }
  } else if ((p = cmd_check(data, "error", len))) {
    DEBUG(0, ("Error: %.*s", len, data));
    if (c->state == STATE_GETTING_VERSION) {
      DEBUG(0, ("CTRL: Probably old version of service, reinstalling.\n"));
      async_close(c->ac_ctrl);
      c->state = STATE_CLOSING_FOR_REINSTALL;
    }
  } else {
    DEBUG(0, ("CTRL: Unknown command: %.*s", len, data));
  }
}

void on_ctrl_pipe_close(struct winexe_context *c)
{
  if (c->state == STATE_CLOSING_FOR_REINSTALL) {
    DEBUG(1,("Reinstalling service\n"));
    svc_uninstall(c->args->hostname, c->args->credentials);
    svc_install(c->args->hostname, c->args->credentials, c->args->interactive | SVC_FORCE_UPLOAD);
    c->state = STATE_OPENING;
    async_open(c->ac_ctrl, "\\" PIPE_NAME, OPENX_MODE_ACCESS_RDWR);
  }
}

void on_io_pipe_open(struct winexe_context *c)
{
  struct termios term;
  tcgetattr(0, &term);
  term.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term);
  setbuf(stdin, NULL);
}

void on_io_pipe_read(struct winexe_context *c, const char *data, int len)
{
  if (result_inc < MAX_RESULTS - 2)
    {
      result[result_inc] = (char *) g_malloc0 (len + 1);
      strncat(result[result_inc], data, len);
      result_len = result_len + len;
      result_inc++;
    }
  else
    {
      DEBUG(1, ("ERROR: Too much output from command."));
      c->return_code = -1;
      exit_program(c);
    }
}

void on_io_pipe_error(struct winexe_context *c, int func, NTSTATUS status)
{
  async_close(c->ac_io);
}

void on_err_pipe_read(struct winexe_context *c, const char *data, int len)
{
  if (result_inc < MAX_RESULTS - 2)
    {
      result[result_inc] = (char *) g_malloc0 (len + 1);
      strncat(result[result_inc], data, len);
      result_len = result_len + len;
      result_inc++;
    }
  else
    {
      DEBUG(1, ("ERROR: Too much output from command."));
      c->return_code = -1;
      exit_program(c);
    }
}

void on_err_pipe_error(struct winexe_context *c, int func, NTSTATUS status)
{
  async_close(c->ac_err);
}

void exit_program(struct winexe_context *c)
{
  if (c->args->uninstall)
    svc_uninstall(c->args->hostname, c->args->credentials);
  exit_flag = 1;
}

/**
 * @brief Execute Windows Command.
 *
 * @param[in] argc - Number of arguments passed
 *
 * @param[in] argv - Host, Credentials, and Command to execute
 *
 * @param[out] res - Result string
 *
 * @return, 0 on success, -1 on failure
 */
int wincmd(int argc, char *argv[], char **res)
{
  NTSTATUS status;
  struct smbcli_state *cli;
  struct program_args myargs = {.reinstall = 0,.uninstall = 0, .system = 0, .interactive = SVC_IGNORE_INTERACTIVE };
  int ret, i = 0, final_result = 0;

  ret = parse_args(argc, argv, &myargs);

  if(ret == 1)
  {
    DEBUG(1, ("ERROR: %s\n", "Invalid input arguments"));
    return -1;
  }

  myargs.interactive &= SVC_INTERACTIVE_MASK;

  dcerpc_init();

  if (myargs.reinstall)
    svc_uninstall(myargs.hostname, cmdline_credentials);

  if (!(myargs.interactive & SVC_IGNORE_INTERACTIVE)) {
    svc_install(myargs.hostname, cmdline_credentials, myargs.interactive | (myargs.reinstall ? SVC_FORCE_UPLOAD : 0));
  }

  status =
      smbcli_full_connection(NULL, &cli, myargs.hostname, "IPC$",
             NULL, cmdline_credentials, NULL);
  if (!NT_STATUS_IS_OK(status)) {
    DEBUG(0,
            ("ERROR: Failed to open connection - %s\n",
             nt_errstr(status)));
      return -1;
  }

  struct winexe_context *c =
        talloc_zero(cli->tree, struct winexe_context);
  if (c == NULL) {
    DEBUG(0,
            ("ERROR: Failed to allocate struct winexe_context\n"));
      return -1;
  }

  c->tree = cli->tree;
  c->ac_ctrl = talloc_zero(cli->tree, struct async_context);
  c->ac_ctrl->tree = cli->tree;
  c->ac_ctrl->cb_ctx = c;
  c->ac_ctrl->cb_open = (async_cb_open) on_ctrl_pipe_open;
  c->ac_ctrl->cb_read = (async_cb_read) on_ctrl_pipe_read;
  c->ac_ctrl->cb_error = (async_cb_error) on_ctrl_pipe_error;
  c->ac_ctrl->cb_close = (async_cb_close) on_ctrl_pipe_close;
  c->args = &myargs;
  c->args->credentials = cmdline_credentials;
  c->return_code = -1;
  c->state = STATE_OPENING;
  async_open(c->ac_ctrl, "\\" PIPE_NAME, OPENX_MODE_ACCESS_RDWR);

  while(exit_flag != 1)
    event_loop_once(cli->tree->session->transport->socket->event.ctx);

  final_result = result_inc + 1;
  result[final_result] = (char *) g_malloc0 (result_len + 1);

  while ( i <  result_inc){
    strncat(result[final_result], result[i], strlen(result[i]));
    i++;
  }

 *res = result[final_result];

  talloc_free(cli);

  exit_flag = 0;
  result_inc = 0;
  result_len = 0;
  return c->return_code;
}
