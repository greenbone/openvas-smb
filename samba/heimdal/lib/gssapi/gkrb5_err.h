/* Generated from /home/aheinecke/arbeit/greenbone/src/wmi-work/Samba/source/heimdal/lib/gssapi/krb5/gkrb5_err.et */
/* $Id: gkrb5_err.et,v 1.1 2006/11/09 23:52:17 lha Exp $ */

#ifndef __gkrb5_err_h__
#define __gkrb5_err_h__

struct et_list;

void initialize_gk5_error_table_r(struct et_list **);

void initialize_gk5_error_table(void);
#define init_gk5_err_tbl initialize_gk5_error_table

typedef enum gk5_error_number{
	GSS_KRB5_S_G_BAD_SERVICE_NAME = 35224064,
	GSS_KRB5_S_G_BAD_STRING_UID = 35224065,
	GSS_KRB5_S_G_NOUSER = 35224066,
	GSS_KRB5_S_G_VALIDATE_FAILED = 35224067,
	GSS_KRB5_S_G_BUFFER_ALLOC = 35224068,
	GSS_KRB5_S_G_BAD_MSG_CTX = 35224069,
	GSS_KRB5_S_G_WRONG_SIZE = 35224070,
	GSS_KRB5_S_G_BAD_USAGE = 35224071,
	GSS_KRB5_S_G_UNKNOWN_QOP = 35224072,
	GSS_KRB5_S_KG_CCACHE_NOMATCH = 35224192,
	GSS_KRB5_S_KG_KEYTAB_NOMATCH = 35224193,
	GSS_KRB5_S_KG_TGT_MISSING = 35224194,
	GSS_KRB5_S_KG_NO_SUBKEY = 35224195,
	GSS_KRB5_S_KG_CONTEXT_ESTABLISHED = 35224196,
	GSS_KRB5_S_KG_BAD_SIGN_TYPE = 35224197,
	GSS_KRB5_S_KG_BAD_LENGTH = 35224198,
	GSS_KRB5_S_KG_CTX_INCOMPLETE = 35224199
} gk5_error_number;

#define ERROR_TABLE_BASE_gk5 35224064

#endif /* __gkrb5_err_h__ */
