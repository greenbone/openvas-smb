/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __PARAM_SECRETS_PROTO_H__
#define __PARAM_SECRETS_PROTO_H__

#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2) PRINTF_ATTRIBUTE(a1, a2)
/* This file was automatically generated by mkproto.pl. DO NOT EDIT */

#ifndef _PUBLIC_
#define _PUBLIC_
#endif


/* The following definitions come from param/secrets.c  */

void secrets_shutdown(void);
BOOL secrets_init(void);
struct ldb_context *secrets_db_connect(TALLOC_CTX *mem_ctx);
struct dom_sid *secrets_get_domain_sid(TALLOC_CTX *mem_ctx,
				       const char *domain);
#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2)

#endif /* __PARAM_SECRETS_PROTO_H__ */

