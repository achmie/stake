#include "crypto.h"

void ecp_pro2aff(Digit *P)
{
	FP_INV(Z(P));
	FP_MUL(Y(P), Z(P));
	FP_SQR(Z(P));
	FP_MUL(X(P), Z(P));
	FP_MUL(Y(P), Z(P));
	FP_ASSIGN_ONE(Z(P));
}

/* Algorithm works only for special case a = p - 3. */
void ecp_doubling(Digit *P)
{
	Digit *t1 = X(P);
	Digit *t2 = Y(P);
	Digit *t3 = Z(P);
	Digit *t4 = arth_tmp + 2*NUMBER_DIGITS_MAX;
	Digit *t5 = t4 + NUMBER_DIGITS_MAX;

	if ( FP_IS_ZERO(t2) || FP_IS_ZERO(t3) ) {
		FP_ASSIGN_ONE(X(P));
		FP_ASSIGN_ONE(Y(P));
		FP_ASSIGN_ZERO(Z(P));
		return;
	}

	FP_ASSIGN(t4, t3);
	FP_SQR(t4);
	FP_ASSIGN(t5, t1);
	FP_SUB(t5, t4);
	FP_ADD(t4, t1);
	FP_MUL(t5, t4);
	FP_ASSIGN(t4, t5);
	FP_ADD(t4, t4);
	FP_ADD(t4, t5);

	FP_MUL(t3, t2);
	FP_ADD(t3, t3);
	FP_SQR(t2);
	FP_ASSIGN(t5, t1);
	FP_MUL(t5, t2);
	FP_ADD(t5, t5);
	FP_ADD(t5, t5);
	FP_ASSIGN(t1, t4);
	FP_SQR(t1);
	FP_SUB(t1, t5);
	FP_SUB(t1, t5);
	FP_SQR(t2);
	FP_ADD(t2, t2);
	FP_ADD(t2, t2);
	FP_ADD(t2, t2);
	FP_SUB(t5, t1);
	FP_MUL(t5, t4);
	FP_SUB(t2, t5);
	FP_MINUS(t2);
}

void ecp_addition(Digit *P, const Digit *Q, int qsign)
{
	Digit *t1 = X(P);
	Digit *t2 = Y(P);
	Digit *t3 = Z(P);
	Digit *t4 = arth_tmp + 2*NUMBER_DIGITS_MAX;
	Digit *t5 = t4 + NUMBER_DIGITS_MAX;
	Digit *t7 = t5 + NUMBER_DIGITS_MAX;

	FP_ASSIGN(t4, X(Q));
	FP_ASSIGN(t5, Y(Q));

	if (qsign < 0)
		FP_MINUS(t5);

	if ( FP_IS_ZERO(t3) ) {
		FP_ASSIGN(t1, t4);
		FP_ASSIGN(t2, t5);
		FP_ASSIGN(t3, Z(Q));
		return;
	}

	if ( FP_IS_ZERO(Z(Q)) )
		return;

	if ( !FP_IS_ONE(Z(Q)) ) {
		FP_ASSIGN(t7, Z(Q));
		FP_SQR(t7);
		FP_MUL(t1, t7);
		FP_MUL(t7, Z(Q));
		FP_MUL(t2, t7);
	}

	FP_ASSIGN(t7, t3);
	FP_SQR(t7);
	FP_MUL(t4, t7);
	FP_MUL(t7, t3);
	FP_MUL(t5, t7);
	FP_SUB(t4, t1);
	FP_MINUS(t4);
	FP_SUB(t5, t2);
	FP_MINUS(t5);

	if ( FP_IS_ZERO(t4) ) {
		if ( FP_IS_ZERO(t5) ) {
			FP_ASSIGN(X(P), X(Q));
			FP_ASSIGN(Y(P), Y(Q));
			FP_ASSIGN(Z(P), Z(Q));

			if (qsign < 0)
				FP_MINUS(Y(P));

			ecp_doubling(P);
			return;
		} else {
			FP_ASSIGN_ONE(X(P));
			FP_ASSIGN_ONE(Y(P));
			FP_ASSIGN_ZERO(Z(P));
			return;
		}
	}

	FP_ADD(t1, t1);
	FP_SUB(t1, t4);
	FP_ADD(t2, t2);
	FP_SUB(t2, t5);

	if ( !FP_IS_ONE(Z(Q)) )
		FP_MUL(t3, Z(Q));

	FP_MUL(t3, t4);
	FP_ASSIGN(t7, t4);
	FP_SQR(t7);
	FP_MUL(t4, t7);
	FP_MUL(t7, t1);
	FP_ASSIGN(t1, t5);
	FP_SQR(t1);
	FP_SUB(t1, t7);
	FP_SUB(t7, t1);
	FP_SUB(t7, t1);
	FP_MUL(t5, t7);
	FP_MUL(t4, t2);
	FP_ASSIGN(t2, t5);
	FP_SUB(t2, t4);
	FP_MUL(t2, FP_INVOF2);
}

#include <iostream>

void ecp_multiple(Digit *P, const Digit *m)
{
	Digit T[3*FP_DIGITS];
	Digit TP[3*FP_DIGITS];

	int k = ARTH_GET_BIT(m, 0);
	int c = 0;
	int k_next;
	int c_next;
	int s;

	int i;
	
	int ops = 0;

	FP_ASSIGN_ONE(X(T));
	FP_ASSIGN_ONE(Y(T));
	FP_ASSIGN_ZERO(Z(T));

	FP_ASSIGN(X(TP), X(P));
	FP_ASSIGN(Y(TP), Y(P));
	FP_ASSIGN_ONE(Z(TP));

	for (i = 0; i < EC_GEN_ORDER_BITS + 1; i++) {
		if (i < EC_GEN_ORDER_BITS - 1) {
			k_next = ARTH_GET_BIT(m, i + 1);
		} else {
			k_next = 0;
		}

		c_next = (k + k_next + c) / 2;
		s = k + c - 2*c_next;
		k = k_next;
		c = c_next;

		if (s) {
			ecp_addition(T, TP, s);
			ops++;
		}

		ecp_doubling(TP);
		ops++;
	}

	ecp_pro2aff(T);
	assign(P, T, 2*FP_DIGITS);
}

void ecp_scalar_product(Digit *P, const Digit *mp, const Digit *Q, const Digit *mq)
{
	Digit T[3*FP_DIGITS];
	Digit TP[3*FP_DIGITS];
	Digit TQ[3*FP_DIGITS];
	Digit TPQ[3*FP_DIGITS];

	int mpi;
	int mqi;

	int i;
	
	int ops = 0;

	FP_ASSIGN_ONE(X(T));
	FP_ASSIGN_ONE(Y(T));
	FP_ASSIGN_ZERO(Z(T));

	FP_ASSIGN(X(TP), X(P));
	FP_ASSIGN(Y(TP), Y(P));
	FP_ASSIGN_ONE(Z(TP));

	FP_ASSIGN(X(TQ), X(Q));
	FP_ASSIGN(Y(TQ), Y(Q));
	FP_ASSIGN_ONE(Z(TQ));

	FP_ASSIGN(X(TPQ), X(P));
	FP_ASSIGN(Y(TPQ), Y(P));
	FP_ASSIGN_ONE(Z(TPQ));
	ecp_addition(TPQ, TQ, 1);

	for (i = EC_GEN_ORDER_BITS - 1; i >= 0; i--) {
		ecp_doubling(T);
		ops++;

		mpi = ARTH_GET_BIT(mp, i);
		mqi = ARTH_GET_BIT(mq, i);

		if ((mpi == 1) && (mqi == 1)) {
			ecp_addition(T, TPQ, 1);
			ops++;
		} else if (mpi == 1) {
            ecp_addition(T, TP, 1);
            ops++;
		} else if (mqi == 1) {
            ecp_addition(T, TQ, 1);
            ops++;
		}
	}

	ecp_pro2aff(T);
	assign(P, T, 2*FP_DIGITS);
}
