#ifndef MD5_H
#define MD5_H
#ifndef HEADER_MD5_H
/* Try to avoid clashes with OpenSSL */
#define HEADER_MD5_H 
#endif

struct MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	uint8_t in[64];
};

/* Add this "namespace" prefix to work around an extraneous symbol in the libSystem.B
   library on MacOS 10.6 */
#define MD5Init _Samba_MD5Init
#define MD5Update _Samba_MD5Update
#define MD5Final _Samba_MD5Final

void _Samba_MD5Init(struct MD5Context *context);
void _Samba_MD5Update(struct MD5Context *context, const uint8_t *buf,
	       size_t len);
void _Samba_MD5Final(uint8_t digest[16], struct MD5Context *context);

#endif /* !MD5_H */
