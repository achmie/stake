#include "crypto.h"

Digit arth_tmp[ARTH_TMP_DIGITS];

void assign(Digit *dst, const Digit *src, int n)
{
	while (n--)
		(*dst++) = (*src++);
}

void assign_digit(Digit *dst, Digit src, int n)
{
	if (n) {
		(*dst++) = src;

		while (--n)
			(*dst++) = 0;
	}
}

void reorder(Digit *dst, int n)
{
	Digit tmp;

	while (n > 1) {
		tmp = dst[0];
		dst[0] = dst[--n];
		dst[n--] = tmp;
		dst++;
	}
}

Cmp cmp(const Digit *op1, const Digit *op2, int n)
{
	Cmp comparison = 0;

	op1 += n;
	op2 += n;

	while (n--) {
		if ( *(--op1) != *(--op2) )
			break;
	}

	if (*op1 > *op2) comparison = 1;
	else if (*op1 < *op2) comparison = -1;

	return (comparison);
}

Cmp cmp_digit(const Digit *op1, Digit op2, int n)
{
	Cmp comparison = 0;

	op1 += n - 1;

	while (--n)
	{
		if ( (*op1--) != 0 )
			return (1);
	}

	if (*op1 > op2) comparison = 1;
	else if (*op1 < op2) comparison = -1;

	return (comparison);
}

Digit add(Digit *dst, const Digit *src, int n)
{
	DDigit carry = 0;

	while (n--)
	{
		carry = (DDigit)(*dst) + (*src++) + carry;
		*dst++ = (Digit)carry;
		carry >>= DIGIT_BITS;
	}

	return ((Digit)carry);
}

Digit add_digit(Digit *dst, Digit src, int n)
{
	Digit carry;

	carry = add(dst, &src, 1);

	while (--n) {
		*(++dst) += carry;

		if (*dst) {
			carry = 0;
			break;
		}
	}

	return (carry);
}

Digit sub(Digit *dst, const Digit *src, int n)
{
	DDigit borrow = 0;

	while (n--) {
		borrow = (DDigit)(*dst) - (*src++) - borrow;
		*dst++ = (Digit)borrow;
		borrow >>= DDIGIT_BITS - 1;
	}

	return ((Digit)borrow);
}

Digit sub_digit(Digit *dst, Digit src, int n)
{
	Digit borrow;

	borrow = sub(dst, &src, 1);

	while (--n) {
		*(++dst) -= borrow;

		if (*dst != (Digit)(-1)) {
			borrow = 0;
			break;
		}
	}

	return (borrow);
}

void div2(Digit *dst, int n)
{
	while (--n) {
		*dst = (*dst >> 1) ^ (*(dst + 1) << (DIGIT_BITS - 1));
		dst++;
	}

	*dst >>= 1;
}

void signed_div2(Digit *dst, int n)
{
	while (--n) {
		*dst = (*dst >> 1) ^ (*(dst + 1) << (DIGIT_BITS - 1));
		dst++;
	}

	if ( (*dst >> (DIGIT_BITS - 1)) ) {
		*dst = (*dst >> 1) ^ (1 << (DIGIT_BITS - 1));
	} else {
		*dst >>= 1;
	}
}

Digit add_mul_digit(Digit *dst, const Digit *src, Digit factor, int n)
{
	DDigit carry = 0;

	while (n--) {
		carry = (DDigit)(*dst) + (DDigit)(*src++) * factor + carry;
		*dst++ = (Digit)carry;
		carry >>= DIGIT_BITS;
	}

	return ((Digit)carry);
}

Digit sub_mul_digit(Digit *dst, const Digit *src, Digit factor, int n)
{
	DDigit carry = 0;
	DDigit borrow = 0;

	while (n--) {
		carry = (DDigit)(*src++) * factor + carry;
		borrow = (DDigit)(*dst) - (Digit)carry - borrow;
		*dst++ = (Digit)borrow;
		borrow >>= DDIGIT_BITS - 1;
		carry >>= DIGIT_BITS;
	}

	return ((Digit)(carry + borrow));
}

void mul(Digit *dst, const Digit *op1, const Digit *op2, int n)
{
	int i;

	assign_digit(dst, 0, n);

	for (i = 0; i < n; i++)
		dst[n + i] = add_mul_digit(dst + i, op1, op2[i], n);
}

void primeinv(Digit *dst, const Digit *src, const Digit *oddprime, int n)
{
#	define isodd(num) ((*(num) & 1) == 1)
#	define iseven(num) ((*(num) & 1) == 0)

	Digit *u1 = arth_tmp;
	Digit *u2 = u1 + NUMBER_DIGITS_MAX;
	Digit *u3 = u2 + NUMBER_DIGITS_MAX;
	Digit *v1 = u3 + NUMBER_DIGITS_MAX;
	Digit *v2 = v1 + NUMBER_DIGITS_MAX;
	Digit *v3 = v2 + NUMBER_DIGITS_MAX;
	Digit *t;

	int m = n + 1;

	assign_digit(u1, 1, m);
	assign_digit(u2, 0, m);
	assign(u3, src, n);
	assign_digit(v1, 0, m);
	assign_digit(v2, 1, m);
	assign(v3, oddprime, n);

	while ( cmp_digit(u3, 0, n) != 0 ) {
		while ( iseven(u3) ) {
			div2(u3, n);
			if ( isodd(u1) || isodd(u2) ) {
				u1[n] += add(u1, oddprime, n);
				u2[n] -= sub(u2, src, n);
			}
			signed_div2(u1, m);
			signed_div2(u2, m);
		}

		if ( cmp(u3, v3, n) < 0 ) {
			t = u1; u1 = v1; v1 = t;
			t = u2; u2 = v2; v2 = t;
			t = u3; u3 = v3; v3 = t;
		}

		sub(u1, v1, m);
		sub(u2, v2, m);
		sub(u3, v3, n);

		if ( (u1[n] >> (DIGIT_BITS - 1)) && (u2[n] || (cmp(u2, oddprime, n) >= 0)) ) {
			u1[n] += add(u1, oddprime, n);
			u2[n] -= sub(u2, src, n);
		} else if ( (u2[n] >> (DIGIT_BITS - 1)) && (u1[n] || (cmp(u1, oddprime, n) >= 0)) ) {
			u1[n] -= sub(u1, oddprime, n);
			u2[n] += add(u2, src, n);
		}
	}

	if ( v1[n] >> (DIGIT_BITS - 1) ) {
		assign(dst, oddprime, n);
		add(dst, v1, n);
	} else {
		assign(dst, v1, n);
	}
}
