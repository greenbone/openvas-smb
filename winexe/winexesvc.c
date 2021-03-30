/*
   Copyright (C) Andrzej Hajda 2006
   Contact: andrzej.hajda@wp.pl
   License: GNU General Public License version 2
*/

#include <windows.h>
#include <aclapi.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "shared.h"

#define BUFSIZE 256

#if 0
static void SvcDebugOut(const char *a, int b)
{
	FILE *f = fopen("C:\\" SERVICE_NAME ".log", "at");
	if (f) {
		fprintf(f, a, b);
		fclose(f);
	}
}
#else
#define SvcDebugOut(a,b) 0
#endif

SECURITY_ATTRIBUTES sa;

/* Creates SECURITY_ATTRIBUTES sa with full access for BUILTIN\Administrators */
int CreatePipesSA()
{
	DWORD dwRes;
	PSID pAdminSID = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

	/* Create a SID for the BUILTIN\Administrators group. */
	if (!AllocateAndInitializeSid(&SIDAuthNT, 2,
				      SECURITY_BUILTIN_DOMAIN_RID,
				      DOMAIN_ALIAS_RID_ADMINS,
				      0, 0, 0, 0, 0, 0, &pAdminSID)) {
		SvcDebugOut("AllocateAndInitializeSid Error %u\n",
			    GetLastError());
		return 0;
	}
	/* Initialize an EXPLICIT_ACCESS structure for an ACE.
	   The ACE will allow the Administrators group full access to the key.
	*/
	ea.grfAccessPermissions = FILE_ALL_ACCESS;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.ptstrName = (LPTSTR) pAdminSID;

	/* Create a new ACL that contains the new ACEs */
	dwRes = SetEntriesInAcl(1, &ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes) {
		SvcDebugOut("SetEntriesInAcl Error %u\n", GetLastError());
		return 0;
	}
	/* Initialize a security descriptor */
	pSD =
	    (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
					      SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (NULL == pSD) {
		SvcDebugOut("LocalAlloc Error %u\n", GetLastError());
		return 0;
	}

	if (!InitializeSecurityDescriptor
	    (pSD, SECURITY_DESCRIPTOR_REVISION)) {
		SvcDebugOut("InitializeSecurityDescriptor Error %u\n",
			    GetLastError());
		return 0;
	}
	/* Add the ACL to the security descriptor */
	if (!SetSecurityDescriptorDacl(pSD, TRUE,	/* bDaclPresent flag */
				       pACL, FALSE))	/* not a default DACL */
	{
		SvcDebugOut("SetSecurityDescriptorDacl Error %u\n",
			    GetLastError());
		return 0;
	}
	/* Initialize a security attributes structure */
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;
	return 1;
}

typedef struct {
	HANDLE h;
	OVERLAPPED o;
} OV_HANDLE;

int hgets(char *str, int n, OV_HANDLE *pipe)
{
	DWORD res;
	DWORD count = 0;
	--n;
	while (--n >= 0) {
		if (!ReadFile(pipe->h, str, 1, NULL, &pipe->o) && GetLastError() != ERROR_IO_PENDING)
			goto finish;
		if (!GetOverlappedResult(pipe->h, &pipe->o, &res, TRUE) || !res)
			goto finish;
		if (*str == '\n')
			goto finish;
		++count;
		++str;
	}
finish:
	*str = 0;
	return count;
}

int hprintf(OV_HANDLE *pipe, const char *fmt, ...)
{
	int res;
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (!WriteFile(pipe->h, buf, strlen(buf), NULL, &pipe->o) && GetLastError() == ERROR_IO_PENDING)
		GetOverlappedResult(pipe->h, &pipe->o, (LPDWORD)&res, TRUE);
	FlushFileBuffers(pipe->h);
	return res;
}

typedef struct {
	OV_HANDLE *pipe;
	const char *cmd;
	HANDLE pio;
	HANDLE perr;
	HANDLE token;
	int implevel;
	int system;
	char *runas;
	int conn_number;
} connection_context;

typedef int CMD_FUNC(connection_context *);

typedef struct {
	const char *name;
	CMD_FUNC *func;
} CMD_ITEM;

int cmd_set(connection_context *c)
{
	static const char* var_system = "system";
	static const char* var_implevel = "implevel";
	static const char* var_runas = "runas";
	char *cmdline;
	int res = 0;

	cmdline = strchr(c->cmd, ' ');
	if (!cmdline) {
		goto finish;
	}
	++cmdline;
	int l;
	if ((strstr(cmdline, var_system) == cmdline) &&
            (cmdline[l = strlen(var_system)] == ' ')) {
		c->system = atoi(cmdline + l + 1);
	} else if ((strstr(cmdline, var_implevel) == cmdline) &&
            (cmdline[l = strlen(var_implevel)] == ' ')) {
		c->implevel = atoi(cmdline + l + 1);
	} else if ((strstr(cmdline, var_runas) == cmdline) &&
            (cmdline[l = strlen(var_runas)] == ' ')) {
		c->runas = strdup(cmdline + l + 1);
	} else {
	    hprintf(c->pipe, "error Unknown command (%s)\n", c->cmd);
	    goto finish;
	}
	res = 1;
finish:
	return res;
}

int cmd_get(connection_context *c)
{
	static const char* var_version = "version";
	char *cmdline;
	int res = 0;

	cmdline = strchr(c->cmd, ' ');
	if (!cmdline) {
		goto finish;
	}
	++cmdline;
	int l;
	if ((strstr(cmdline, var_version) == cmdline) &&
            (cmdline[l = strlen(var_version)] == 0)) {
		hprintf(c->pipe, "version 0x%04X\n", VERSION);
	} else {
		hprintf(c->pipe, "error Unknown argument (%s)\n", c->cmd);
	        goto finish;
	}
	res = 1;
finish:
	return res;
}

typedef struct {
	char *user;
	char *domain;
	char *password;
} credentials;

int prepare_credentials(char *str, credentials *crd)
{
	char *p;
	p = strchr(str, '/');
	if (!p) p = strchr(str, '\\');
	if (p) {
		*p++ = 0;
		crd->domain = str;
	} else {
	        p = str;
		crd->domain = ".";
	}
	crd->user = p;
	p = strchr(p, '%');
	if (p)
	    *p++ = 0;
	crd->password = p;
	return 1;
}

int get_token(connection_context *c)
{
	int res = 0;
	int wres;
	HANDLE token;

	if (c->runas) {
		credentials crd;
		if (!prepare_credentials(c->runas, &crd)) {
			hprintf(c->pipe, "error Incorrect runas credentials\n");
			goto finish;
		}
		wres = LogonUser(crd.user, crd.domain, crd.password, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &c->token);
		if (!wres) {
			hprintf(c->pipe, "error Cannot LogonUser(%s,%s,%s) %d\n", crd.user, crd.domain, crd.password, GetLastError());
			goto finish;
		}
		res = 1;
		goto finish;
	} else if (c->system) {
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token)) {
			hprintf(c->pipe, "error Cannot OpenProcessToken %d\n", GetLastError());
			goto finish;
		}
	} else {
		if (!ImpersonateNamedPipeClient(c->pipe->h)) {
			hprintf(c->pipe, "error Cannot ImpersonateNamedPipeClient %d\n", GetLastError());
			goto finish;
		}
		if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE, &token)) {
			hprintf(c->pipe, "error Cannot OpenThreadToken %d\n", GetLastError());
			goto finishRevertToSelf;
		}
	}
	if (!DuplicateTokenEx(token, MAXIMUM_ALLOWED, 0, c->implevel, TokenPrimary, &c->token)) {
		hprintf(c->pipe, "error Cannot Duplicate Token %d\n", GetLastError());
		goto finishCloseToken;
	}
	res = 1;
finishCloseToken:
	CloseHandle(token);
finishRevertToSelf:
	if (!c->system) {
		if (!RevertToSelf()) {
			hprintf(c->pipe, "error Cannot RevertToSelf %d\n", GetLastError());
			res = 0;
		}
	}
finish:
	return res;
}

int cmd_run(connection_context *c)
{
	char buf[256];
	int res = 0;
	int wres;
	char *cmdline;
	DWORD pipe_nr;

	cmdline = strchr(c->cmd, ' ');
	if (!cmdline) {
		goto finish;
	}
	++cmdline;

	if (!get_token(c))
		return 0;

	pipe_nr = (GetCurrentProcessId() << 16) + (DWORD) c->conn_number;
	sprintf(buf, "\\\\.\\pipe\\" PIPE_NAME_IO, pipe_nr);
	c->pio = CreateNamedPipe(buf,
			      PIPE_ACCESS_DUPLEX,
			      PIPE_WAIT,
			      1,
			      BUFSIZE,
			      BUFSIZE,
			      NMPWAIT_USE_DEFAULT_WAIT,
			      &sa);
	if (c->pio == INVALID_HANDLE_VALUE) {
		hprintf(c->pipe, "error Cannot create io pipe(%s), error 0x%08X\n", buf, GetLastError());
		goto finishCloseToken;
	}

	sprintf(buf, "\\\\.\\pipe\\" PIPE_NAME_ERR, pipe_nr);
	c->perr = CreateNamedPipe(buf,
			       PIPE_ACCESS_DUPLEX,
			       PIPE_WAIT,
			       1,
			       BUFSIZE,
			       BUFSIZE,
			       NMPWAIT_USE_DEFAULT_WAIT,
			       &sa);
	if (c->perr == INVALID_HANDLE_VALUE) {
		hprintf(c->pipe, "error Cannot create err pipe\n");
		goto finishClosePio;
	}
	/* Send handle to client (it will use it to connect pipes) */
	hprintf(c->pipe, CMD_STD_IO_ERR " %08X\n", pipe_nr);

	wres = ConnectNamedPipe(c->pio, NULL);
	if (!wres)
		wres = (GetLastError() == ERROR_PIPE_CONNECTED);

	if (!wres) {
		hprintf(c->pipe, "error ConnectNamedPipe(pio)\n");
		goto finishClosePerr;
	}

	wres = ConnectNamedPipe(c->perr, NULL);
	if (!wres)
		wres = (GetLastError() == ERROR_PIPE_CONNECTED);

	if (!wres) {
		hprintf(c->pipe, "error ConnectNamedPipe(perr)\n");
		goto finishDisconnectPio;
	}

	SetHandleInformation(c->pio, HANDLE_FLAG_INHERIT, 1);
	SetHandleInformation(c->perr, HANDLE_FLAG_INHERIT, 1);


	SECURITY_ATTRIBUTES sattr;
	sattr.nLength = sizeof(SECURITY_ATTRIBUTES);
	sattr.bInheritHandle = TRUE;
	sattr.lpSecurityDescriptor = NULL;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = c->pio;
	si.hStdOutput = c->pio;
	si.hStdError = c->perr;
	si.dwFlags |= STARTF_USESTDHANDLES;

	if (CreateProcessAsUser(
			  c->token,
			  NULL,
			  cmdline,	/* command line */
			  NULL,	/* process security attributes */
			  NULL,	/* primary thread security attributes */
			  TRUE,	/* handles are inherited */
			  0,	/* creation flags */
			  NULL,	/* use parent's environment */
			  NULL,	/* use parent's current directory */
			  &si,	/* STARTUPINFO pointer */
			  &pi))	/* receives PROCESS_INFORMATION */
	{
		HANDLE hlist[2] = {c->pipe->o.hEvent, pi.hProcess};
		DWORD ec;
		char str[1];

		if (!ResetEvent(c->pipe->o.hEvent))
			SvcDebugOut("ResetEvent error - %d\n", GetLastError());
                if (!ReadFile(c->pipe->h, str, 1, NULL, &c->pipe->o) && GetLastError() != ERROR_IO_PENDING)
			SvcDebugOut("ReadFile(control_pipe) error - %d\n", GetLastError());
		ec = WaitForMultipleObjects(2, hlist, FALSE, INFINITE);
		SvcDebugOut("WaitForMultipleObjects=%d\n", ec - WAIT_OBJECT_0);
		if (ec != WAIT_OBJECT_0)
			GetExitCodeProcess(pi.hProcess, &ec);
		else
			TerminateProcess(pi.hProcess, ec = 0x1234);
		FlushFileBuffers(c->pio);
		FlushFileBuffers(c->perr);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		hprintf(c->pipe, CMD_RETURN_CODE " %08X\n", ec);
	} else {
		hprintf(c->pipe, "error Creating process(%s) %d\n", cmdline, GetLastError());
	}

	DisconnectNamedPipe(c->perr);
finishDisconnectPio:
	DisconnectNamedPipe(c->pio);
finishClosePerr:
	CloseHandle(c->perr);
finishClosePio:
	CloseHandle(c->pio);
finishCloseToken:
	CloseHandle(c->token);
finish:
	return res;
}

CMD_ITEM cmd_table[] = {
	{"run", cmd_run},
	{"set", cmd_set},
	{"get", cmd_get},
	{NULL, NULL}
};

typedef struct {
	OV_HANDLE *pipe;
	int conn_number;
} connection_data;

#define MAX_COMMAND_LENGTH (32768)

VOID handle_connection(connection_data *data)
{
        char *cmd = 0;
	int res;
	connection_context _c, *c = &_c;
        cmd = malloc(MAX_COMMAND_LENGTH);
        if (!cmd) {
	    hprintf(data->pipe,
		    "error: unable to allocate buffer for command\n");
	    return;
	}
        ZeroMemory(cmd, MAX_COMMAND_LENGTH);
	ZeroMemory(c, sizeof(connection_context));
	c->pipe = data->pipe;
	c->cmd = cmd;
	c->conn_number = data->conn_number;
	free(data);
	/* FIXME make wait for end of process or ctrl_pipe input */
	while (1) {
		res = hgets(cmd, MAX_COMMAND_LENGTH, c->pipe);
		if (res <= 0) {
			SvcDebugOut("Error reading from pipe(%08X)\n", (int) c->pipe->h);
			goto finish;
		}
		SvcDebugOut("Retrieved line: \"%s\"\n", (int)cmd);
		CMD_ITEM *ci;
		for (ci = cmd_table; ci->name; ++ci) {
			if (strstr(cmd, ci->name) != cmd) continue;
			char c = cmd[strlen(ci->name)];
			if (!c || (c == ' '))
				break;
		}
		if (ci->name) {
			if (!ci->func(c))
				goto finish;
		} else
			hprintf(c->pipe, "error Ignoring unknown command (%s)\n", cmd);
	}
finish:
	FlushFileBuffers(c->pipe->h);
	DisconnectNamedPipe(c->pipe->h);
	CloseHandle(c->pipe->h);
	CloseHandle(c->pipe->o.hEvent);
	free(c->pipe);
        free(cmd);
}

static int conn_number = 0;

DWORD WINAPI server_loop(LPVOID lpParameter)
{
	BOOL res;

	SvcDebugOut("server_loop: alive\n", 0);
	if (!CreatePipesSA()) {
		SvcDebugOut("CreatePipesSA failed (%08X)\n",
			    GetLastError());
		return -1;
	}
	SvcDebugOut("server_loop: CreatePipesSA done\n", 0);
	for (;;) {
		SvcDebugOut("server_loop: Create Pipe\n", 0);
		OV_HANDLE *pipe;
		pipe = (OV_HANDLE *)malloc(sizeof(OV_HANDLE));
		ZeroMemory(&pipe->o, sizeof(OVERLAPPED));
		pipe->o.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		pipe->h = CreateNamedPipe("\\\\.\\pipe\\" PIPE_NAME,
				       PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
				       PIPE_WAIT,
				       PIPE_UNLIMITED_INSTANCES,
				       BUFSIZE,
				       BUFSIZE,
				       NMPWAIT_USE_DEFAULT_WAIT,
				       &sa);

		if (pipe->h == INVALID_HANDLE_VALUE) {
			SvcDebugOut("CreatePipe failed(%08X)\n",
				    GetLastError());
			CloseHandle(pipe->o.hEvent);
			free(pipe);
			return 0;
		}

		SvcDebugOut("server_loop: Connect Pipe\n", 0);
		if (ConnectNamedPipe(pipe->h, &pipe->o)) {
			SvcDebugOut("server_loop: Connect Pipe err %08X\n", GetLastError());
			res = FALSE;
		} else {
			switch (GetLastError()) {
			case ERROR_IO_PENDING:
				SvcDebugOut("server_loop: Connect Pipe(0) pending\n", 0);
				DWORD t;
				res = GetOverlappedResult(pipe->h, &pipe->o, &t, TRUE);
				break;
			case ERROR_PIPE_CONNECTED:
				SvcDebugOut("server_loop: Connect Pipe(0) connected\n", 0);
				res = TRUE;
				break;
			default:
				SvcDebugOut("server_loop: Connect Pipe(0) err %08X\n", GetLastError());
				res = FALSE;
			}
		}

		if (res) {
			connection_data *cd = malloc(sizeof(connection_data));
			cd->pipe = pipe;
			cd->conn_number = ++conn_number;
			SvcDebugOut("server_loop: CreateThread\n", 0);
			HANDLE th = CreateThread(NULL,	/* no security attribute */
						 0,	/* default stack size */
						 (LPTHREAD_START_ROUTINE)
						 handle_connection,
						 (LPVOID) cd,	/* thread parameter */
						 0,	/* not suspended */
						 NULL);	/* returns thread ID */
			if (!th) {
				SvcDebugOut("Cannot create thread\n", 0);
				CloseHandle(pipe->h);
				CloseHandle(pipe->o.hEvent);
				free(pipe);
			} else {
				CloseHandle(th);
				SvcDebugOut("server_loop: Thread created\n", 0);
			}
		} else {
			SvcDebugOut("server_loop: Pipe not connected\n", 0);
			CloseHandle(pipe->h);
			CloseHandle(pipe->o.hEvent);
			free(pipe);
		}
	}
	SvcDebugOut("server_loop: STH wrong\n", 0);
}
