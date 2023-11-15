#include "crypto.h"
#include "aes_locl.h"

void aes128_key_expansion(Octet *ekey, const Octet *key){
	Octet rc = 0x01;
	Octet t;
	int i;

	aes_strcpy(ekey, key, AES128_KEY_BYTES);

	for (i = AES128_KEY_BYTES; i < AES128_EKEY_BYTES; i += 4) {
		ekey[i + 0] = ekey[i - 4];
		ekey[i + 1] = ekey[i - 3];
		ekey[i + 2] = ekey[i - 2];
		ekey[i + 3] = ekey[i - 1];

		if ((i & 0x0F) == 0) {
			sub_word(ekey + i);
			t = ekey[i + 0];
			ekey[i + 0] = ekey[i + 1] ^ rc;
			ekey[i + 1] = ekey[i + 2];
			ekey[i + 2] = ekey[i + 3];
			ekey[i + 3] = t;
			MUL_BY_X(rc);
		}

		ekey[i + 0] ^= ekey[i - 16];
		ekey[i + 1] ^= ekey[i - 15];
		ekey[i + 2] ^= ekey[i - 14];
		ekey[i + 3] ^= ekey[i - 13];
	}
}

void aes128_encrypt(Octet *out, const Octet *in, const Octet *ekey){
	aes_encrypt(out, in, ekey, AES128_EKEY_BYTES);
}

void aes128_decrypt(Octet *out, const Octet *in, const Octet *ekey){
	aes_decrypt(out, in, ekey, AES128_EKEY_BYTES);
}

static void aes128_xor_block(Octet *dst, const Octet *src){
	for (int i = 0; i < 16; i++) {
		dst[i] ^= src[i];
	}
}

static void aes128_mov_block(Octet *dst, const Octet *src){
	for (int i = 0; i < 16; i++) {
		dst[i] = src[i];
	}
}

int aes128_cbc_encrypt(Octet *out, const Octet *in, int len, const Octet* iv, const Octet *ekey){
	Octet buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int retLen = 0;

	aes128_xor_block(buf, iv);

	while (len >= 16) {
		aes128_xor_block(buf, in);
		in += 16;
		len -= 16;
		aes128_encrypt(buf, buf, ekey);
		aes128_mov_block(out, buf);
		out += 16;
		retLen += 16;
	}

	Octet pad = (Octet)(16 - len);

	for (int i = 0; i < len; i++) {
		buf[i] ^= in[i];
	}

	for (int i = len; i < 16; i++) {
		buf[i] ^= pad;
	}

	aes128_encrypt(buf, buf, ekey);
	aes128_mov_block(out, buf);
	retLen += 16;

	return retLen;
}

int aes128_cbc_decrypt(Octet *out, const Octet *in, int len, const Octet* iv, const Octet *ekey){
	Octet buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Octet tmp[16];
	int retLen = 0;

	aes128_xor_block(buf, iv);

	while (len > 16) {
		aes128_decrypt(tmp, in, ekey);
		aes128_xor_block(tmp, buf);
		aes128_mov_block(out, tmp);
		aes128_mov_block(buf, in);
		in += 16;
		len -= 16;
		out += 16;
		retLen += 16;
	}

	aes128_decrypt(tmp, in, ekey);
	aes128_xor_block(tmp, buf);

	for (int i = 0; i < 16-tmp[15]; i++) {
		out[i] = tmp[i];
	}

	return retLen + 16 - tmp[15];
}
