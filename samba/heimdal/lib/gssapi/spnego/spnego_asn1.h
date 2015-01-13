/* Generated from spnego.asn1 */
/* Do not edit */

#ifndef __spnego_asn1_h__
#define __spnego_asn1_h__

#include <stddef.h>
#include <time.h>

#ifndef __asn1_common_definitions__
#define __asn1_common_definitions__

typedef struct heim_integer {
  size_t length;
  void *data;
  int negative;
} heim_integer;

typedef struct heim_octet_string {
  size_t length;
  void *data;
} heim_octet_string;

typedef char *heim_general_string;

typedef char *heim_utf8_string;

typedef char *heim_printable_string;

typedef char *heim_ia5_string;

typedef struct heim_bmp_string {
  size_t length;
  uint16_t *data;
} heim_bmp_string;

typedef struct heim_universal_string {
  size_t length;
  uint32_t *data;
} heim_universal_string;

typedef struct heim_oid {
  size_t length;
  unsigned *components;
} heim_oid;

typedef struct heim_bit_string {
  size_t length;
  void *data;
} heim_bit_string;

typedef struct heim_octet_string heim_any;
typedef struct heim_octet_string heim_any_set;

#define ASN1_MALLOC_ENCODE(T, B, BL, S, L, R)                  \
  do {                                                         \
    (BL) = length_##T((S));                                    \
    (B) = malloc((BL));                                        \
    if((B) == NULL) {                                          \
      (R) = ENOMEM;                                            \
    } else {                                                   \
      (R) = encode_##T(((unsigned char*)(B)) + (BL) - 1, (BL), \
                       (S), (L));                              \
      if((R) != 0) {                                           \
        free((B));                                             \
        (B) = NULL;                                            \
      }                                                        \
    }                                                          \
  } while (0)

#endif

/*
MechType ::= OBJECT IDENTIFIER
*/

typedef heim_oid MechType;

int    encode_MechType(unsigned char *, size_t, const MechType *, size_t *);
int    decode_MechType(const unsigned char *, size_t, MechType *, size_t *);
void   free_MechType  (MechType *);
size_t length_MechType(const MechType *);
int    copy_MechType  (const MechType *, MechType *);


/*
MechTypeList ::= SEQUENCE OF MechType
*/

typedef struct MechTypeList {
  unsigned int len;
  MechType *val;
} MechTypeList;

int    encode_MechTypeList(unsigned char *, size_t, const MechTypeList *, size_t *);
int    decode_MechTypeList(const unsigned char *, size_t, MechTypeList *, size_t *);
void   free_MechTypeList  (MechTypeList *);
size_t length_MechTypeList(const MechTypeList *);
int    copy_MechTypeList  (const MechTypeList *, MechTypeList *);


/*
ContextFlags ::= BIT STRING {
  delegFlag(0),
  mutualFlag(1),
  replayFlag(2),
  sequenceFlag(3),
  anonFlag(4),
  confFlag(5),
  integFlag(6)
}
*/

typedef struct ContextFlags {
  unsigned int delegFlag:1;
  unsigned int mutualFlag:1;
  unsigned int replayFlag:1;
  unsigned int sequenceFlag:1;
  unsigned int anonFlag:1;
  unsigned int confFlag:1;
  unsigned int integFlag:1;
} ContextFlags;


int    encode_ContextFlags(unsigned char *, size_t, const ContextFlags *, size_t *);
int    decode_ContextFlags(const unsigned char *, size_t, ContextFlags *, size_t *);
void   free_ContextFlags  (ContextFlags *);
size_t length_ContextFlags(const ContextFlags *);
int    copy_ContextFlags  (const ContextFlags *, ContextFlags *);
unsigned ContextFlags2int(ContextFlags);
ContextFlags int2ContextFlags(unsigned);
const struct units * asn1_ContextFlags_units(void);

/*
NegHints ::= SEQUENCE {
  hintName        [0] GeneralString OPTIONAL,
  hintAddress     [1] OCTET STRING OPTIONAL,
}
*/

typedef struct NegHints {
  heim_general_string *hintName;
  heim_octet_string *hintAddress;
} NegHints;

int    encode_NegHints(unsigned char *, size_t, const NegHints *, size_t *);
int    decode_NegHints(const unsigned char *, size_t, NegHints *, size_t *);
void   free_NegHints  (NegHints *);
size_t length_NegHints(const NegHints *);
int    copy_NegHints  (const NegHints *, NegHints *);


/*
NegTokenInit ::= SEQUENCE {
  mechTypes       [0] MechTypeList,
  reqFlags        [1] ContextFlags OPTIONAL,
  mechToken       [2] OCTET STRING OPTIONAL,
  negHints        [3] NegHints OPTIONAL,
  mechListMIC     [4] OCTET STRING OPTIONAL,
}
*/

typedef struct NegTokenInit {
  MechTypeList mechTypes;
  ContextFlags *reqFlags;
  heim_octet_string *mechToken;
  NegHints *negHints;
  heim_octet_string *mechListMIC;
} NegTokenInit;

int    encode_NegTokenInit(unsigned char *, size_t, const NegTokenInit *, size_t *);
int    decode_NegTokenInit(const unsigned char *, size_t, NegTokenInit *, size_t *);
void   free_NegTokenInit  (NegTokenInit *);
size_t length_NegTokenInit(const NegTokenInit *);
int    copy_NegTokenInit  (const NegTokenInit *, NegTokenInit *);


/*
NegTokenResp ::= SEQUENCE {
  negResult       [0] INTEGER {
    accept_completed(0),
    accept_incomplete(1),
    reject(2),
    request_mic(3)
  } OPTIONAL,
  supportedMech   [1] MechType OPTIONAL,
  responseToken   [2] OCTET STRING OPTIONAL,
  mechListMIC     [3] OCTET STRING OPTIONAL,
}
*/

typedef struct NegTokenResp {
  enum  {
    accept_completed = 0,
    accept_incomplete = 1,
    reject = 2,
    request_mic = 3
} *negResult;
  MechType *supportedMech;
  heim_octet_string *responseToken;
  heim_octet_string *mechListMIC;
} NegTokenResp;

int    encode_NegTokenResp(unsigned char *, size_t, const NegTokenResp *, size_t *);
int    decode_NegTokenResp(const unsigned char *, size_t, NegTokenResp *, size_t *);
void   free_NegTokenResp  (NegTokenResp *);
size_t length_NegTokenResp(const NegTokenResp *);
int    copy_NegTokenResp  (const NegTokenResp *, NegTokenResp *);


/*
NegotiationToken ::= CHOICE {
  negTokenInit    [0] NegTokenInit,
  negTokenResp    [1] NegTokenResp,
}
*/

typedef struct NegotiationToken {
  enum {
    choice_NegotiationToken_negTokenInit = 1,
    choice_NegotiationToken_negTokenResp
  } element;
  union {
    NegTokenInit negTokenInit;
    NegTokenResp negTokenResp;
  } u;
} NegotiationToken;

int    encode_NegotiationToken(unsigned char *, size_t, const NegotiationToken *, size_t *);
int    decode_NegotiationToken(const unsigned char *, size_t, NegotiationToken *, size_t *);
void   free_NegotiationToken  (NegotiationToken *);
size_t length_NegotiationToken(const NegotiationToken *);
int    copy_NegotiationToken  (const NegotiationToken *, NegotiationToken *);


#endif /* __spnego_asn1_h__ */
