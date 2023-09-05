#ifndef __LIBCLI_NBT_NBTNAME_H__
#define __LIBCLI_NBT_NBTNAME_H__

#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2) PRINTF_ATTRIBUTE(a1, a2)
/* This file was automatically generated by mkproto.pl. DO NOT EDIT */

#ifndef _PUBLIC_
#define _PUBLIC_
#endif


/* The following definitions come from libcli/nbt/nbtname.c  */

_PUBLIC_ void ndr_print_nbt_string(struct ndr_print *ndr, const char *name, const char *s);
_PUBLIC_ NTSTATUS ndr_pull_nbt_string(struct ndr_pull *ndr, int ndr_flags, const char **s);
_PUBLIC_ NTSTATUS ndr_push_nbt_string(struct ndr_push *ndr, int ndr_flags, const char *s);
_PUBLIC_ NTSTATUS ndr_pull_nbt_name(struct ndr_pull *ndr, int ndr_flags, struct nbt_name *r);
_PUBLIC_ NTSTATUS ndr_push_nbt_name(struct ndr_push *ndr, int ndr_flags, const struct nbt_name *r);
_PUBLIC_ NTSTATUS nbt_name_dup(TALLOC_CTX *mem_ctx, struct nbt_name *name, struct nbt_name *newname);
_PUBLIC_ NTSTATUS nbt_name_to_blob(TALLOC_CTX *mem_ctx, DATA_BLOB *blob, struct nbt_name *name);
_PUBLIC_ NTSTATUS nbt_name_from_blob(TALLOC_CTX *mem_ctx, const DATA_BLOB *blob, struct nbt_name *name);
_PUBLIC_ void nbt_choose_called_name(TALLOC_CTX *mem_ctx,
			    struct nbt_name *n, const char *name, int type);
_PUBLIC_ char *nbt_name_string(TALLOC_CTX *mem_ctx, const struct nbt_name *name);
_PUBLIC_ NTSTATUS ndr_pull_wrepl_nbt_name(struct ndr_pull *ndr, int ndr_flags, struct nbt_name **_r);
_PUBLIC_ NTSTATUS ndr_push_wrepl_nbt_name(struct ndr_push *ndr, int ndr_flags, const struct nbt_name *r);
_PUBLIC_ void ndr_print_wrepl_nbt_name(struct ndr_print *ndr, const char *name, const struct nbt_name *r);
#undef _PRINTF_ATTRIBUTE
#define _PRINTF_ATTRIBUTE(a1, a2)

#endif /* __LIBCLI_NBT_NBTNAME_H__ */

