/*
ehbigint.c: slow Big Int library hopefully somewhat suitable for 8bit CPUs
Copyright (C) 2016 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/

#include "ehbigint.h"
#include "ehbigint-log.h"
#include "ehbigint-util.h"
#include "ehbigint-eba.h"

static void ehbi_unsafe_zero(struct ehbigint *bi);
static int ehbi_reset_bytes_used(struct ehbigint *bi);
static int ehbi_is_odd(const struct ehbigint *bi, int *err);

#if EHBI_DEBUG
static unsigned Ti = 0;
static unsigned Tdepth = 0;
static char TBuf0[80];
static char TBuf1[80];

#ifdef _GNU_SOURCE
#define EHBI_FUNC __PRETTY_FUNCTION__
#else
#if (__STDC_VERSION__ >= 199901L)
#define EHBI_FUNC __func__
#else
#define EHBI_FUNC NULL
#endif /* _GNU_SOURCE */
#endif /* __STDC_VERSION__ */

#define Trace_bi(bi) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	++Tdepth; \
	fprintf(stderr, "%s%s(", TBuf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		((bi)) ? (unsigned long)((bi)->bytes_len) : 0, \
		((bi)) ? (unsigned long)((bi)->bytes_used) : 0, \
		((bi)) ? (unsigned)((bi)->sign) : 0); \
	for (Ti = 0; Ti < (((bi))? (bi)->bytes_len : 0); ++Ti) { \
		fprintf(stderr, "%02X",  ((bi)->bytes) ? (bi)->bytes[Ti] : 0); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_l(bi, l) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	++Tdepth; \
	fprintf(stderr, "%s%s(", TBuf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		((bi)) ? (unsigned long)((bi)->bytes_len) : 0, \
		((bi)) ? (unsigned long)((bi)->bytes_used) : 0, \
		((bi)) ? (unsigned)((bi)->sign) : 0); \
	for (Ti = 0; Ti < (((bi))? (bi)->bytes_len : 0); ++Ti) { \
		fprintf(stderr, "%02X",  ((bi)->bytes) ? (bi)->bytes[Ti] : 0); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ",\n%s\t\t%ld)\n", TBuf1, l)

#define Trace_bi_bi(bi1, bi2) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	++Tdepth; \
	fprintf(stderr, "%s%s(", TBuf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (Ti = 0; Ti < (bi1)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (Ti = 0; Ti < (bi2)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi(bi1, bi2, bi3) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	++Tdepth; \
	fprintf(stderr, "%s%s(", TBuf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (Ti = 0; Ti < (bi1)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "%s\n\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (Ti = 0; Ti < (bi2)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi3{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi3)->bytes_len), \
		(unsigned long)((bi3)->bytes_used), \
		(unsigned)((bi3)->sign)); \
	for (Ti = 0; Ti < (bi3)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi3)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	++Tdepth; \
	fprintf(stderr, "%s%s(", TBuf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (Ti = 0; Ti < (bi1)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (Ti = 0; Ti < (bi2)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi3{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi3)->bytes_len), \
		(unsigned long)((bi3)->bytes_used), \
		(unsigned)((bi3)->sign)); \
	for (Ti = 0; Ti < (bi3)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi3)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi4{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		(unsigned long)((bi4)->bytes_len), \
		(unsigned long)((bi4)->bytes_used), \
		(unsigned)((bi4)->sign)); \
	for (Ti = 0; Ti < (bi4)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi4)->bytes[Ti]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_msg_s_bi(s, bi) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf1[Ti] = ' '; \
	} \
	TBuf1[Ti] = '\0'; \
	fprintf(stderr, "%s%s: {bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		TBuf1, \
		s, \
		(unsigned long)((bi)->bytes_len), \
		(unsigned long)((bi)->bytes_used), \
		(unsigned)((bi)->sign)); \
	for (Ti = 0; Ti < (bi)->bytes_len; ++Ti) { \
		fprintf(stderr, "%02X",  (bi)->bytes[Ti]); \
	} \
	fprintf(stderr, "}\n")

#define Return_i(val) \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	fprintf(stderr, "%sreturn (%d) (%s)\n", TBuf0, val, EHBI_FUNC); \
	--Tdepth; \
	return val

#define Return_void() \
	for (Ti= 0; Ti < Tdepth; ++Ti) { \
		TBuf0[Ti] = '-'; \
		TBuf1[Ti] = ' '; \
	} \
	TBuf0[Ti] = '\0'; \
	TBuf1[Ti] = '\0'; \
	fprintf(stderr, "%sreturn (%s)\n", TBuf0, EHBI_FUNC); \
	--Tdepth; \
	return

#else
#define Trace_bi(bi)
#define Trace_bi_l(bi, l)
#define Trace_bi_bi(bi1, bi2)
#define Trace_bi_bi_bi(bi1, bi2, bi3)
#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4)
#define Trace_msg_s_bi(s, bi)
#define Return_i(val) return val
#define Return_void() return
#endif

int ehbi_zero(struct ehbigint *bi)
{
	Trace_bi(bi);

	if (bi == NULL) {
		Ehbi_log_error0("Null struct");
		Return_i(EHBI_NULL_STRUCT);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	ehbi_unsafe_zero(bi);

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

int ehbi_set_l(struct ehbigint *bi, long val)
{
	int err;

	Trace_bi_l(bi, val);

	err = ehbi_zero(bi);
	if (err) {
		Return_i(err);
	}

	err = ehbi_inc_l(bi, val);

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_set(struct ehbigint *bi, const struct ehbigint *val)
{
	int err;
	size_t i;
	unsigned char byte;

	Trace_bi_bi(bi, val);

	/* TODO: add val safety checks */

	err = ehbi_zero(bi);
	if (err) {
		Return_i(err);
	}

	bi->sign = val->sign;
	bi->bytes_used = 0;
	for (i = 0; i < val->bytes_used; ++i) {
		if (bi->bytes_used >= bi->bytes_len) {
			ehbi_unsafe_zero(bi);
			Ehbi_log_error0("Result byte[] too small");
			Return_i(EHBI_BYTES_TOO_SMALL);
		}
		byte = val->bytes[val->bytes_len - 1 - i];
		bi->bytes[bi->bytes_len - 1 - i] = byte;
		++bi->bytes_used;
	}

	for (i = 0; i < (bi->bytes_len - bi->bytes_used); ++i) {
		bi->bytes[i] = 0x00;
	}

	/* TODO: enusre bytes used ? */

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

int ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t i, size;
	unsigned char a, b, c;
	const struct ehbigint *swp;
	struct ehbigint tmp;
	int err;

	Trace_bi_bi_bi(res, bi1, bi2);

	if (res == NULL || bi1 == NULL || bi2 == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (res->bytes == NULL || bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	/* adding zero */
	if (bi2->bytes_used == 1 && bi2->bytes[bi2->bytes_len - 1] == 0x00) {
		err = ehbi_set(res, bi1);
		Return_i(err);
	}

	/* adding to zero */
	if (bi1->bytes_used == 1 && bi1->bytes[bi1->bytes_len - 1] == 0x00) {
		err = ehbi_set(res, bi2);
		Return_i(err);
	}

	tmp.bytes = NULL;
	tmp.bytes_len = 0;
	if (bi1->sign != bi2->sign) {
		size = bi2->bytes_len;
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			err = EHBI_STACK_TOO_SMALL;
		}
		tmp.bytes_len = size;
		err = ehbi_set(&tmp, bi2);
		err = err || ehbi_negate(&tmp);
		err = err || ehbi_subtract(res, bi1, &tmp);
		ehbi_stack_free(tmp.bytes, size);
		if (err) {
			ehbi_unsafe_zero(res);
		}
		Return_i(err);
	}
	res->sign = bi1->sign;

	if (bi1->bytes_used < bi2->bytes_used) {
		swp = bi1;
		bi1 = bi2;
		bi2 = swp;
	}

	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= bi1->bytes_used; ++i) {
		a = bi1->bytes[bi1->bytes_len - i];
		b = (bi2->bytes_used < i) ? 0 : bi2->bytes[bi2->bytes_len - i];
		c = c + a + b;

		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small");
			Return_i(EHBI_BYTES_TOO_SMALL);
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
	}
	if (c) {
		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small for carry");
			Return_i(EHBI_BYTES_TOO_SMALL_FOR_CARRY);
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
		if (c == 0xFF) {
			if (res->bytes_used == res->bytes_len) {
				Ehbi_log_error0
				    ("Result byte[] too small for carry");
				Return_i(EHBI_BYTES_TOO_SMALL_FOR_CARRY);
			}
			res->bytes_used++;
		}
	}

	if ((res->bytes_used == 1) && (res->bytes[res->bytes_len - 1] == 0x00)) {
		res->sign = 0;
	}

	Trace_msg_s_bi("end", res);
	Return_i(EHBI_SUCCESS);
}

int ehbi_mul(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t size, i, j;
	int err;
	const struct ehbigint *t;
	unsigned int a, b, r;
	struct ehbigint tmp;

	Trace_bi_bi_bi(res, bi1, bi2);

	if (res == NULL || bi1 == NULL || bi2 == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (res->bytes == NULL || bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	err = 0;
	if (bi1->bytes_used < bi2->bytes_used) {
		t = bi1;
		bi1 = bi2;
		bi2 = t;
	}

	size = res->bytes_len;
	tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!tmp.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		goto ehbi_mul_end;
	}
	tmp.bytes_len = size;
	ehbi_unsafe_zero(&tmp);
	ehbi_unsafe_zero(res);

	for (i = 0; i < bi2->bytes_used; ++i) {
		for (j = 0; j < bi1->bytes_used; ++j) {
			a = bi2->bytes[(bi2->bytes_len - 1) - i];
			b = bi1->bytes[(bi1->bytes_len - 1) - j];
			r = (a * b);
			err = err || ehbi_set_l(&tmp, r);
			err = err || ehbi_bytes_shift_left(&tmp, i);
			err = err || ehbi_bytes_shift_left(&tmp, j);
			err = err || ehbi_inc(res, &tmp);
			if (err) {
				goto ehbi_mul_end;
			}
		}
	}

ehbi_mul_end:
	if (err) {
		ehbi_zero(res);
	} else {
		if (bi1->sign != bi2->sign) {
			res->sign = 1;
		}
	}
	if (tmp.bytes) {
		ehbi_stack_free(tmp.bytes, size);
	}

	Trace_msg_s_bi("end", res);
	Return_i(err);
}

int ehbi_div(struct ehbigint *quotient, struct ehbigint *remainder,
	     const struct ehbigint *numerator,
	     const struct ehbigint *denominator)
{
	int err;
	size_t i, size, num_idx;
	struct ehbigint s_abs_numer;
	struct ehbigint s_abs_denom;
	const struct ehbigint *abs_numer;
	const struct ehbigint *abs_denom;

	Trace_bi_bi_bi_bi(quotient, remainder, numerator, denominator);

	if (quotient == NULL || remainder == NULL || numerator == NULL
	    || denominator == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (quotient->bytes == NULL || remainder->bytes == NULL
	    || numerator->bytes == NULL || denominator->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if (remainder->bytes_len < numerator->bytes_used) {
		Ehbi_log_error2("byte[] too small;"
				" remainder->bytes_len < numerator->bytes_used"
				" (%lu < %lu)",
				(unsigned long)remainder->bytes_len,
				(unsigned long)numerator->bytes_used);
		Return_i(EHBI_BYTES_TOO_SMALL);
	}

	s_abs_numer.bytes = NULL;
	s_abs_denom.bytes = NULL;

	if (numerator->sign == 0) {
		abs_numer = numerator;
	} else {
		s_abs_numer.bytes_used = 0;
		s_abs_numer.bytes_len = 0;
		s_abs_numer.sign = 0;
		size = numerator->bytes_used;
		s_abs_numer.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!s_abs_numer.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			err = EHBI_STACK_TOO_SMALL;
			goto ehbi_div_end;
		}
		s_abs_numer.bytes_len = size;
		err = ehbi_set(&s_abs_numer, numerator);
		err = err || ehbi_negate(&s_abs_numer);
		if (err) {
			goto ehbi_div_end;
		}
		abs_numer = &s_abs_numer;
	}

	if (denominator->sign == 0) {
		abs_denom = denominator;
	} else {
		s_abs_denom.bytes_used = 0;
		s_abs_denom.bytes_len = 0;
		s_abs_denom.sign = 0;
		size = numerator->bytes_used;
		s_abs_denom.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!s_abs_denom.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			err = EHBI_STACK_TOO_SMALL;
			goto ehbi_div_end;
		}
		s_abs_denom.bytes_len = size;
		err = ehbi_set(&s_abs_denom, denominator);
		err = err || ehbi_negate(&s_abs_denom);
		if (err) {
			goto ehbi_div_end;
		}
		abs_denom = &s_abs_denom;
	}

	/* just early return if abs_denom is bigger than abs_numer */
	if (ehbi_greater_than(abs_denom, abs_numer, &err)) {
		ehbi_unsafe_zero(quotient);
		err = ehbi_set(remainder, abs_numer);
		goto ehbi_div_end;
	}
	if (err) {
		goto ehbi_div_end;
	}

	/* base 256 "long division" */
	ehbi_unsafe_zero(quotient);
	ehbi_unsafe_zero(remainder);

	if (ehbi_equals(abs_denom, quotient, &err)) {
		Ehbi_log_error0("denominator == 0");
		err = EHBI_DIVIDE_BY_ZERO;
		goto ehbi_div_end;
	}
	if (err) {
		goto ehbi_div_end;
	}

	num_idx = abs_numer->bytes_len - abs_numer->bytes_used;
	for (i = 0; i < abs_denom->bytes_used; ++i) {
		if ((remainder->bytes_used > 1)
		    || (remainder->bytes[remainder->bytes_len - 1] != 0x00)) {
			err = ehbi_bytes_shift_left(remainder, 1);
			if (err) {
				goto ehbi_div_end;
			}
		}
		ehbi_inc_l(remainder, abs_numer->bytes[num_idx++]);
	}
	if (ehbi_greater_than(abs_denom, remainder, &err)) {
		err = ehbi_bytes_shift_left(remainder, 1);
		if (err) {
			goto ehbi_div_end;
		}
		ehbi_inc_l(remainder, abs_numer->bytes[num_idx++]);
	}
	if (err) {
		goto ehbi_div_end;
	}

	i = 0;
	while (ehbi_greater_than(remainder, abs_denom, &err)
	       || ehbi_equals(remainder, abs_denom, &err)) {
		if (err) {
			goto ehbi_div_end;
		}
		err = ehbi_inc_l(quotient, 1);
		if (err) {
			goto ehbi_div_end;
		}
		err = ehbi_dec(remainder, abs_denom);
		if (err) {
			goto ehbi_div_end;
		}
		while (ehbi_less_than(remainder, abs_denom, &err)
		       && (num_idx < abs_numer->bytes_len)) {
			err = ehbi_bytes_shift_left(quotient, 1);
			if (err) {
				goto ehbi_div_end;
			}

			err = ehbi_bytes_shift_left(remainder, 1);
			if (err) {
				goto ehbi_div_end;
			}
			remainder->bytes[remainder->bytes_len - 1] =
			    abs_numer->bytes[num_idx++];
		}
		if (err) {
			goto ehbi_div_end;
		}
	}

ehbi_div_end:
	if (s_abs_denom.bytes) {
		ehbi_stack_free(s_abs_denom.bytes, s_abs_denom.bytes_len);
	}
	if (s_abs_numer.bytes) {
		ehbi_stack_free(s_abs_numer.bytes, s_abs_numer.bytes_len);
	}
	/* if error, let's not return garbage or 1/2 an answer */
	if (err) {
		ehbi_zero(quotient);
		ehbi_zero(remainder);
	} else {
		if (numerator->sign != denominator->sign) {
			quotient->sign = 1;
		}
	}
	Trace_msg_s_bi("end quotient", quotient);
	Trace_msg_s_bi("end remainder", remainder);
	Return_i(err);
}

int ehbi_exp_mod(struct ehbigint *result, const struct ehbigint *base,
		 const struct ehbigint *exponent,
		 const struct ehbigint *modulus)
{
	int err;
	size_t size;
	struct ehbigint zero, tmp1, tquot, texp, tbase;
	unsigned char zero_bytes[2];

	Trace_bi_bi_bi_bi(result, base, exponent, modulus);

	if (result == NULL || base == NULL || exponent == NULL
	    || modulus == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (result->bytes == NULL || base->bytes == NULL
	    || exponent->bytes == NULL || modulus->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	zero.bytes = zero_bytes;
	zero.bytes_len = 2;
	zero.bytes_used = 0;
	zero.sign = 0;
	ehbi_unsafe_zero(&zero);

	tmp1.bytes = NULL;
	tmp1.bytes_len = 0;
	tmp1.bytes_used = 0;
	tmp1.sign = 0;

	tbase.bytes = NULL;
	tbase.bytes_len = 0;
	tbase.bytes_used = 0;
	tbase.sign = 0;

	texp.bytes = NULL;
	texp.bytes_len = 0;
	texp.bytes_used = 0;
	texp.sign = 0;

	tquot.bytes = NULL;
	tquot.bytes_len = 0;
	tquot.bytes_used = 0;
	tquot.sign = 0;

	err = EHBI_SUCCESS;

	size = 2 + (2 * base->bytes_used) + exponent->bytes_used;

	tmp1.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!tmp1.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes",
				ehbi_stack_alloc_str, (unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		goto ehbi_mod_exp_end;
	}
	tmp1.bytes_len = size;

	tbase.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!tbase.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes",
				ehbi_stack_alloc_str, (unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		goto ehbi_mod_exp_end;
	}
	tbase.bytes_len = size;

	texp.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!texp.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes",
				ehbi_stack_alloc_str, (unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		goto ehbi_mod_exp_end;
	}
	texp.bytes_len = size;

	tquot.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!tquot.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes",
				ehbi_stack_alloc_str, (unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		goto ehbi_mod_exp_end;
	}
	tquot.bytes_len = size;

	/* prevent divide by zero */
	ehbi_unsafe_zero(&tmp1);
	if (ehbi_equals(modulus, &tmp1, &err)) {
		Ehbi_log_error0("modulus == 0");
		err = EHBI_DIVIDE_BY_ZERO;
		goto ehbi_mod_exp_end;
	}

	/* prevent negative eponent */
	if (ehbi_is_negative(exponent, &err)) {
		Ehbi_log_error0("exponent < 0");
		err = EHBI_BAD_DATA;
		goto ehbi_mod_exp_end;
	}

	/* if modulus == 1 then return 0 */
	err = ehbi_set_l(&tmp1, 1);
	if (!err && ehbi_equals(modulus, &tmp1, &err)) {
		ehbi_unsafe_zero(result);
		goto ehbi_mod_exp_end;
	}

	err = err || ehbi_set(&tbase, base);
	err = err || ehbi_set(&texp, exponent);
	err = err || ehbi_set_l(result, 1);
	if (err) {
		goto ehbi_mod_exp_end;
	}

	while (ehbi_greater_than(&texp, &zero, &err)) {
		if (err) {
			goto ehbi_mod_exp_end;
		}

		if (ehbi_is_odd(&texp, &err)) {
			err = err || ehbi_mul(&tmp1, result, &tbase);
			err = err || ehbi_div(&tquot, result, &tmp1, modulus);
		}
		err = err || ehbi_shift_right(&texp, 1);
		err = err || ehbi_mul(&tmp1, &tbase, &tbase);
		err = err || ehbi_div(&tquot, &tbase, &tmp1, modulus);

		if (err) {
			goto ehbi_mod_exp_end;
		}
	}

ehbi_mod_exp_end:
	if (tmp1.bytes) {
		ehbi_stack_free(tmp1.bytes, tmp1.bytes_len);
	}
	if (tbase.bytes) {
		ehbi_stack_free(tbase.bytes, tbase.bytes_len);
	}
	if (texp.bytes) {
		ehbi_stack_free(texp.bytes, texp.bytes_len);
	}
	if (tquot.bytes) {
		ehbi_stack_free(tquot.bytes, tmp1.bytes_len);
	}
	if (err) {
		ehbi_zero(result);
	}

	Trace_msg_s_bi("end", result);
	Return_i(err);
}

int ehbi_inc(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t size;
	int err;
	struct ehbigint temp;

	Trace_bi_bi(bi, val);

	if (bi == NULL || val == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL || val->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}
	if (val->bytes_used > bi->bytes_len) {
		Ehbi_log_error0("byte[] too small");
		Return_i(EHBI_BYTES_TOO_SMALL);
	}

	err = EHBI_SUCCESS;

	size = bi->bytes_used;

	temp.bytes_used = 0;
	temp.bytes_len = 0;
	temp.sign = 0;
	temp.bytes = (unsigned char *)ehbi_stack_alloc(size);
	if (!temp.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes",
				ehbi_stack_alloc_str, (unsigned long)size);
		err = EHBI_STACK_TOO_SMALL;
		Return_i(err);
	}
	temp.bytes_len = size;
	err = ehbi_set(&temp, bi);
	err = err || ehbi_add(bi, &temp, val);
	ehbi_stack_free(temp.bytes, temp.bytes_len);

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_inc_l(struct ehbigint *bi, long val)
{
	size_t i, j;
	unsigned long v;
	unsigned char c, val_negative;
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;
	int err;

	Trace_bi_l(bi, val);

	val_negative = (val < 0) ? 1 : 0;

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	temp.bytes_used = sizeof(unsigned long);
	temp.sign = 0;

	v = (val_negative) ? (unsigned long)(-val) : (unsigned long)val;

	for (i = 0; i < temp.bytes_used; ++i) {
		c = (v >> (8 * i));
		j = (temp.bytes_len - 1) - i;
		temp.bytes[j] = c;
	}
	for (i = 0; i < temp.bytes_len; ++i) {
		if (temp.bytes[i] != 0x00) {
			break;
		}
	}
	temp.bytes_used = temp.bytes_len - i;
	if (temp.bytes_used == 0) {
		++temp.bytes_used;
	}

	if (val_negative) {
		err = ehbi_dec(bi, &temp);
	} else {
		err = ehbi_inc(bi, &temp);
	}

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_dec(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i, j, size;
	unsigned char a, b, c;
	struct ehbigint tmp;
	int err;

	Trace_bi_bi(bi, val);

	if (bi == NULL || val == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL || val->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if (ehbi_is_negative(val, &err)) {
		size = val->bytes_used;
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			Return_i(EHBI_STACK_TOO_SMALL);
		}
		tmp.bytes_len = size;
		tmp.bytes_used = 0;
		tmp.sign = 0;
		err = err || ehbi_set(&tmp, val);
		err = err || ehbi_negate(&tmp);
		err = err || ehbi_inc(bi, &tmp);
		ehbi_stack_free(tmp.bytes, tmp.bytes_len);
		Return_i(err);
	}

	err = 0;
	if (ehbi_greater_than(val, bi, &err)) {
		size = val->bytes_used;
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			Return_i(EHBI_STACK_TOO_SMALL);
		}
		tmp.bytes_len = size;
		tmp.bytes_used = 0;
		tmp.sign = 0;
		err = ehbi_subtract(&tmp, val, bi);
		err = err || ehbi_set(bi, &tmp);
		ehbi_stack_free(tmp.bytes, tmp.bytes_len);
		err = err || ehbi_negate(bi);
		Return_i(err);
	}
	if (err) {
		Return_i(err);
	}

	c = 0;
	for (i = 1; i <= val->bytes_used; ++i) {
		a = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		b = val->bytes[val->bytes_len - i];
		c = a - b;

		bi->bytes[bi->bytes_len - i] = c;

		j = i;
		while (c > a) {
			if ((bi->bytes_len - j) == 0) {
				Ehbi_log_error0("byte[] too small for borrow");
				Return_i(EHBI_BYTES_TOO_SMALL_FOR_BORROW);
			}
			bi->bytes[bi->bytes_len - (j + 1)] -= 1;
			if (bi->bytes[bi->bytes_len - (j + 1)] == 0xFF) {
				c = 1;
				a = 0;
			} else {
				c = 0;
				a = 0;
			}
			++j;
		}
	}
	for (i = 0; i < bi->bytes_len; ++i) {
		if (bi->bytes[i] != 0x00) {
			break;
		}
	}
	bi->bytes_used = bi->bytes_len - i;
	if ((bi->bytes_used > 0) && (bi->bytes_used < bi->bytes_len)
	    && (bi->bytes[i] > 0x7F) && (i > 0) && (i <= bi->bytes_len)) {
		++bi->bytes_used;
	}
	if (bi->bytes_used == 0) {
		bi->bytes_used = 1;
	}

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

int ehbi_subtract(struct ehbigint *res, const struct ehbigint *bi1,
		  const struct ehbigint *bi2)
{
	size_t i, size;
	unsigned char a, b, c, negate;
	const struct ehbigint *swp;
	struct ehbigint abs;
	int err;

	Trace_bi_bi_bi(res, bi1, bi2);

	abs.bytes = NULL;
	abs.bytes_len = 0;
	abs.sign = 0;

	if (res == NULL || bi1 == NULL || bi2 == NULL) {
		Ehbi_log_error0("Null argument(s)");
		err = EHBI_NULL_ARGS;
		goto ehbi_subtract_end;
	}
	if (res->bytes == NULL || bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		err = EHBI_NULL_BYTES;
		goto ehbi_subtract_end;
	}

	/* subtract zero */
	if (bi2->bytes_used == 1 && bi2->bytes[bi2->bytes_len - 1] == 0x00) {
		err = ehbi_set(res, bi1);
		goto ehbi_subtract_end;
	}

	/* subtract from 0 */
	if (bi1->bytes_used == 1 && bi1->bytes[bi1->bytes_len - 1] == 0x00) {
		err = ehbi_set(res, bi2);
		err = err || ehbi_negate(res);
		goto ehbi_subtract_end;
	}

	if (bi1->sign == 0 && bi2->sign != 0) {
		size = bi2->bytes_len;
		abs.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!abs.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			err = EHBI_STACK_TOO_SMALL;
			goto ehbi_subtract_end;
		}
		abs.bytes_len = size;
		err = ehbi_set(&abs, bi2);
		err = err || ehbi_negate(&abs);
		err = err || ehbi_add(res, bi1, &abs);
		goto ehbi_subtract_end;
	}

	if (bi1->sign != 0 && bi2->sign == 0) {
		size = bi1->bytes_len;
		abs.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!abs.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			err = EHBI_STACK_TOO_SMALL;
			goto ehbi_subtract_end;
		}
		abs.bytes_len = size;
		err = ehbi_set(&abs, bi1);
		err = err || ehbi_negate(&abs);
		err = err || ehbi_add(res, &abs, bi2);
		err = err || ehbi_negate(res);
		goto ehbi_subtract_end;
	}

	if ((bi1->sign == 0 && bi2->sign == 0
	     && ehbi_greater_than(bi2, bi1, &err)) || (bi1->sign != 0
						       && bi2->sign != 0
						       && ehbi_less_than(bi2,
									 bi1,
									 &err)))
	{
		negate = 1;
		swp = bi1;
		bi1 = bi2;
		bi2 = swp;
	} else {
		negate = 0;
	}

	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= bi1->bytes_used; ++i) {
		if (bi1->bytes_used < i) {
			a = ((1 << 7) & bi1->bytes[0]) ? 0xFF : 0;
		} else {
			a = bi1->bytes[bi1->bytes_len - i];
		}
		if ((bi2->bytes_used < i) || (i > bi2->bytes_len)) {
			b = 0;
		} else {
			b = bi2->bytes[bi2->bytes_len - i];
		}
		c = c + (a - b);

		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small");
			Return_i(EHBI_BYTES_TOO_SMALL);
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c > a) ? 0xFF : 0;
	}

	if (c) {
		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small for carry");
			err = EHBI_BYTES_TOO_SMALL_FOR_CARRY;
			goto ehbi_subtract_end;
		}
		while (i <= res->bytes_len) {
			res->bytes[res->bytes_len - i] = 0xFF;
			++i;
		}
	}

	res->sign = (negate) ? !(bi1->sign) : bi1->sign;

	if ((res->bytes_used == 1) && (res->bytes[res->bytes_len - 1] == 0x00)) {
		res->sign = 0;
	}
	/* TODO: err = ehbi_reset_bytes_used(res); */

	err = err || EHBI_SUCCESS;

ehbi_subtract_end:
	if (err && res) {
		ehbi_zero(res);
	}
	if (abs.bytes) {
		ehbi_stack_free(abs.bytes, abs.bytes_len);
	}

	Trace_msg_s_bi("end", res);
	Return_i(EHBI_SUCCESS);
}

int ehbi_bytes_shift_left(struct ehbigint *bi, size_t num_bytes)
{
	size_t i;

	Trace_bi_l(bi, ((long)num_bytes));

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if (num_bytes == 0) {
		Trace_msg_s_bi("end", bi);
		Return_i(EHBI_SUCCESS);
	}

	/* by ensuring that we have at least one extra byte of freespace,
	 * we avoid some werid sign issues */
	if (bi->bytes_len < (bi->bytes_used + 1 + num_bytes)) {
		Ehbi_log_error3("Result byte[] too small for shift"
				" (bi->bytes_len <"
				" (bi->bytes_used + 1 + num_bytes))"
				" (%lu < (%lu + 1 + %lu))",
				(unsigned long)bi->bytes_len,
				(unsigned long)bi->bytes_used,
				(unsigned long)num_bytes);
		Return_i(EHBI_BYTES_TOO_SMALL_FOR_CARRY);
	}

	bi->bytes_used += num_bytes;

	/* shift the value left by num_bytes bytes */
	for (i = 0; i < bi->bytes_len; ++i) {
		/* shift the value byte one byte */
		if (i + num_bytes >= bi->bytes_len) {
			/* set the zero/-1 value on the right */
			bi->bytes[i] = bi->bytes[0];
		} else {
			bi->bytes[i] = bi->bytes[i + num_bytes];
		}
	}

	/* make sure we keep "bytes_used" reasonable */
	while (bi->bytes_used > 1
	       && bi->bytes[bi->bytes_len - bi->bytes_used] == bi->bytes[0]
	       && bi->bytes[bi->bytes_len - (bi->bytes_used - 1)] < 0x80) {
		--(bi->bytes_used);
	}

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

int ehbi_bytes_shift_right(struct ehbigint *bi, size_t num_bytes)
{
	size_t i;

	Trace_bi_l(bi, ((long)num_bytes));

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if (num_bytes == 0) {
		Trace_msg_s_bi("end", bi);
		Return_i(EHBI_SUCCESS);
	}

	if (bi->bytes_used <= num_bytes) {
		ehbi_unsafe_zero(bi);
		Trace_msg_s_bi("end", bi);
		Return_i(EHBI_SUCCESS);
	}

	/* shift the value left by num_bytes bytes */
	for (i = bi->bytes_len; i > 0; --i) {
		if (i > num_bytes) {
			bi->bytes[i - 1] = bi->bytes[i - (1 + num_bytes)];
		} else {
			bi->bytes[i - 1] = 0x00;
		}
	}
	bi->bytes_used -= num_bytes;

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

int ehbi_shift_right(struct ehbigint *bi, unsigned long num_bits)
{
	size_t bytes;
	int err;
	struct eba_s eba;

	Trace_bi_l(bi, ((long)num_bits));

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if ((num_bits % 8UL) == 0) {
		bytes = (size_t)(num_bits / 8UL);
		err = ehbi_bytes_shift_right(bi, bytes);

		Trace_msg_s_bi("end", bi);
		Return_i(err);
	}

	eba.endian = eba_big_endian;
	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	ehbi_eba_err = EHBI_SUCCESS;
	eba_shift_right(&eba, 1);
	err = ehbi_eba_err;

	err = err || ehbi_reset_bytes_used(bi);

	if (err) {
		ehbi_zero(bi);
	}

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_shift_left(struct ehbigint *bi, unsigned long num_bits)
{
	size_t bytes;
	int err;
	struct eba_s eba;

	Trace_bi_l(bi, ((long)num_bits));

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		Return_i(EHBI_NULL_ARGS);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	if ((num_bits % 8UL) == 0) {
		bytes = (size_t)(num_bits / 8UL);
		err = ehbi_bytes_shift_left(bi, bytes);

		Trace_msg_s_bi("end", bi);
		Return_i(err);
	}

	eba.endian = eba_big_endian;
	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	ehbi_eba_err = EHBI_SUCCESS;
	eba_shift_left(&eba, 1);
	err = ehbi_eba_err;

	err = err || ehbi_reset_bytes_used(bi);

	if (err) {
		ehbi_zero(bi);
	}

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_negate(struct ehbigint *bi)
{
	int err;

	Trace_bi(bi);

	if (bi == NULL) {
		Ehbi_log_error0("Null argument");
		Return_i(EHBI_NULL_ARGS);
	}

	bi->sign = (bi->sign == 0) ? 1 : 0;

	err = EHBI_SUCCESS;
	/* TODO: err = ehbi_reset_bytes_used(bi); ? */

	Trace_msg_s_bi("end", bi);
	Return_i(err);
}

int ehbi_is_negative(const struct ehbigint *bi, int *err)
{
	int rv;

	Trace_bi(bi);

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		Return_i(0);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		if (err) {
			*err = EHBI_NULL_BYTES;
		}
		Return_i(0);
	}

	/* guard for negative zero? */
	if (bi->bytes_used == 0) {
		Return_i(0);
	}
	if (bi->bytes_used == 1) {
		if (bi->bytes[bi->bytes_len - 1] == 0x00) {
			Return_i(0);
		}
	}

	rv = (bi->sign == 0) ? 0 : 1;
	Return_i(rv);
}

int ehbi_compare(const struct ehbigint *bi1, const struct ehbigint *bi2,
		 int *err)
{
	size_t i;
	unsigned char a, b;
	int rv, b1_pos, b2_pos;

	Trace_bi_bi(bi1, bi2);

	if (bi1 == NULL || bi2 == NULL || err == NULL) {
		Ehbi_log_error0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		Return_i(0);
	}
	if (bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		if (err) {
			*err = EHBI_NULL_BYTES;
		}
		Return_i(0);
	}

	*err = EHBI_SUCCESS;

	b1_pos = !ehbi_is_negative(bi1, err);
	b2_pos = !ehbi_is_negative(bi2, err);

	if (b1_pos != b2_pos) {
		rv = b1_pos ? 1 : -1;
		Return_i(rv);
	}

	if (bi1->bytes_used > bi2->bytes_used) {
		rv = b1_pos ? 1 : -1;
		Return_i(rv);
	} else if (bi1->bytes_used < bi2->bytes_used) {
		rv = b1_pos ? -1 : 1;
		Return_i(rv);
	}

	for (i = 0; i < bi1->bytes_used; ++i) {
		a = bi1->bytes[(bi1->bytes_len - bi1->bytes_used) + i];
		b = bi2->bytes[(bi2->bytes_len - bi2->bytes_used) + i];
		if (a > b) {
			rv = b1_pos ? 1 : -1;
			Return_i(rv);
		} else if (a < b) {
			rv = b1_pos ? -1 : 1;
			Return_i(rv);
		}
	}

	rv = 0;
	Return_i(rv);
}

int ehbi_equals(const struct ehbigint *bi1, const struct ehbigint *bi2,
		int *err)
{
	int rv, terr;

	Trace_bi_bi(bi1, bi2);

	err = (err != NULL) ? err : &terr;
	rv = ((ehbi_compare(bi1, bi2, err) == 0) && (*err == EHBI_SUCCESS));

	Return_i(rv);
}

int ehbi_less_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		   int *err)
{
	int rv, terr;

	Trace_bi_bi(bi1, bi2);

	err = (err != NULL) ? err : &terr;
	rv = ((ehbi_compare(bi1, bi2, err) < 0) && (*err == EHBI_SUCCESS));

	Return_i(rv);
}

int ehbi_greater_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		      int *err)
{
	int rv, terr;

	Trace_bi_bi(bi1, bi2);

	err = (err != NULL) ? err : &terr;
	rv = ((ehbi_compare(bi1, bi2, err) > 0) && (*err == EHBI_SUCCESS));

	Return_i(rv);
}

static int ehbi_is_odd(const struct ehbigint *bi, int *err)
{
	unsigned char bit;

	Trace_bi(bi);

	if (bi == NULL) {
		Ehbi_log_error0("Null struct");
		*err = EHBI_NULL_STRUCT;
		Return_i(0);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		*err = EHBI_NULL_BYTES;
		Return_i(0);
	}
	if (bi->bytes_used == 0) {
		Return_i(0);
	}

	bit = 0x01 & bi->bytes[bi->bytes_len - 1];

	Return_i(bit ? 1 : 0);
}

static int ehbi_reset_bytes_used(struct ehbigint *bi)
{
	size_t i;

	Trace_bi(bi);

	if (bi == NULL) {
		Ehbi_log_error0("Null struct");
		Return_i(EHBI_NULL_STRUCT);
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		Return_i(EHBI_NULL_BYTES);
	}

	for (i = 0; i < bi->bytes_len; ++i) {
		if (bi->bytes[i] != 0) {
			break;
		}
	}
	bi->bytes_used = (bi->bytes_len - i);
	if (bi->bytes_used == 0) {
		bi->bytes_used = 1;
	}

	if ((bi->bytes_used == 1) && (bi->bytes[bi->bytes_len - 1] == 0x00)) {
		bi->sign = 0;
	}

	if (bi->sign && bi->bytes_used < bi->bytes_len
	    && (bi->bytes[bi->bytes_len - bi->bytes_used] > 0x7F)) {
		++(bi->bytes_used);
	}

	Trace_msg_s_bi("end", bi);
	Return_i(EHBI_SUCCESS);
}

static void ehbi_unsafe_zero(struct ehbigint *bi)
{
	size_t i;

	Trace_bi(bi);

	for (i = 0; i < bi->bytes_len; ++i) {
		bi->bytes[i] = 0x00;
	}
	bi->bytes_used = 1;
	bi->sign = 0;

	Trace_msg_s_bi("end", bi);
	Return_void();
}
