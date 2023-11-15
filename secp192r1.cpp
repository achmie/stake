#include "crypto.h"

const Digit secp192r1_prime[FP_DIGITS] = {
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF
};

const Digit secp192r1_invof2[FP_DIGITS] = {
	0x00000000, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0x7FFFFFFF
};

const Digit secp192r1_gen[2*FP_DIGITS] = {
	/* X coordinate. */
	0x82FF1012,	0xF4FF0AFD,	0x43A18800,	0x7CBF20EB,
	0xB03090F6,	0x188DA80E,
	/* Y coordinate. */
	0x1E794811,	0x73F977A1,	0x6B24CDD5,	0x631011ED,
	0xFFC8DA78,	0x07192B95
};

const Digit secp192r1_gen_order[EC_GEN_ORDER_DIGITS] = {
	0xB4D22831,	0x146BC9B1,	0x99DEF836,	0xFFFFFFFF,
	0xFFFFFFFF,	0xFFFFFFFF
};

Void
secp192r1_fp_add(Digit *dst, const Digit *src)
{
	if ( add(dst, src, FP_DIGITS) ) {
		sub(dst, secp192r1_prime, FP_DIGITS);
	} else if ( cmp(dst, secp192r1_prime, FP_DIGITS) >= 0 ) {
		sub(dst, secp192r1_prime, FP_DIGITS);
	}
}

Void
secp192r1_fp_sub(Digit *dst, const Digit *src)
{
	if ( sub(dst, src, FP_DIGITS) )
		add(dst, secp192r1_prime, FP_DIGITS);
}

Void
secp192r1_fp_mul(Digit *dst, const Digit *src)
{
	mul(arth_tmp, dst, src, FP_DIGITS);
	secp192r1_fp_modred(arth_tmp);
	assign(dst, arth_tmp, FP_DIGITS);
}

Void
secp192r1_fp_sqr(Digit *dst)
{
	mul(arth_tmp, dst, dst, FP_DIGITS);
	secp192r1_fp_modred(arth_tmp);
	assign(dst, arth_tmp, FP_DIGITS);
}

Void
secp192r1_fp_minus(Digit *dst)
{
	if ( cmp_digit(dst, 0, FP_DIGITS) != 0 ) {
		assign(arth_tmp, secp192r1_prime, FP_DIGITS);
		sub(arth_tmp, dst, FP_DIGITS);
		assign(dst, arth_tmp, FP_DIGITS);
	}
}

Void
secp192r1_fp_modred(Digit *dst)
{
	Digit *high = dst + FP_DIGITS;
	Digit carry;

	carry = add(dst, high, FP_DIGITS);
	carry += add(dst + 2, high, FP_DIGITS - 2);

	high[0] = high[FP_DIGITS - 2];
	high[1] = high[FP_DIGITS - 1];
	high[2] = high[FP_DIGITS - 2];
	high[3] = high[FP_DIGITS - 1];
	high[4] = 0;
	high[5] = 0;

	carry += add(dst, high, FP_DIGITS);

	while (carry > 0)
		carry -= sub(dst, secp192r1_prime, FP_DIGITS);

	if ( cmp(dst, secp192r1_prime, FP_DIGITS) >= 0 )
		sub(dst, secp192r1_prime, FP_DIGITS);
}

Void
secp192r1_gen_order_modred(Digit *dst, int n)
{
	Digit *tmp = dst + n - EC_GEN_ORDER_DIGITS - 1;

	while (n > EC_GEN_ORDER_DIGITS) {
		n--;

		dst[n] -= sub_mul_digit(tmp, secp192r1_gen_order, dst[n],
			EC_GEN_ORDER_DIGITS);

		while (dst[n])
			dst[n] -= sub(tmp, secp192r1_gen_order, EC_GEN_ORDER_DIGITS);

		tmp--;
	}

	if ( (n == EC_GEN_ORDER_DIGITS)
			&& (cmp(tmp, secp192r1_gen_order, EC_GEN_ORDER_DIGITS) >= 0) )
	{
		sub(tmp, secp192r1_gen_order, EC_GEN_ORDER_DIGITS);
	}
}
