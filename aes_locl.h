#ifndef __AES_LOCL_H
#define __AES_LOCL_H

#define MUL_BY_X(t) \
	if ((t) & 0x80) (t) = ((t) << 1) ^ 0x1B; \
	else (t) = (t) << 1;

/**
 *
 */
extern void aes_strcpy(unsigned char *dst, const unsigned char *src, int n);

/**
 *
 */
extern void sub_word(unsigned char *word);

/**
 *
 */
extern void aes_encrypt(unsigned char *out, const unsigned char *in,
	const unsigned char *ekey, int ekey_bytes);

/**
 *
 */
extern void aes_decrypt(unsigned char *out, const unsigned char *in,
	const unsigned char *ekey, int ekey_bytes);

#endif
