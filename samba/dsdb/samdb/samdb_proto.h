/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __DSDB_SAMDB_SAMDB_PROTO_H__
#define __DSDB_SAMDB_SAMDB_PROTO_H__

#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2) PRINTF_ATTRIBUTE(a1, a2)
/* This file was automatically generated by mkproto.pl. DO NOT EDIT */

#ifndef _PUBLIC_
#define _PUBLIC_
#endif


/* The following definitions come from dsdb/samdb/samdb.c  */

struct ldb_context *samdb_connect(TALLOC_CTX *mem_ctx, 
				  struct auth_session_info *session_info);
int samdb_search_domain(struct ldb_context *sam_ldb,
			TALLOC_CTX *mem_ctx, 
			struct ldb_dn *basedn,
			struct ldb_message ***res,
			const char * const *attrs,
			const struct dom_sid *domain_sid,
			const char *format, ...)  _PRINTF_ATTRIBUTE(7,8);
const char *samdb_search_string_v(struct ldb_context *sam_ldb,
				  TALLOC_CTX *mem_ctx,
				  struct ldb_dn *basedn,
				  const char *attr_name,
				  const char *format, va_list ap) _PRINTF_ATTRIBUTE(5,0);
const char *samdb_search_string(struct ldb_context *sam_ldb,
				TALLOC_CTX *mem_ctx,
				struct ldb_dn *basedn,
				const char *attr_name,
				const char *format, ...) _PRINTF_ATTRIBUTE(5,6);
struct ldb_dn *samdb_search_dn(struct ldb_context *sam_ldb,
			       TALLOC_CTX *mem_ctx,
			       struct ldb_dn *basedn,
			       const char *format, ...) _PRINTF_ATTRIBUTE(4,5);
struct dom_sid *samdb_search_dom_sid(struct ldb_context *sam_ldb,
				     TALLOC_CTX *mem_ctx,
				     struct ldb_dn *basedn,
				     const char *attr_name,
				     const char *format, ...) _PRINTF_ATTRIBUTE(5,6);
int samdb_search_count(struct ldb_context *sam_ldb,
		       TALLOC_CTX *mem_ctx,
		       struct ldb_dn *basedn,
		       const char *format, ...) _PRINTF_ATTRIBUTE(4,5);
uint_t samdb_search_uint(struct ldb_context *sam_ldb,
			 TALLOC_CTX *mem_ctx,
			 uint_t default_value,
			 struct ldb_dn *basedn,
			 const char *attr_name,
			 const char *format, ...) _PRINTF_ATTRIBUTE(6,7);
int64_t samdb_search_int64(struct ldb_context *sam_ldb,
			   TALLOC_CTX *mem_ctx,
			   int64_t default_value,
			   struct ldb_dn *basedn,
			   const char *attr_name,
			   const char *format, ...) _PRINTF_ATTRIBUTE(6,7);
int samdb_search_string_multiple(struct ldb_context *sam_ldb,
				 TALLOC_CTX *mem_ctx,
				 struct ldb_dn *basedn,
				 const char ***strs,
				 const char *attr_name,
				 const char *format, ...) _PRINTF_ATTRIBUTE(6,7);
uint_t samdb_result_uint(const struct ldb_message *msg, const char *attr, uint_t default_value);
int64_t samdb_result_int64(const struct ldb_message *msg, const char *attr, int64_t default_value);
const char *samdb_result_string(const struct ldb_message *msg, const char *attr, 
				const char *default_value);
struct ldb_dn *samdb_result_dn(struct ldb_context *ldb, TALLOC_CTX *mem_ctx, const struct ldb_message *msg,
			       const char *attr, struct ldb_dn *default_value);
uint32_t samdb_result_rid_from_sid(TALLOC_CTX *mem_ctx, const struct ldb_message *msg, 
				   const char *attr, uint32_t default_value);
struct dom_sid *samdb_result_dom_sid(TALLOC_CTX *mem_ctx, const struct ldb_message *msg, 
				     const char *attr);
struct GUID samdb_result_guid(const struct ldb_message *msg, const char *attr);
struct dom_sid *samdb_result_sid_prefix(TALLOC_CTX *mem_ctx, const struct ldb_message *msg, 
					const char *attr);
NTTIME samdb_result_nttime(struct ldb_message *msg, const char *attr, NTTIME default_value);
uint64_t samdb_result_uint64(struct ldb_message *msg, const char *attr, uint64_t default_value);
NTTIME samdb_result_allow_password_change(struct ldb_context *sam_ldb, 
					  TALLOC_CTX *mem_ctx, 
					  struct ldb_dn *domain_dn, 
					  struct ldb_message *msg, 
					  const char *attr);
NTTIME samdb_result_force_password_change(struct ldb_context *sam_ldb, 
					  TALLOC_CTX *mem_ctx, 
					  struct ldb_dn *domain_dn, 
					  struct ldb_message *msg);
struct samr_Password *samdb_result_hash(TALLOC_CTX *mem_ctx, struct ldb_message *msg, const char *attr);
uint_t samdb_result_hashes(TALLOC_CTX *mem_ctx, struct ldb_message *msg, 
			   const char *attr, struct samr_Password **hashes);
NTSTATUS samdb_result_passwords(TALLOC_CTX *mem_ctx, struct ldb_message *msg, 
				struct samr_Password **lm_pwd, struct samr_Password **nt_pwd) ;
struct samr_LogonHours samdb_result_logon_hours(TALLOC_CTX *mem_ctx, struct ldb_message *msg, const char *attr);
uint16_t samdb_result_acct_flags(struct ldb_message *msg, const char *attr);
struct ldb_message_element *samdb_find_attribute(struct ldb_context *ldb, 
						 const struct ldb_message *msg, 
						 const char *name, const char *value);
int samdb_find_or_add_value(struct ldb_context *ldb, struct ldb_message *msg, const char *name, const char *set_value);
int samdb_find_or_add_attribute(struct ldb_context *ldb, struct ldb_message *msg, const char *name, const char *set_value);
int samdb_copy_template(struct ldb_context *ldb, 
			struct ldb_message *msg, const char *filter,
			const char **errstring);
int samdb_msg_add_string(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, const char *str);
int samdb_msg_add_dom_sid(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, struct dom_sid *sid);
int samdb_msg_add_delete(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name);
int samdb_msg_add_addval(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, const char *value);
int samdb_msg_add_delval(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, const char *value);
int samdb_msg_add_int(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
		       const char *attr_name, int v);
int samdb_msg_add_uint(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
		       const char *attr_name, uint_t v);
int samdb_msg_add_int64(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			const char *attr_name, int64_t v);
int samdb_msg_add_uint64(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			const char *attr_name, uint64_t v);
int samdb_msg_add_hash(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
		       const char *attr_name, struct samr_Password *hash);
int samdb_msg_add_hashes(TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, struct samr_Password *hashes, uint_t count);
int samdb_msg_add_acct_flags(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			     const char *attr_name, uint32_t v);
int samdb_msg_add_logon_hours(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			      const char *attr_name, struct samr_LogonHours *hours);
int samdb_msg_add_value(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			      const char *attr_name, const struct ldb_val *val);
int samdb_msg_set_value(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			const char *attr_name, const struct ldb_val *val);
int samdb_msg_set_string(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg,
			 const char *attr_name, const char *str);
int samdb_add(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg);
int samdb_delete(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_dn *dn);
int samdb_modify(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg);
int samdb_replace(struct ldb_context *sam_ldb, TALLOC_CTX *mem_ctx, struct ldb_message *msg);
struct security_descriptor *samdb_default_security_descriptor(TALLOC_CTX *mem_ctx);
struct ldb_dn *samdb_base_dn(struct ldb_context *sam_ctx) ;
struct ldb_dn *samdb_partitions_dn(struct ldb_context *sam_ctx, TALLOC_CTX *mem_ctx);
const struct dom_sid *samdb_domain_sid(struct ldb_context *ldb);
_PUBLIC_ NTSTATUS samdb_set_password(struct ldb_context *ctx, TALLOC_CTX *mem_ctx,
			    struct ldb_dn *user_dn,
			    struct ldb_dn *domain_dn,
			    struct ldb_message *mod,
			    const char *new_pass,
			    struct samr_Password *lmNewHash, 
			    struct samr_Password *ntNewHash,
			    BOOL user_change,
			    BOOL restrictions,
			    enum samr_RejectReason *reject_reason,
			    struct samr_DomInfo1 **_dominfo);
_PUBLIC_ NTSTATUS samdb_set_password_sid(struct ldb_context *ctx, TALLOC_CTX *mem_ctx,
				const struct dom_sid *user_sid,
				const char *new_pass,
				struct samr_Password *lmNewHash, 
				struct samr_Password *ntNewHash,
				BOOL user_change,
				BOOL restrictions,
				enum samr_RejectReason *reject_reason,
				struct samr_DomInfo1 **_dominfo) ;
NTSTATUS security_token_create(TALLOC_CTX *mem_ctx, 
			       struct dom_sid *user_sid,
			       struct dom_sid *group_sid, 
			       int n_groupSIDs,
			       struct dom_sid **groupSIDs, 
			       BOOL is_authenticated,
			       struct security_token **token);
NTSTATUS samdb_create_foreign_security_principal(struct ldb_context *sam_ctx, TALLOC_CTX *mem_ctx, 
						 struct dom_sid *sid, struct ldb_dn **ret_dn) ;

/* The following definitions come from dsdb/samdb/samdb_privilege.c  */

_PUBLIC_ NTSTATUS samdb_privilege_setup(struct security_token *token);

/* The following definitions come from dsdb/samdb/cracknames.c  */

WERROR DsCrackNameOneName(struct ldb_context *sam_ctx, TALLOC_CTX *mem_ctx,
			  uint32_t format_flags, uint32_t format_offered, uint32_t format_desired,
			  const char *name, struct drsuapi_DsNameInfo1 *info1);
NTSTATUS crack_user_principal_name(struct ldb_context *sam_ctx, 
				   TALLOC_CTX *mem_ctx, 
				   const char *user_principal_name, 
				   struct ldb_dn **user_dn,
				   struct ldb_dn **domain_dn) ;
NTSTATUS crack_service_principal_name(struct ldb_context *sam_ctx, 
				      TALLOC_CTX *mem_ctx, 
				      const char *service_principal_name, 
				      struct ldb_dn **user_dn,
				      struct ldb_dn **domain_dn) ;
NTSTATUS crack_dn_to_nt4_name(TALLOC_CTX *mem_ctx, 
			      const char *dn, 
			      const char **nt4_domain, const char **nt4_account);

/* The following definitions come from dsdb/common/sidmap.c  */

_PUBLIC_ struct sidmap_context *sidmap_open(TALLOC_CTX *mem_ctx);
_PUBLIC_ NTSTATUS sidmap_sid_to_unixuid(struct sidmap_context *sidmap, 
					struct dom_sid *sid, uid_t *uid);
_PUBLIC_ BOOL sidmap_sid_is_group(struct sidmap_context *sidmap, struct dom_sid *sid);
_PUBLIC_ NTSTATUS sidmap_sid_to_unixgid(struct sidmap_context *sidmap,
					struct dom_sid *sid, gid_t *gid);
_PUBLIC_ NTSTATUS sidmap_uid_to_sid(struct sidmap_context *sidmap,
				    TALLOC_CTX *mem_ctx,
				    uid_t uid, struct dom_sid **sid);
_PUBLIC_ NTSTATUS sidmap_gid_to_sid(struct sidmap_context *sidmap,
				    TALLOC_CTX *mem_ctx,
				    gid_t gid, struct dom_sid **sid);
_PUBLIC_ NTSTATUS sidmap_allocated_sid_lookup(struct sidmap_context *sidmap, 
					      TALLOC_CTX *mem_ctx, 
					      const struct dom_sid *sid,
					      const char **name,
					      uint32_t *atype);

/* The following definitions come from dsdb/common/flag_mapping.c  */

uint32_t samdb_acb2uf(uint32_t acb);
uint32_t samdb_uf2acb(uint32_t uf);
uint32_t samdb_uf2atype(uint32_t uf);
uint32_t samdb_gtype2atype(uint32_t gtype);
enum lsa_SidType samdb_atype_map(uint32_t atype);
#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2)

#endif /* __DSDB_SAMDB_SAMDB_PROTO_H__ */

