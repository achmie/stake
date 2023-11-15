#include "crypto.h"
#include <stdlib.h>

Digit ecc_tmp[ECC_TMP_DIGITS];

static void rng_bits(Digit *dst, int n)
{
	while (n--) {
		*dst++ = ((Digit)rand() << 24) ^ ((Digit)rand() << 16) ^
			((Digit)rand() << 8) ^ (Digit)rand();
	}
}

void ecc_generate_key(Digit *public_key, Digit *private_key, void (*rng)(Digit *, int))
{
	/*
	 * Generation of integer which is greather than 1 and
	 * less than ec generator order.
	 */
	do {
		/* Generate random number - private key. */
		if (rng) {
			rng(private_key, EC_GEN_ORDER_DIGITS);
		} else {
			rng_bits(private_key, EC_GEN_ORDER_DIGITS);
		}
		/* Reduction modulo ec generator order. */
		EC_GEN_ORDER_MODRED(private_key, EC_GEN_ORDER_DIGITS);
	}
	while (cmp_digit(private_key, 2, EC_GEN_ORDER_DIGITS) < 0);

	/* Assign generator. */
	FP_ASSIGN(X(ecc_tmp), X(EC_GEN));
	FP_ASSIGN(Y(ecc_tmp), Y(EC_GEN));

	/* Compute public key. */
	ecp_multiple(ecc_tmp, private_key);

	FP_ASSIGN(X(public_key), X(ecc_tmp));
	FP_ASSIGN(Y(public_key), Y(ecc_tmp));
}

void ecc_ecdsa_sign(EcdsaSign *signature, const Octet *digest, int digest_octets, const Digit *private_key)
{
	Digit *t = arth_tmp;
	Digit *k = ecc_tmp;
	Digit *r = k + EC_GEN_ORDER_DIGITS;
	Digit *s = r + EC_GEN_ORDER_DIGITS;
	Digit *e = s + EC_GEN_ORDER_DIGITS;

	int i;
	int digest_words;

	/* Do this sequence until integer s != 0. */
	do {
		/* Do this sequence until integer r != 0. */
		do {
			/* Random generation of integer k != 0. */
			do {
				/* Generate random number. */
				rng_bits(k, EC_GEN_ORDER_DIGITS);
				/* Reduction of integer k modulo ec generator order. */
				EC_GEN_ORDER_MODRED(k, EC_GEN_ORDER_DIGITS);
			}
			while (cmp_digit(k, 0, EC_GEN_ORDER_DIGITS) == 0);

			/* Assign generator to r. */
            FP_ASSIGN(X(r), X(EC_GEN));
            FP_ASSIGN(Y(r), Y(EC_GEN));

			/* Compute [k]G, and write X([k]G) to r. */
			ecp_multiple(r, k);

			/* Add lead zeros to the table representing r. */
			assign_digit(r + FP_DIGITS, 0, EC_GEN_ORDER_DIGITS -
				FP_DIGITS);

			/* Reduction of integer r modulo ec generator order. */
			EC_GEN_ORDER_MODRED(r, EC_GEN_ORDER_DIGITS);
		}
		while (cmp_digit(r, 0, EC_GEN_ORDER_DIGITS) == 0);

        /* Convert digest to integer. */
        digest_words = (digest_octets * OCTET_BITS) / WORD_BITS;
        if (EC_GEN_ORDER_DIGITS <= digest_words)
        	digest_words = EC_GEN_ORDER_DIGITS;

        assign_digit(e, 0, EC_GEN_ORDER_DIGITS);
        assign(e, (const Digit *)digest, digest_words);

		/* Compute k <- k^(-1) modulo ec generator order. */
		primeinv(k, k, EC_GEN_ORDER, EC_GEN_ORDER_DIGITS);
		/* Compute t <- r*pk modulo ec generator order. */
		mul(t, r, private_key, EC_GEN_ORDER_DIGITS);
		EC_GEN_ORDER_MODRED(t, 2*EC_GEN_ORDER_DIGITS);
		/* Compute t <- t+e modulo ec generator order. */
		if ( add(t, e, EC_GEN_ORDER_DIGITS) )
			sub(t, EC_GEN_ORDER, EC_GEN_ORDER_DIGITS);
		/* Compute s <- t*k modulo ec generator order. */
		mul(s, t, k, EC_GEN_ORDER_DIGITS);
		EC_GEN_ORDER_MODRED(s, 2*EC_GEN_ORDER_DIGITS);
	}
	while (cmp_digit(s, 0, EC_GEN_ORDER_DIGITS) == 0);

	assign(signature->r, r, EC_GEN_ORDER_DIGITS);
	assign(signature->s, s, EC_GEN_ORDER_DIGITS);

}

int ecc_ecdsa_verify(const EcdsaSign *signature, const Octet *digest, int digest_octets, const Digit *public_key)
{
	Digit *t = arth_tmp;
	Digit *e = ecc_tmp;
	Digit *s = e + EC_GEN_ORDER_DIGITS;
	Digit *u1 = s + EC_GEN_ORDER_DIGITS;
	Digit *u2 = u1 + EC_GEN_ORDER_DIGITS;
	Digit *R = u2 + EC_GEN_ORDER_DIGITS;

	int i;
	int digest_words;

	/* Convert digest to integer. */
	digest_words = (digest_octets * OCTET_BITS) / WORD_BITS;
	if (EC_GEN_ORDER_DIGITS <= digest_words)
		digest_words = EC_GEN_ORDER_DIGITS;

	assign_digit(e, 0, EC_GEN_ORDER_DIGITS);
	assign(e, (const Digit *)digest, digest_words);

	/* Compute s <- s^(-1) modulo ec generator order. */
	primeinv(s, signature->s, EC_GEN_ORDER, EC_GEN_ORDER_DIGITS);
	/* Compute u1 <- e*s^(-1) modulo ec generator order. */
	mul(t, e, s, EC_GEN_ORDER_DIGITS);
	EC_GEN_ORDER_MODRED(t, 2*EC_GEN_ORDER_DIGITS);
	assign(u1, t, EC_GEN_ORDER_DIGITS);
	/* Compute u2 <- r*s^(-1) modulo ec generator order. */
	mul(t, signature->r, s, EC_GEN_ORDER_DIGITS);
	EC_GEN_ORDER_MODRED(t, 2*EC_GEN_ORDER_DIGITS);
	assign(u2, t, EC_GEN_ORDER_DIGITS);
	/* Assign generator to R. */
	FP_ASSIGN(X(R), X(EC_GEN));
	FP_ASSIGN(Y(R), Y(EC_GEN));
	/* Compute R <- [u1]G + [u2]P where P is public key. */
	ecp_scalar_product(R, u1, public_key, u2);

	if (cmp(X(R), signature->r, EC_GEN_ORDER_DIGITS) == 0) {
	    return 0;
	} else {
	    return 1;
	}
}

static int ecc_multiplication(Digit *P, const Digit *k)
{
	Digit *t1 = ecc_tmp;
	Digit *t2 = t1 + FP_DIGITS;
	Digit *t3 = t2 + FP_DIGITS;

	assign_digit(t3, 3, FP_DIGITS);
	FP_ASSIGN(t1, Y(EC_GEN));
	FP_SQR(t1);
	FP_ASSIGN(t2, Y(P));
	FP_SQR(t2);
	FP_SUB(t1, t2);
	FP_ASSIGN(t2, X(EC_GEN));
	FP_SQR(t2);
	FP_SUB(t2, t3);
	FP_MUL(t2, X(EC_GEN));
	FP_SUB(t1, t2);
	FP_ASSIGN(t2, X(P));
	FP_SQR(t2);
	FP_SUB(t2, t3);
	FP_MUL(t2, X(P));
	FP_ADD(t1, t2);

	if (!FP_IS_ZERO(t1))
		return 1;

	ecp_multiple(P, k);

	return 0;
}

int ecc_ecdh_shared_info(Octet *shared_info, Digit *P, const Digit *private_key)
{
	int err;

	err = ecc_multiplication(P, private_key);

	if (!err)
		FP_ASSIGN((Digit *)shared_info, X(P));

	return err;
}

int ecc_iotstake_init(ProtocolIoTStake *ctx, const Digit *prvA, const Digit *pubB, void (*rng)(Digit *, int))
{
	assign(ctx->prvKeyA, prvA, EC_GEN_ORDER_DIGITS);
	assign(ctx->pubKeyB, pubB, 2*FP_DIGITS);
	ecc_generate_key(ctx->ephPubKeyA, ctx->ephPrvKeyA, rng);
	return 0;
}

int ecc_iotstake_q1(ProtocolIoTStake *ctx, Digit *Q1A)
{
	int err;

	assign(ctx->Q1, ctx->pubKeyB, 2*FP_DIGITS);
	err = ecc_multiplication(ctx->Q1, ctx->ephPrvKeyA);

	if (err) {
		return err;
	} else if (Q1A) {
		assign(Q1A, ctx->Q1, 2*FP_DIGITS);
	}

	return 0;
}

int ecc_iotstake_q2(ProtocolIoTStake *ctx, const Digit *Q1B, Digit *Q2B)
{
	int err;

	assign(ctx->Q2, Q1B, 2*FP_DIGITS);
	err = ecc_multiplication(ctx->Q2, ctx->ephPrvKeyA);

	if (err) {
		return err;
	} else if (Q2B) {
		assign(Q2B, ctx->Q2, 2*FP_DIGITS);
	}

	return 0;
}

int ecc_iotstake_q3(ProtocolIoTStake *ctx, const Digit *Q2A)
{
	int err;

	assign(ctx->Q3, Q2A, 2*FP_DIGITS);
	err = ecc_multiplication(ctx->Q3, ctx->prvKeyA);

	return err;
}

int ecc_iotstake_hash(ProtocolIoTStake *ctx, Octet *hash)
{
	Octet ekey[AES128_EKEY_BYTES];
	int i;

	for (i = 0; i < 16; i++) {
		ctx->hash[i] = (ctx->Q3[i/4] >> (i % 4)) & 0xFF;
	}

	aes128_key_expansion(ekey, ctx->hash);

	for (i = 0; i < 16; i++) {
		ctx->hash[i] = 0;
	}

	aes128_encrypt(ctx->hash, ctx->hash, ekey);

	if (hash) {
		for (i = 0; i < 16; i++) {
			hash[i] = ctx->hash[i];
		}
	}

	return 0;
}

int ecc_iotpki_init(ProtocolIoTPki *ctx, const Digit *prvA, const Digit *pubB, void (*rng)(Digit *, int))
{
	assign(ctx->prvKeyA, prvA, EC_GEN_ORDER_DIGITS);
	assign(ctx->pubKeyB, pubB, 2*FP_DIGITS);
	ecc_generate_key(ctx->ephPubKeyA, ctx->ephPrvKeyA, rng);
	return 0;
}

int ecc_iotpki_q1(ProtocolIoTPki *ctx, Digit *Q1A, EcdsaSign *signA)
{
	int err;

	assign(ctx->Q1, ctx->ephPubKeyA, 2*FP_DIGITS);
	assign(Q1A, ctx->Q1, 2*FP_DIGITS);
	ecc_ecdsa_sign(signA, (const Octet *)Q1A, FP_DIGITS*(WORD_BITS/OCTET_BITS), ctx->prvKeyA);

	return 0;
}

int ecc_iotpki_q2(ProtocolIoTPki *ctx, const Digit *Q1B, const EcdsaSign *signB)
{
	int err;

	assign(ctx->Q2, Q1B, 2*FP_DIGITS);

	if (ecc_multiplication(ctx->Q2, ctx->ephPrvKeyA) != 0) {
	    return 1;
	}

	if (ecc_ecdsa_verify(signB, (const Octet *)Q1B, FP_DIGITS*(WORD_BITS/OCTET_BITS), ctx->pubKeyB) != 0) {
		return 2;
	}

	return 0;
}

int ecc_iotpki_hash(ProtocolIoTPki *ctx, Octet *hash)
{
	Octet ekey[AES128_EKEY_BYTES];
	int i;

	for (i = 0; i < 16; i++) {
		ctx->hash[i] = (ctx->Q2[i/4] >> (i % 4)) & 0xFF;
	}

	aes128_key_expansion(ekey, ctx->hash);

	for (i = 0; i < 16; i++) {
		ctx->hash[i] = 0;
	}

	aes128_encrypt(ctx->hash, ctx->hash, ekey);

	if (hash) {
		for (i = 0; i < 16; i++) {
			hash[i] = ctx->hash[i];
		}
	}

	return 0;
}
