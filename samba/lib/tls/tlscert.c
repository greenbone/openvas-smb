/*
 * SPDX-FileCopyrightText: Andrew Tridgell 2005
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/* 
   Unix SMB/CIFS implementation.

   auto-generate self signed TLS certificates

   Copyright (C) Andrew Tridgell 2005
   
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

#if ENABLE_GNUTLS
#include "gnutls/gnutls.h"
#include "gnutls/x509.h"

#define ORGANISATION_NAME "Samba Administration"
#define UNIT_NAME         "Samba - temporary autogenerated certificate"
#define COMMON_NAME       "Samba"
#define LIFETIME          700*24*60*60
#define DH_BITS 		  1024

/* 
   auto-generate a set of self signed certificates
*/
void tls_cert_generate(TALLOC_CTX *mem_ctx, 
		       const char *keyfile, const char *certfile,
		       const char *cafile)
{
	gnutls_x509_crt cacrt, crt;
	gnutls_x509_privkey key, cakey;
	uint32_t serial = (uint32_t)time(NULL);
	unsigned char keyid[100];
	char buf[4096];
	size_t bufsize;
	size_t keyidsize = sizeof(keyid);
	time_t activation = time(NULL), expiry = activation + LIFETIME;
	int ret;

	if (file_exist(keyfile) || file_exist(certfile) || file_exist(cafile)) {
		DEBUG(0,("TLS autogeneration skipped - some TLS files already exist\n"));
		return;
	}

#define TLSCHECK(call) do { \
	ret = call; \
	if (ret < 0) { \
		DEBUG(0,("TLS %s - %s\n", #call, gnutls_strerror(ret))); \
		goto failed; \
	} \
} while (0)

	TLSCHECK(gnutls_global_init());

	DEBUG(0,("Attempting to autogenerate TLS self-signed keys for https\n"));
	
	DEBUG(3,("Generating private key\n"));
	TLSCHECK(gnutls_x509_privkey_init(&key));
	TLSCHECK(gnutls_x509_privkey_generate(key,   GNUTLS_PK_RSA, DH_BITS, 0));

	DEBUG(3,("Generating CA private key\n"));
	TLSCHECK(gnutls_x509_privkey_init(&cakey));
	TLSCHECK(gnutls_x509_privkey_generate(cakey, GNUTLS_PK_RSA, DH_BITS, 0));

	DEBUG(3,("Generating CA certificate\n"));
	TLSCHECK(gnutls_x509_crt_init(&cacrt));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(cacrt, 
				      GNUTLS_OID_X520_ORGANIZATION_NAME, 0,
				      ORGANISATION_NAME, strlen(ORGANISATION_NAME)));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(cacrt, 
				      GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME, 0,
				      UNIT_NAME, strlen(UNIT_NAME)));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(cacrt,
				      GNUTLS_OID_X520_COMMON_NAME, 0,
				      COMMON_NAME, strlen(COMMON_NAME)));
	TLSCHECK(gnutls_x509_crt_set_key(cacrt, cakey));
	TLSCHECK(gnutls_x509_crt_set_serial(cacrt, &serial, sizeof(serial)));
	TLSCHECK(gnutls_x509_crt_set_activation_time(cacrt, activation));
	TLSCHECK(gnutls_x509_crt_set_expiration_time(cacrt, expiry));
	TLSCHECK(gnutls_x509_crt_set_ca_status(cacrt, 0));
#ifdef GNUTLS_KP_TLS_WWW_SERVER
	TLSCHECK(gnutls_x509_crt_set_key_purpose_oid(cacrt, GNUTLS_KP_TLS_WWW_SERVER, 0));
#endif
	TLSCHECK(gnutls_x509_crt_set_version(cacrt, 3));
	TLSCHECK(gnutls_x509_crt_get_key_id(cacrt, 0, keyid, &keyidsize));
#if HAVE_GNUTLS_X509_CRT_SET_SUBJECT_KEY_ID
	TLSCHECK(gnutls_x509_crt_set_subject_key_id(cacrt, keyid, keyidsize));
#endif
	TLSCHECK(gnutls_x509_crt_sign(cacrt, cacrt, cakey));

	DEBUG(3,("Generating TLS certificate\n"));
	TLSCHECK(gnutls_x509_crt_init(&crt));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(crt, 
				      GNUTLS_OID_X520_ORGANIZATION_NAME, 0,
				      ORGANISATION_NAME, strlen(ORGANISATION_NAME)));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(crt, 
				      GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME, 0,
				      UNIT_NAME, strlen(UNIT_NAME)));
	TLSCHECK(gnutls_x509_crt_set_dn_by_oid(crt,
				      GNUTLS_OID_X520_COMMON_NAME, 0,
				      COMMON_NAME, strlen(COMMON_NAME)));
	TLSCHECK(gnutls_x509_crt_set_key(crt, key));
	TLSCHECK(gnutls_x509_crt_set_serial(crt, &serial, sizeof(serial)));
	TLSCHECK(gnutls_x509_crt_set_activation_time(crt, activation));
	TLSCHECK(gnutls_x509_crt_set_expiration_time(crt, expiry));
	TLSCHECK(gnutls_x509_crt_set_ca_status(crt, 0));
#ifdef GNUTLS_KP_TLS_WWW_SERVER
	TLSCHECK(gnutls_x509_crt_set_key_purpose_oid(crt, GNUTLS_KP_TLS_WWW_SERVER, 0));
#endif
	TLSCHECK(gnutls_x509_crt_set_version(crt, 3));
	TLSCHECK(gnutls_x509_crt_get_key_id(crt, 0, keyid, &keyidsize));
#if HAVE_GNUTLS_X509_CRT_SET_SUBJECT_KEY_ID
	TLSCHECK(gnutls_x509_crt_set_subject_key_id(crt, keyid, keyidsize));
#endif
	TLSCHECK(gnutls_x509_crt_sign(crt, crt, key));

	DEBUG(3,("Exporting TLS keys\n"));

	bufsize = sizeof(buf);
	TLSCHECK(gnutls_x509_crt_export(crt, GNUTLS_X509_FMT_PEM, buf, &bufsize));
	file_save(certfile, buf, bufsize);

	bufsize = sizeof(buf);
	TLSCHECK(gnutls_x509_crt_export(cacrt, GNUTLS_X509_FMT_PEM, buf, &bufsize));
	file_save(cafile, buf, bufsize);

	bufsize = sizeof(buf);
	TLSCHECK(gnutls_x509_privkey_export(key, GNUTLS_X509_FMT_PEM, buf, &bufsize));
	file_save(keyfile, buf, bufsize);

	gnutls_x509_privkey_deinit(key);
	gnutls_x509_privkey_deinit(cakey);
	gnutls_x509_crt_deinit(cacrt);
	gnutls_x509_crt_deinit(crt);
	gnutls_global_deinit();

	DEBUG(0,("TLS self-signed keys generated OK\n"));
	return;

failed:
	DEBUG(0,("TLS certificate generation failed\n"));
}

#else
 void tls_cert_dummy(void) {}
#endif
