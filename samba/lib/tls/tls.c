/*
 * SPDX-FileCopyrightText: Andrew Bartlett 2006
 * SPDX-FileCopyrightText: Stefan Metzmacher 2004
 * SPDX-FileCopyrightText: Andrew Tridgell 2004-2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   transport layer security handling code

   Copyright (C) Andrew Tridgell 2004-2005
   Copyright (C) Stefan Metzmacher 2004
   Copyright (C) Andrew Bartlett 2006
 
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
#include "lib/events/events.h"
#include "lib/socket/socket.h"

#if ENABLE_GNUTLS
#include "gnutls/gnutls.h"

#define DH_BITS 1024

#if defined(HAVE_GNUTLS_DATUM) && !defined(HAVE_GNUTLS_DATUM_T)
typedef gnutls_datum gnutls_datum_t;
#endif

/* hold persistent tls data */
struct tls_params {
	gnutls_certificate_credentials_t x509_cred;
	gnutls_dh_params_t dh_params;
	BOOL tls_enabled;
};
#endif

/* hold per connection tls data */
struct tls_context {
	struct socket_context *socket;
	struct fd_event *fde;
	BOOL tls_enabled;
#if ENABLE_GNUTLS
	gnutls_session_t session;
	BOOL done_handshake;
	BOOL have_first_byte;
	uint8_t first_byte;
	BOOL tls_detect;
	const char *plain_chars;
	BOOL output_pending;
	gnutls_certificate_credentials_t xcred;
	BOOL interrupted;
#endif
};

BOOL tls_enabled(struct socket_context *sock)
{
	struct tls_context *tls;
	if (!sock) {
		return False;
	}
	if (strcmp(sock->backend_name, "tls") != 0) {
		return False;
	}
	tls = talloc_get_type(sock->private_data, struct tls_context);
	if (!tls) {
		return False;
	}
	return tls->tls_enabled;
}


#if ENABLE_GNUTLS

static const struct socket_ops tls_socket_ops;

static NTSTATUS tls_socket_init(struct socket_context *sock)
{
	switch (sock->type) {
	case SOCKET_TYPE_STREAM:
		break;
	default:
		return NT_STATUS_INVALID_PARAMETER;
	}

	sock->backend_name = "tls";

	return NT_STATUS_OK;
}

#define TLSCHECK(call) do { \
	ret = call; \
	if (ret < 0) { \
		DEBUG(0,("TLS %s - %s\n", #call, gnutls_strerror(ret))); \
		goto failed; \
	} \
} while (0)


/*
  callback for reading from a socket
*/
static ssize_t tls_pull(gnutls_transport_ptr_t ptr, void *buf, size_t size)
{
	struct tls_context *tls = talloc_get_type(ptr, struct tls_context);
	NTSTATUS status;
	size_t nread;
	
	if (tls->have_first_byte) {
		*(uint8_t *)buf = tls->first_byte;
		tls->have_first_byte = False;
		return 1;
	}

	status = socket_recv(tls->socket, buf, size, &nread);
	if (NT_STATUS_EQUAL(status, NT_STATUS_END_OF_FILE)) {
		return 0;
	}
	if (NT_STATUS_IS_ERR(status)) {
		EVENT_FD_NOT_READABLE(tls->fde);
		EVENT_FD_NOT_WRITEABLE(tls->fde);
		errno = EBADF;
		return -1;
	}
	if (!NT_STATUS_IS_OK(status)) {
		EVENT_FD_READABLE(tls->fde);
		errno = EAGAIN;
		return -1;
	}
	if (tls->output_pending) {
		EVENT_FD_WRITEABLE(tls->fde);
	}
	if (size != nread) {
		EVENT_FD_READABLE(tls->fde);
	}
	return nread;
}

/*
  callback for writing to a socket
*/
static ssize_t tls_push(gnutls_transport_ptr_t ptr, const void *buf, size_t size)
{
	struct tls_context *tls = talloc_get_type(ptr, struct tls_context);
	NTSTATUS status;
	size_t nwritten;
	DATA_BLOB b;

	if (!tls->tls_enabled) {
		return size;
	}

	b.data = discard_const(buf);
	b.length = size;

	status = socket_send(tls->socket, &b, &nwritten);
	if (NT_STATUS_EQUAL(status, STATUS_MORE_ENTRIES)) {
		errno = EAGAIN;
		return -1;
	}
	if (!NT_STATUS_IS_OK(status)) {
		EVENT_FD_WRITEABLE(tls->fde);
		return -1;
	}
	if (size != nwritten) {
		EVENT_FD_WRITEABLE(tls->fde);
	}
	return nwritten;
}

/*
  destroy a tls session
 */
static int tls_destructor(struct tls_context *tls)
{
	int ret;
	ret = gnutls_bye(tls->session, GNUTLS_SHUT_WR);
	if (ret < 0) {
		DEBUG(0,("TLS gnutls_bye failed - %s\n", gnutls_strerror(ret)));
	}
	return 0;
}


/*
  possibly continue the handshake process
*/
static NTSTATUS tls_handshake(struct tls_context *tls)
{
	int ret;

	if (tls->done_handshake) {
		return NT_STATUS_OK;
	}
	
	ret = gnutls_handshake(tls->session);
	if (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN) {
		if (gnutls_record_get_direction(tls->session) == 1) {
			EVENT_FD_WRITEABLE(tls->fde);
		}
		return STATUS_MORE_ENTRIES;
	}
	if (ret < 0) {
		DEBUG(0,("TLS gnutls_handshake failed - %s\n", gnutls_strerror(ret)));
		return NT_STATUS_UNEXPECTED_NETWORK_ERROR;
	}
	tls->done_handshake = True;
	return NT_STATUS_OK;
}

/*
  possibly continue an interrupted operation
*/
static NTSTATUS tls_interrupted(struct tls_context *tls)
{
	int ret;

	if (!tls->interrupted) {
		return NT_STATUS_OK;
	}
	if (gnutls_record_get_direction(tls->session) == 1) {
		ret = gnutls_record_send(tls->session, NULL, 0);
	} else {
		ret = gnutls_record_recv(tls->session, NULL, 0);
	}
	if (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN) {
		return STATUS_MORE_ENTRIES;
	}
	tls->interrupted = False;
	return NT_STATUS_OK;
}

/*
  see how many bytes are pending on the connection
*/
static NTSTATUS tls_socket_pending(struct socket_context *sock, size_t *npending)
{
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);
	if (!tls->tls_enabled || tls->tls_detect) {
		return socket_pending(tls->socket, npending);
	}
	*npending = gnutls_record_check_pending(tls->session);
	if (*npending == 0) {
		NTSTATUS status = socket_pending(tls->socket, npending);
		if (*npending == 0) {
			/* seems to be a gnutls bug */
			(*npending) = 100;
		}
		return status;
	}
	return NT_STATUS_OK;
}

/*
  receive data either by tls or normal socket_recv
*/
static NTSTATUS tls_socket_recv(struct socket_context *sock, void *buf, 
				size_t wantlen, size_t *nread)
{
	int ret;
	NTSTATUS status;
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);

	if (tls->tls_enabled && tls->tls_detect) {
		status = socket_recv(tls->socket, &tls->first_byte, 1, nread);
		NT_STATUS_NOT_OK_RETURN(status);
		if (*nread == 0) return NT_STATUS_OK;
		tls->tls_detect = False;
		/* look for the first byte of a valid HTTP operation */
		if (strchr(tls->plain_chars, tls->first_byte)) {
			/* not a tls link */
			tls->tls_enabled = False;
			*(uint8_t *)buf = tls->first_byte;
			return NT_STATUS_OK;
		}
		tls->have_first_byte = True;
	}

	if (!tls->tls_enabled) {
		return socket_recv(tls->socket, buf, wantlen, nread);
	}

	status = tls_handshake(tls);
	NT_STATUS_NOT_OK_RETURN(status);

	status = tls_interrupted(tls);
	NT_STATUS_NOT_OK_RETURN(status);

	ret = gnutls_record_recv(tls->session, buf, wantlen);
	if (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN) {
		if (gnutls_record_get_direction(tls->session) == 1) {
			EVENT_FD_WRITEABLE(tls->fde);
		}
		tls->interrupted = True;
		return STATUS_MORE_ENTRIES;
	}
	if (ret < 0) {
		return NT_STATUS_UNEXPECTED_NETWORK_ERROR;
	}
	*nread = ret;
	return NT_STATUS_OK;
}


/*
  send data either by tls or normal socket_recv
*/
static NTSTATUS tls_socket_send(struct socket_context *sock, 
				const DATA_BLOB *blob, size_t *sendlen)
{
	NTSTATUS status;
	int ret;
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);

	if (!tls->tls_enabled) {
		return socket_send(tls->socket, blob, sendlen);
	}

	status = tls_handshake(tls);
	NT_STATUS_NOT_OK_RETURN(status);

	status = tls_interrupted(tls);
	NT_STATUS_NOT_OK_RETURN(status);

	ret = gnutls_record_send(tls->session, blob->data, blob->length);
	if (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN) {
		if (gnutls_record_get_direction(tls->session) == 1) {
			EVENT_FD_WRITEABLE(tls->fde);
		}
		tls->interrupted = True;
		return STATUS_MORE_ENTRIES;
	}
	if (ret < 0) {
		DEBUG(0,("gnutls_record_send of %d failed - %s\n", (int)blob->length, gnutls_strerror(ret)));
		return NT_STATUS_UNEXPECTED_NETWORK_ERROR;
	}
	*sendlen = ret;
	tls->output_pending = (ret < blob->length);
	return NT_STATUS_OK;
}


/*
  initialise global tls state
*/
struct tls_params *tls_initialise(TALLOC_CTX *mem_ctx)
{
	struct tls_params *params;
	int ret;
	TALLOC_CTX *tmp_ctx = talloc_new(mem_ctx);
	const char *keyfile = private_path(tmp_ctx, lp_tls_keyfile());
	const char *certfile = private_path(tmp_ctx, lp_tls_certfile());
	const char *cafile = private_path(tmp_ctx, lp_tls_cafile());
	const char *crlfile = private_path(tmp_ctx, lp_tls_crlfile());
	const char *dhpfile = private_path(tmp_ctx, lp_tls_dhpfile());
	void tls_cert_generate(TALLOC_CTX *, const char *, const char *, const char *);

	params = talloc(mem_ctx, struct tls_params);
	if (params == NULL) {
		talloc_free(tmp_ctx);
		return NULL;
	}

	if (!lp_tls_enabled() || keyfile == NULL || *keyfile == 0) {
		params->tls_enabled = False;
		talloc_free(tmp_ctx);
		return params;
	}

	if (!file_exist(cafile)) {
		tls_cert_generate(params, keyfile, certfile, cafile);
	}

	ret = gnutls_global_init();
	if (ret < 0) goto init_failed;

	gnutls_certificate_allocate_credentials(&params->x509_cred);
	if (ret < 0) goto init_failed;

	if (cafile && *cafile) {
		ret = gnutls_certificate_set_x509_trust_file(params->x509_cred, cafile, 
							     GNUTLS_X509_FMT_PEM);	
		if (ret < 0) {
			DEBUG(0,("TLS failed to initialise cafile %s\n", cafile));
			goto init_failed;
		}
	}

	if (crlfile && *crlfile) {
		ret = gnutls_certificate_set_x509_crl_file(params->x509_cred, 
							   crlfile, 
							   GNUTLS_X509_FMT_PEM);
		if (ret < 0) {
			DEBUG(0,("TLS failed to initialise crlfile %s\n", crlfile));
			goto init_failed;
		}
	}
	
	ret = gnutls_certificate_set_x509_key_file(params->x509_cred, 
						   certfile, keyfile,
						   GNUTLS_X509_FMT_PEM);
	if (ret < 0) {
		DEBUG(0,("TLS failed to initialise certfile %s and keyfile %s\n", 
			 certfile, keyfile));
		goto init_failed;
	}
	
	
	ret = gnutls_dh_params_init(&params->dh_params);
	if (ret < 0) goto init_failed;

	if (dhpfile && *dhpfile) {
		gnutls_datum_t dhparms;
		size_t size;
		dhparms.data = (uint8_t *)file_load(dhpfile, &size, mem_ctx);

		if (!dhparms.data) {
			DEBUG(0,("Failed to read DH Parms from %s\n", dhpfile));
			goto init_failed;
		}
		dhparms.size = size;
			
		ret = gnutls_dh_params_import_pkcs3(params->dh_params, &dhparms, GNUTLS_X509_FMT_PEM);
		if (ret < 0) goto init_failed;
	} else {
		ret = gnutls_dh_params_generate2(params->dh_params, DH_BITS);
		if (ret < 0) goto init_failed;
	}
		
	gnutls_certificate_set_dh_params(params->x509_cred, params->dh_params);

	params->tls_enabled = True;

	talloc_free(tmp_ctx);
	return params;

init_failed:
	DEBUG(0,("GNUTLS failed to initialise - %s\n", gnutls_strerror(ret)));
	params->tls_enabled = False;
	talloc_free(tmp_ctx);
	return params;
}


/*
  setup for a new connection
*/
struct socket_context *tls_init_server(struct tls_params *params, 
				       struct socket_context *socket,
				       struct fd_event *fde, 
				       const char *plain_chars)
{
	struct tls_context *tls;
	int ret;
	struct socket_context *new_sock;
	NTSTATUS nt_status;
	
	nt_status = socket_create_with_ops(socket, &tls_socket_ops, &new_sock, 
					   SOCKET_TYPE_STREAM, 
					   socket->flags | SOCKET_FLAG_ENCRYPT);
	if (!NT_STATUS_IS_OK(nt_status)) {
		return NULL;
	}

	tls = talloc(new_sock, struct tls_context);
	if (tls == NULL) {
		return NULL;
	}

	tls->socket          = socket;
	tls->fde             = fde;
	if (talloc_reference(tls, fde) == NULL) {
		talloc_free(new_sock);
		return NULL;
	}
	if (talloc_reference(tls, socket) == NULL) {
		talloc_free(new_sock);
		return NULL;
	}

	new_sock->private_data    = tls;

	if (!params->tls_enabled) {
		talloc_free(new_sock);
		return NULL;
	}

	TLSCHECK(gnutls_init(&tls->session, GNUTLS_SERVER));

	talloc_set_destructor(tls, tls_destructor);

	TLSCHECK(gnutls_set_default_priority(tls->session));
	TLSCHECK(gnutls_credentials_set(tls->session, GNUTLS_CRD_CERTIFICATE, 
					params->x509_cred));
	gnutls_certificate_server_set_request(tls->session, GNUTLS_CERT_REQUEST);
	gnutls_dh_set_prime_bits(tls->session, DH_BITS);
	gnutls_transport_set_ptr(tls->session, (gnutls_transport_ptr_t)tls);
	gnutls_transport_set_pull_function(tls->session, (gnutls_pull_func)tls_pull);
	gnutls_transport_set_push_function(tls->session, (gnutls_push_func)tls_push);

	tls->plain_chars = plain_chars;
	if (plain_chars) {
		tls->tls_detect = True;
	} else {
		tls->tls_detect = False;
	}

	tls->output_pending  = False;
	tls->done_handshake  = False;
	tls->have_first_byte = False;
	tls->tls_enabled     = True;
	tls->interrupted     = False;
	
	new_sock->state = SOCKET_STATE_SERVER_CONNECTED;

	return new_sock;

failed:
	DEBUG(0,("TLS init connection failed - %s\n", gnutls_strerror(ret)));
	talloc_free(new_sock);
	return NULL;
}


/*
  setup for a new client connection
*/
struct socket_context *tls_init_client(struct socket_context *socket,
				       struct fd_event *fde)
{
	struct tls_context *tls;
	int ret = 0;
	char *cafile;
	struct socket_context *new_sock;
	NTSTATUS nt_status;
	
	nt_status = socket_create_with_ops(socket, &tls_socket_ops, &new_sock, 
					   SOCKET_TYPE_STREAM, 
					   socket->flags | SOCKET_FLAG_ENCRYPT);
	if (!NT_STATUS_IS_OK(nt_status)) {
		return NULL;
	}

	tls = talloc(new_sock, struct tls_context);
	if (tls == NULL) return NULL;

	tls->socket          = socket;
	tls->fde             = fde;
	if (talloc_reference(tls, fde) == NULL) {
		return NULL;
	}
	if (talloc_reference(tls, socket) == NULL) {
		return NULL;
	}
	new_sock->private_data    = tls;

	cafile = private_path(tls, lp_tls_cafile());
	if (!cafile || !*cafile) {
		goto failed;
	}

	gnutls_global_init();

	gnutls_certificate_allocate_credentials(&tls->xcred);
	gnutls_certificate_set_x509_trust_file(tls->xcred, cafile, GNUTLS_X509_FMT_PEM);
	talloc_free(cafile);
	TLSCHECK(gnutls_init(&tls->session, GNUTLS_CLIENT));
	TLSCHECK(gnutls_set_default_priority(tls->session));
	TLSCHECK(gnutls_credentials_set(tls->session, GNUTLS_CRD_CERTIFICATE, tls->xcred));

	talloc_set_destructor(tls, tls_destructor);

	gnutls_transport_set_ptr(tls->session, (gnutls_transport_ptr_t)tls);
	gnutls_transport_set_pull_function(tls->session, (gnutls_pull_func)tls_pull);
	gnutls_transport_set_push_function(tls->session, (gnutls_push_func)tls_push);
	tls->tls_detect = False;

	tls->output_pending  = False;
	tls->done_handshake  = False;
	tls->have_first_byte = False;
	tls->tls_enabled     = True;
	tls->interrupted     = False;
	
	new_sock->state = SOCKET_STATE_CLIENT_CONNECTED;

	return new_sock;

failed:
	DEBUG(0,("TLS init connection failed - %s\n", gnutls_strerror(ret)));
	tls->tls_enabled = False;
	return new_sock;
}

static NTSTATUS tls_socket_set_option(struct socket_context *sock, const char *option, const char *val)
{
	set_socket_options(socket_get_fd(sock), option);
	return NT_STATUS_OK;
}

static char *tls_socket_get_peer_name(struct socket_context *sock, TALLOC_CTX *mem_ctx)
{
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);
	return socket_get_peer_name(tls->socket, mem_ctx);
}

static struct socket_address *tls_socket_get_peer_addr(struct socket_context *sock, TALLOC_CTX *mem_ctx)
{
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);
	return socket_get_peer_addr(tls->socket, mem_ctx);
}

static struct socket_address *tls_socket_get_my_addr(struct socket_context *sock, TALLOC_CTX *mem_ctx)
{
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);
	return socket_get_my_addr(tls->socket, mem_ctx);
}

static int tls_socket_get_fd(struct socket_context *sock)
{
	struct tls_context *tls = talloc_get_type(sock->private_data, struct tls_context);
	return socket_get_fd(tls->socket);
}

static const struct socket_ops tls_socket_ops = {
	.name			= "tls",
	.fn_init		= tls_socket_init,
	.fn_recv		= tls_socket_recv,
	.fn_send		= tls_socket_send,
	.fn_pending		= tls_socket_pending,

	.fn_set_option		= tls_socket_set_option,

	.fn_get_peer_name	= tls_socket_get_peer_name,
	.fn_get_peer_addr	= tls_socket_get_peer_addr,
	.fn_get_my_addr		= tls_socket_get_my_addr,
	.fn_get_fd		= tls_socket_get_fd
};

BOOL tls_support(struct tls_params *params)
{
	return params->tls_enabled;
}

#else

/* for systems without tls we just fail the operations, and the caller
 * will retain the original socket */

struct tls_params *tls_initialise(TALLOC_CTX *mem_ctx)
{
	return talloc_new(mem_ctx);
}

/*
  setup for a new connection
*/
struct socket_context *tls_init_server(struct tls_params *params, 
				    struct socket_context *socket,
				    struct fd_event *fde, 
				    const char *plain_chars)
{
	return NULL;
}


/*
  setup for a new client connection
*/
struct socket_context *tls_init_client(struct socket_context *socket,
				       struct fd_event *fde)
{
	return NULL;
}

BOOL tls_support(struct tls_params *params)
{
	return False;
}

#endif

