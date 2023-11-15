#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <stdint.h>

/**
 * \defgroup types_group Types and constants
 * \brief Definition of library types and constants
 * \{
 */

/**
 * \name Basic types and constants
 * \{
 */

/** \brief Definition of void type. */
typedef void Void;

/** \brief Number of bits in single \ref Octet. */
#define OCTET_BITS 8
/** \brief Definition of 8-bit type. */
typedef uint8_t Octet;

/** \brief Number of bits in single \ref Word. */
#define WORD_BITS 32
/** \brief Definition of type which represents single word. */
typedef uint32_t Word;
/** \} */

/**
 * \name Arithmetic types and constants
 * \{
 */

/** \brief Number of bits in single \ref Digit. */
#define DIGIT_BITS 32
/** \brief Definition of type which represents single digit. */
typedef uint32_t Digit;

/** \brief Number of bits in single \ref DDigit. */
#define DDIGIT_BITS 64
/** \brief Definition of type which contains two digits (\ref Digit) inside. */
typedef uint64_t DDigit;

/** \brief Definition of number comparison result. */
typedef int Cmp;

/** \} */


/** \} */



/**
 * \defgroup arth_group Arithmetic
 * \brief Natural numbers arithmetic operations.
 * \{
 */

/** \brief Minimum of two numbers. */
#define MIN(x,y) (x) < (y) ? (x) : (y)

/** \brief Maximum of two numbers. */
#define MAX(x,y) (x) < (y) ? (y) : (x)

/** \brief Simple lexems concatenation. */
#define GLUE(x,y) x##y

/** \brief Macro concatenation. */
#define XGLUE(x,y) GLUE(x,y)

#define FP_BITS 192
#define EC_GEN_ORDER_BITS 192
#define ECC_PARAMS_PREFIX secp192r1_

/** \brief Number of digits in representation of field element. */
#define FP_DIGITS ((FP_BITS + DIGIT_BITS - 1) / DIGIT_BITS)
/** \brief Number of digits in elliptic curve generator representation. */
#define EC_GEN_ORDER_DIGITS ((EC_GEN_ORDER_BITS + DIGIT_BITS - 1) / DIGIT_BITS)
/** \brief Macro which constructs names of adequate parameters and functions. */
#define ECC_PARAMS_SET(param) XGLUE(ECC_PARAMS_PREFIX, param)


/** \brief Macro returns specified bit of number. */
#define ARTH_GET_BIT(src, bit) \
	(((src)[(bit) / DIGIT_BITS] >> ((bit) % DIGIT_BITS)) & 1)

/** \brief Macro sets specified bit of number to 1. */
#define ARTH_SET_BIT(dst, bit) \
	((dst)[(bit) / DIGIT_BITS] |= (Digit)(1 << ((bit) % DIGIT_BITS)))

/** \brief Macro clears specified bit of number (sets to 0). */
#define ARTH_CLR_BIT(dst, bit) \
	((dst)[(bit) / DIGIT_BITS] &= ~(Digit)(1 << ((bit) % DIGIT_BITS)))

/** \brief Maximum number of digits in numbers. */
#define NUMBER_DIGITS_MAX (EC_GEN_ORDER_DIGITS + 1)

/** \brief Number of digits in arithmetic module shared memory. */
#define ARTH_TMP_DIGITS (6*NUMBER_DIGITS_MAX)

/** \brief Shared memory in arithmetic module. */
extern Digit arth_tmp[ARTH_TMP_DIGITS];

/**
 * \brief Assigns one number to another.
 *
 * \param[out] dst -
 *   destination number.
 * \param[in] src -
 *   number which is assigning.
 * \param[in] n -
 *   digits both of \a src and \a dst.
 */
extern void assign(Digit *dst, const Digit *src, int n);

/**
 * \brief Assigns single digit to number.
 *
 * \param[out] dst -
 *   destination number.
 * \param[in] src -
 *   digit which is assigning to \a dst number.
 * \param[in] n -
 *   digits of \a dst number.
 */
extern void assign_digit(Digit *dst, Digit src, int n);

/**
 * \brief Changes order of digits.
 *
 * \param[in,out] dst -
 *   table of digits which will be reordered.
 * \param[in] n -
 *   digits of \a dst.
 */
extern void reorder(Digit *dst, int n);

/**
 * \brief Comparison of two numbers.
 *
 * \param[in] op1 -
 *   first number which will be compared.
 * \param[in] op2 -
 *   second number which will be compared.
 * \param[in] n -
 *   digits of both of \a op1 and \a op2.
 *
 * \return
 *   - -1 if \a op1 < \a op2,
 *   - 0 if \a op1 = \a op2,
 *   - 1 if \a op1 > \a op2.
 */
extern Cmp cmp(const Digit *op1, const Digit *op2, int n);

/**
 * \brief Comparison of number and single digit.
 *
 * \param[in] op1 -
 *   number which will be compared.
 * \param[in] op2 -
 *   digit which will be compared.
 * \param[in] n -
 *   digits of \a op1.
 *
 * \return
 *   - -1 if \a op1 < \a op2,
 *   - 0 if \a op1 = \a op2,
 *   - 1 if \a op1 > \a op2.
 */
extern Cmp cmp_digit(const Digit *op1, Digit op2, int n);

/**
 * \brief Addition of two numbers.
 *
 * \param[in,out] dst -
 *   first component where sum will be stored.
 * \param[in] src -
 *   second component.
 * \param[in] n -
 *   digits both of \a dst and \a src.
 *
 * \return Carry which is efect of this operation.
 */
extern Digit add(Digit *dst, const Digit *src, int n);

/**
 * \brief Addition of number and digit.
 *
 * \param[in,out] dst -
 *   first component where sum will be stored.
 * \param[in] src -
 *   digit which will be add to \a dst.
 * \param[in] n -
 *   digits of \a dst.
 *
 * \return Carry which is efect of this operation.
 */
extern Digit add_digit(Digit *dst, Digit src, int n);

/**
 * \brief Subtraction of two numbers.
 *
 * \param[in,out] dst -
 *   first number where difference will be stored.
 * \param[in] src -
 *   second number.
 * \param[in] n -
 *   digits both of \a dst and \a src.
 *
 * \return Borrow which is efect of this operation.
 */
extern Digit sub(Digit *dst, const Digit *src, int n);

/**
 * \brief Subtraction of number and digit.
 *
 * \param[in,out] dst -
 *   first number where difference will be stored.
 * \param[in] src -
 *   digit which will be subtract from \a dst.
 * \param[in] n -
 *   digits of \a dst.
 *
 * \return Borrow which is efect of this operation.
 */
extern Digit sub_digit(Digit *dst, Digit src, int n);

/**
 * \brief Division by two.
 *
 * \param[in,out] dst -
 *   number which will be divided by 2 and where result will
 *   be stored.
 * \param[in] n -
 *   digits of \a dst.
 */
extern void div2(Digit *dst, int n);

/**
 * \brief Signed division by two.
 *
 * \param[in,out] dst -
 *   number which will be divided by 2 and where result will
 *   be stored.
 * \param[in] n -
 *   digits of \a dst.
 */
extern void signed_div2(Digit *dst, int n);

/**
 * \brief Add single digit multiple.
 *
 * \param[in,out] dst -
 *   first component and number where sum of \a dst +
 *   \a factor * \a src will be stored.
 * \param[in] src -
 *   number which is first factor.
 * \param[in] factor -
 *   digit wich is second factor.
 * \param[in] n -
 *   digits both of \a dst and \a src.
 *
 * \return Carry which is efect of this operation.
 */
extern Digit add_mul_digit(Digit *dst, const Digit *src,
	Digit factor, int n);

/**
 * \brief Subtract single digit multiple.
 *
 * \param[in,out] dst -
 *   first number and place where difference of \a dst and
 *   \a factor * \a src will be stored.
 * \param[in] src -
 *   number which is first factor.
 * \param[in] factor -
 *   digit wich is second factor.
 * \param[in] n -
 *   digits both of \a dst, \a src and \a factor.
 *
 * \return Borrow which is efect of this operation.
 */
extern Digit sub_mul_digit(Digit *dst, const Digit *src,
	Digit factor, int n);

/**
 * \brief Multiplication of two numbers.
 *
 * \param[out] dst -
 *   number where result will be stored.
 * \param[in] op1 -
 *   first factor.
 * \param[in] op2 -
 *   second factor.
 * \param[in] n -
 *   digits both of \a op1 and \a op2.
 */
extern void mul(Digit *dst, const Digit *op1,
	const Digit *op2, int n);

/**
 * \brief Inversion modulo odd prime number.
 *
 * \param[out] dst -
 *   number where inversion of \a src will be stored.
 * \param[in] src -
 *   number smaller than \a oddprime which will be inverted.
 * \param[in] oddprime -
 *   odd prime number.
 * \param[in] n -
 *   digits both of \a src and \a oddprime.
 */
extern void primeinv(Digit *dst, const Digit *src,
	const Digit *oddprime, int n);

/** \} */



/**
 * \defgroup eccparams_group Elliptic curve domain parameters
 *
 * This module contains all elliptic curve domain parameters. This
 * module defines universal macro interface to access dedicated functions
 * and data.
 *
 * \{
 */


/** \brief Elliptic curve generator. */
#define EC_GEN ECC_PARAMS_SET(gen)
/** \brief Order of elliptic curve generator. */
#define EC_GEN_ORDER ECC_PARAMS_SET(gen_order)
/** \brief Reduction modulo generator order. */
#define EC_GEN_ORDER_MODRED ECC_PARAMS_SET(gen_order_modred)
/** \brief Prime number which define field. */
#define FP_PRIME ECC_PARAMS_SET(prime)
/** \brief Inversion of 2 modulo \ref FP_PRIME. */
#define FP_INVOF2 ECC_PARAMS_SET(invof2)
/** \brief Assign field elements. */
#define FP_ASSIGN(dst, src) assign(dst, src, FP_DIGITS)
/** \brief Assign field element equel to 0. */
#define FP_ASSIGN_ZERO(dst) assign_digit(dst, 0, FP_DIGITS)
/** \brief Assign field element equel to 1. */
#define FP_ASSIGN_ONE(dst) assign_digit(dst, 1, FP_DIGITS)
/** \brief Check if field element is equel to 0. */
#define FP_IS_ZERO(dst) (cmp_digit(dst, 0, FP_DIGITS) == 0)
/** \brief Check if field element is equel to 1. */
#define FP_IS_ONE(dst) (cmp_digit(dst, 1, FP_DIGITS) == 0)
/** \brief Add two field elements and store result in first one. */
#define FP_ADD ECC_PARAMS_SET(fp_add)
/** \brief Subtract two field elements and store result in first one. */
#define FP_SUB ECC_PARAMS_SET(fp_sub)
/** \brief Multiply two field elements and store result in first one. */
#define FP_MUL ECC_PARAMS_SET(fp_mul)
/** \brief Compute square of field element and store result in argument. */
#define FP_SQR ECC_PARAMS_SET(fp_sqr)
/** \brief Compute reverse of field element and store result in argument. */
#define FP_MINUS ECC_PARAMS_SET(fp_minus)
/** \brief Compute inverse of field element and store result in argument. */
#define FP_INV(dst) primeinv(dst, dst, FP_PRIME, FP_DIGITS)

/**
 * \defgroup secp192r1_group SECP192R1
 * \brief Domain identified in SEC-2 as secp192r1.
 *
 * The verifiably random elliptic curve domain parameters over F(\a p)
 * secp192r1 are specified by the sextuple \a T = (\a p; \a a; \a b;
 * \a G; \a n; \a h) is defined by:
 * \code
 *   p = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFF
 *
 *   a = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFC
 *
 *   b = 64210519 E59C80E7 0FA7E9AB 72243049 FEB8DEEC C146B9B1
 *
 *   G = 04 188DA80E B03090F6 7CBF20EB 43A18800 F4FF0AFD 82FF1012
 *          07192B95 FFC8DA78 631011ED 6B24CDD5 73F977A1 1E794811
 *
 *   n = FFFFFFFF FFFFFFFF FFFFFFFF 99DEF836 146BC9B1 B4D22831
 *
 *   h = 01
 * \endcode
 *
 * This domain parameters are defined by the following object identifier:
 * \code
 *   ansi-X9-62 OBJECT IDENTIFIER ::= {
 *       iso(1) member-body(2) us(840) 10045
 *   }
 *
 *   secp192r1 OBJECT IDENTIFIER ::= {
 *       ansi-X9-62 curves(3) prime(1) 1
 *   }
 * \endcode
 *
 * \{
 */

/** \brief Prime which define field. */
extern const Digit secp192r1_prime[FP_DIGITS];
/** \brief Inversion of 2 modulo \ref secp192r1_prime. */
extern const Digit secp192r1_invof2[FP_DIGITS];
/** \brief Elliptic curve generator. */
extern const Digit secp192r1_gen[2*FP_DIGITS];
/** \brief Order of elliptic curve generator. */
extern const Digit secp192r1_gen_order[EC_GEN_ORDER_DIGITS];

/**
 * \brief Field addition.
 *
 * Function adds two elements modulo prime \a p = \ref secp192r1_prime
 * and strore result (\a dst = \a dst + \a src mod \a p).
 *
 * \param[in,out] dst -
 *   first component and element, where sum is stored.
 * \param[in] src -
 *   second component.
 */
extern void secp192r1_fp_add(Digit *dst, const Digit *src);
/**
 * \brief Field subtraction.
 *
 * Function subtract two elements modulo prime \a p = \ref secp192r1_prime
 * and strore result (\a dst = \a dst - \a src mod \a p).
 *
 * \param[in,out] dst -
 *   first element and operand where difference is stored.
 * \param[in] src -
 *   second element.
 */
extern void secp192r1_fp_sub(Digit *dst, const Digit *src);
/**
 * \brief Field multiplication.
 *
 * Function multiply two elements modulo prime \a p = \ref secp192r1_prime
 * and strore result (\a dst = \a dst * \a src mod \a p).
 *
 * \param[in,out] dst -
 *   first factor and element, where product is stored.
 * \param[in] src -
 *   second factor.
 */
extern void secp192r1_fp_mul(Digit *dst, const Digit *src);
/**
 * \brief Field squareing.
 *
 * Function compute square modulo prime \a p = \ref secp192r1_prime
 * and strore result (\a dst = \a dst * \a dst mod \a p).
 *
 * \param[in,out] dst -
 *   squaring element, where result is stored.
 */
extern void secp192r1_fp_sqr(Digit *dst);
/**
 * \brief Field reverse.
 *
 * Function reverse modulo prime \a p = \ref secp192r1_prime and
 * strore result (\a dst = -\a dst mod \a p).
 *
 * \param[in,out] dst -
 *   squaring element, where result is stored.
 */
extern void secp192r1_fp_minus(Digit *dst);
/**
 * \brief Reduction modulo field prime \ref secp192r1_prime.
 *
 * Function compute reduction modulo prime \a p = \ref secp192r1_prime
 * and strore result (\a dst = \a dst mod \a p). This function
 * assumes that \a dst has 2 * \ref FP_DIGITS digits.
 *
 * \param[in,out] dst -
 *   reduced element, where result is stored.
 */
extern void secp192r1_fp_modred(Digit *dst);
/**
 * \brief Reduction modulo generator order \ref secp192r1_gen_order.
 *
 * Function compute reduction modulo prime \a r = \ref secp192r1_gen_order
 * and strore result (\a dst = \a dst mod \a r).
 *
 * \param[in,out] dst -
 *   reduced number, where result is stored.
 * \param[in] n -
 *   digits of \a dst.
 */
extern void secp192r1_gen_order_modred(Digit *dst, int n);

/** \} */

/** \} */



/**
 * \defgroup ecc_group Elliptic curve cryptography for RevA (ECC-RevA)
 * \brief Elliptic curve point operations and ECC primitives for RevA.
 * \{
 */

/** \brief Get X coordinate of elliptic curve point. */
#define X(point) (point)
/** \brief Get Y coordinate of elliptic curve point. */
#define Y(point) ((point) + FP_DIGITS)
/** \brief Get Z coordinate of elliptic curve point. */
#define Z(point) ((point) + 2*FP_DIGITS)

/** \brief Number of digits in elliptic curve module shared memory. */
#define ECC_TMP_DIGITS (6*EC_GEN_ORDER_DIGITS)

/** \brief Shared memory in elliptic curve module. */
extern Digit ecc_tmp[ECC_TMP_DIGITS];

typedef struct EcdsaSign_st {
    Digit r[EC_GEN_ORDER_DIGITS];
    Digit s[EC_GEN_ORDER_DIGITS];
} EcdsaSign;

/**
 * \brief Point conversion from projective to affine coordinates.
 *
 * \param[in,out] P -
 *   point which will be converted.
 */
extern void ecp_pro2aff(Digit *P);

/**
 * \brief Elliptic curve point addition (only projective coordinates).
 *
 * Function computes sum or difference of two elliptic curve points.
 * We have two possibilitis depends on \a qsign
 *   - if \a qsign < 0 then \a P <- \a P - \a Q,
 *   - else \a P <- \a P + \a Q.
 *
 * \param[in,out] P -
 *   elliptic curve point to which point \a Q will be added and
 *   in which result will be stored (in projective coordinates).
 * \param[in] Q -
 *   elliptic curve point adding to \a P (in projective coordinates).
 * \param[in] qsign -
 *   sign of point \a Q (may be 1 for +Q and -1 for -Q).
 */
extern void ecp_addition(Digit *P, const Digit *Q, int qsign);

/**
 * \brief Elliptic curve point doubling (only projective coordinates).
 *
 * Function computes [2]\a P and store result in \a P.
 *
 * \param[in,out] P -
 *   elliptic curve point which will be doubled and in which result
 *   of this operation will be stored (in projective coordinates).
 */
extern void ecp_doubling(Digit *P);

/**
 * \brief Elliptic curve point multiple (only affine coordinates).
 *
 * Function computes [\a m]\a P and store result in \a P.
 *
 * \param[in,out] P -
 *   point to multiply (in affine coordinates).
 * \param[in] m -
 *   multiple which will be computed. Number of digits for this
 *   number is constant and equal to \ref EC_GEN_ORDER_DIGITS.
 */
extern void ecp_multiple(Digit *P, const Digit *m);

/**
 * \brief Elliptic curve point scalar product (only affine coordinates).
 *
 * Function computes [\a mp]\a P + [\a mq]\a Q and store result in \a P.
 * 
 * \param[in,out] P -
 *   point to scalar product (in affine coordinates).
 * \param[in] mp -
 *   multiple of P which will be computed. Number of digits for this
 *   number is constant and equal to \ref EC_GEN_ORDER_DIGITS.
 * \param[in] Q -
 *   point to scalar product (in affine coordinates).
 * \param[in] mq -
 *   multiple of Q which will be computed. Number of digits for this
 *   number is constant and equal to \ref EC_GEN_ORDER_DIGITS.
 */
extern void ecp_scalar_product(Digit *P, const Digit *mp, const Digit *Q, const Digit *mq);

/**
 * \brief ECC key pair generation.
 *
 * Function generates key pair for elliptic curve domain parameters
 * specified by ECC_PARAMS.
 *
 * \param[out] public_key -
 *   buffer for public key (affine point of elliptic curve).
 * \param[out] private_key -
 *   buffer for private key (integer less than elliptic curve
 *   generator order).
 * \parameter[in] rng -
 *   random number generation function. If 0 pointer, then function
 *   uses rand().
 */
extern void ecc_generate_key(Digit *public_key, Digit *private_key, void (*rng)(Digit *, int));

/**
 * \brief Compute digital signature based on ECDSA scheme.
 * 
 * \param[out] signature -
 *   ECDSA signature structure.
 * \param[in] digest -
 *   buffer wif digest to sign.
 * \param[in] digest_octets -
 *   number of octets in \a digest buffer.
 * \param[in] private_key -
 *   user private key.
 */
extern void ecc_ecdsa_sign(EcdsaSign *signature, const Octet *digest, int digest_octets, const Digit *private_key);

/**
 * \brief Compute digital signature based on ECDSA scheme.
 * 
 * \param[in] signature -
 *   ECDSA signature structure.
 * \param[in] digest -
 *   buffer wif digest to sign.
 * \param[in] digest_octets -
 *   number of octets in \a digest buffer.
 * \param[in] private_key -
 *   user private key.
 * 
 * \return Signature verification result: 0 - signature correct, 1 - signature error.
 */
extern int ecc_ecdsa_verify(const EcdsaSign *signature, const Octet *digest, int digest_octets, const Digit *private_key);

/**
 * \brief Compute shared info in ECDH or ECIES scheme.
 *
 * \param[out] shared_info -
 *   buffer for shared info octets (always \ref FP_DIGITS octets).
 * \param[in] P -
 *   elliptic curve point using to generate shared info (public key).
 * \param[in] private_key -
 *   User key used to generate shared information.
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if point \a P is not on supported curve.
 */
extern int ecc_ecdh_shared_info(Octet *shared_info, Digit *P, const Digit *private_key);

/** \brief Data structure for IoT STAKE protocol. */
typedef struct ProtocolIoTStake_st {
	/** \brief User private key. */
	Digit prvKeyA[EC_GEN_ORDER_DIGITS];
	/** \brief Public key of the other side. */
	Digit pubKeyB[2*FP_DIGITS];
	/** \brief Session private key. */
	Digit ephPrvKeyA[EC_GEN_ORDER_DIGITS];
	/** \brief Session public key. */
	Digit ephPubKeyA[2*FP_DIGITS];
	/** \brief Point Q1 of protocol. */
	Digit Q1[2*FP_DIGITS];
	/** \brief Point Q2 of protocol. */
	Digit Q2[2*FP_DIGITS];
	/** \brief Point Q3 of protocol. */
	Digit Q3[2*FP_DIGITS];
	/** \brief Hash of point Q3. */
	Octet hash[16];
} ProtocolIoTStake;

/**
 * \brief IoT STAKE protocol initialization.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[in] prvA -
 *   user private key.
 * \param[in] pubB -
 *   public key of the other side.
 * \param[in] rng -
 *   pointer to function which generates random numbers (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotstake_init(ProtocolIoTStake *ctx, const Digit *prvA, const Digit *pubB, void (*rng)(Digit *, int));

/**
 * \brief IoT STAKE protocol determine point Q1.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[out] Q1A -
 *   user Q1 EC point (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotstake_q1(ProtocolIoTStake *ctx, Digit *Q1A);

/**
 * \brief IoT STAKE protocol determine point Q2.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[in] Q1B -
 *   Q1 point of the other side.
 * \param[out] Q2A -
 *   Q2 point of the other side (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotstake_q2(ProtocolIoTStake *ctx, const Digit *Q1B, Digit *Q2B);

/**
 * \brief IoT STAKE protocol determine point Q3.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[in] Q2A -
 *   user Q2 point received from the other side.
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotstake_q3(ProtocolIoTStake *ctx, const Digit *Q2A);

/**
 * \brief IoT STAKE protocol determine hash from common secret.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[out] hash -
 *   hash of the secret which can be used as a session key (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotstake_hash(ProtocolIoTStake *ctx, Octet *hash);

/** \brief Data structure for IoT PKI protocol. */
typedef struct ProtocolIoTPki_st {
	/** \brief User private ECDSA key. */
	Digit prvKeyA[EC_GEN_ORDER_DIGITS];
	/** \brief Public ECDSA key of the other side. */
	Digit pubKeyB[2*FP_DIGITS];
	/** \brief Session ECDH private key. */
	Digit ephPrvKeyA[EC_GEN_ORDER_DIGITS];
	/** \brief Session ECDH public key. */
	Digit ephPubKeyA[2*FP_DIGITS];
	/** \brief Point Q1 of protocol. */
	Digit Q1[2*FP_DIGITS];
	/** \brief Point Q2 of protocol. */
	Digit Q2[2*FP_DIGITS];
	/** \brief Hash of point Q3. */
	Octet hash[16];
} ProtocolIoTPki;

/**
 * \brief IoT PKI protocol initialization.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[in] prvA -
 *   user private key.
 * \param[in] pubB -
 *   public key of the other side.
 * \param[in] rng -
 *   pointer to function which generates random numbers (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotpki_init(ProtocolIoTPki *ctx, const Digit *prvA, const Digit *pubB, void (*rng)(Digit *, int));

/**
 * \brief IoT PKI protocol determine point Q1 and signature.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[out] Q1A -
 *   user Q1 EC point (may be 0).
 * \param[out] signA -
 *   user signature under X(Q1A).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotpki_q1(ProtocolIoTPki *ctx, Digit *Q1A, EcdsaSign *signA);

/**
 * \brief IoT PKI protocol determine point Q2.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[in] Q1B -
 *   Q1 point of the other side.
 * \param[in] signB -
 *   user signature under X(Q1B).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotpki_q2(ProtocolIoTPki *ctx, const Digit *Q1B, const EcdsaSign *signB);

/**
 * \brief IoT PKI protocol determine hash from common secret.
 *
 * \param[in,out] ctx -
 *   protocol context.
 * \param[out] hash -
 *   hash of the secret which can be used as a session key (may be 0).
 *
 * \return
 *   - \ref 0 - if everything is OK.
 *   - \ref 1 - if error.
 */
extern int ecc_iotpki_hash(ProtocolIoTPki *ctx, Octet *hash);

/** \} */



/**
 * \defgroup aes AES (Advanced Encryption Standard)
 * \brief AES interface
 * \{
 */

/** \brief Number of block bytes for AES. */
#define AES_BLOCK_BYTES 16

/**
 * \defgroup aes128 AES-128
 * \brief AES-128 interface for microcontrollers
 * \{
 */

/** \brief Number of key bytes for AES-128. */
#define AES128_KEY_BYTES 16
/** \brief Number of expanded key bytes for AES-128. */
#define AES128_EKEY_BYTES 176
/** \brief Number of block bytes for AES-128. */
#define AES128_BLOCK_BYTES AES_BLOCK_BYTES

/**
 * \brief Finding key expansion for AES-128.
 *
 * \param[out] ekey -
 *   table for expanded key. It has to be at most \ref AES128_EKEY_BYTES bytes long.
 * \param[in] key -
 *   table with \ref AES128_KEY_BYTES bytes of secret key.
 */
extern void aes128_key_expansion(Octet *ekey, const Octet *key);

/**
 * \brief Encrypt data using AES-128.
 *
 * \param[out] out -
 *   table for output block. It has to be at most \ref AES128_BLOCK_BYTES bytes long.
 * \param[in] in -
 *   table with input block. It has to be \ref AES128_BLOCK_BYTES bytes long.
 * \param[in] ekey -
 *   table with key expansion for AES-128.
 */
extern void aes128_encrypt(Octet *out, const Octet *in, const Octet *ekey);

/**
 * \brief Decrypt data using AES-128.
 *
 * \param[out] out -
 *   table for output block. It has to be at most \ref AES128_BLOCK_BYTES bytes long.
 * \param[in] in -
 *   table with input block. It has to be \ref AES128_BLOCK_BYTES bytes long.
 * \param[in] ekey -
 *   table with key expansion for AES-128.
 */
extern void aes128_decrypt(Octet *out, const Octet *in, const Octet *ekey);

/**
 * \brief Encrypt data using AES-128-CBC.
 *
 * \param[out] out -
 *   table for output data.
 * \param[in] in -
 *   table with input data.
 * \param[in] len -
 *   number of octets in \a in data buffer.
 * \param[in] iv -
 *   initialization vector.
 * \param[in] ekey -
 *   table with key expansion for AES-128.
 */
extern int aes128_cbc_encrypt(Octet *out, const Octet *in, int len, const Octet* iv, const Octet *ekey);

/**
 * \brief Decrypt data using AES-128-CBC.
 *
 * \param[out] out -
 *   table for output data.
 * \param[in] in -
 *   table with input data.
 * \param[in] len -
 *   number of octets in \a in data buffer.
 * \param[in] iv -
 *   initialization vector.
 * \param[in] ekey -
 *   table with key expansion for AES-128.
 */
extern int aes128_cbc_decrypt(Octet *out, const Octet *in, int len, const Octet* iv, const Octet *ekey);

/** \} */

/** \} */

#endif /* __CRYPTO_H */
