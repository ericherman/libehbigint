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

static int ehbi_zero(struct ehbigint *bi);

int ehbi_set_l(struct ehbigint *bi, long val)
{
	int err;
	err = ehbi_zero(bi);
	if (err) {
		return err;
	}
	return ehbi_inc_l(bi, val);
}

int ehbi_set(struct ehbigint *bi, const struct ehbigint *val)
{
	int err;
	size_t i;
	unsigned char byte;

	err = ehbi_zero(bi);
	if (err) {
		return err;
	}

	bi->sign = val->sign;
	bi->bytes_used = 0;
	for (i = 0; i < val->bytes_used; ++i) {
		if (bi->bytes_used >= bi->bytes_len) {
			ehbi_zero(bi);
			Ehbi_log_error0("Result byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		byte = val->bytes[val->bytes_len - 1 - i];
		bi->bytes[bi->bytes_len - 1 - i] = byte;
		++bi->bytes_used;
	}
	for (i = 0; i < (bi->bytes_len - bi->bytes_used); ++i) {
		bi->bytes[i] = 0x00;
	}

	return EHBI_SUCCESS;
}

int ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t i, size;
	unsigned char a, b, c;
	const struct ehbigint *swp;
	struct ehbigint tmp;
	int err;

	if (res == NULL || bi1 == NULL || bi2 == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (res->bytes == NULL || bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	/* adding zero */
	if (bi2->bytes_used == 1 && bi2->bytes[bi2->bytes_len - 1] == 0x00) {
		return ehbi_set(res, bi1);
	}

	/* adding to zero */
	if (bi1->bytes_used == 1 && bi1->bytes[bi1->bytes_len - 1] == 0x00) {
		return ehbi_set(res, bi2);
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
			ehbi_zero(res);
		}
		return err;
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
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
	}
	if (c) {
		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
		if (c == 0xFF) {
			if (res->bytes_used == res->bytes_len) {
				Ehbi_log_error0
				    ("Result byte[] too small for carry");
				return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
			}
			res->bytes_used++;
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_mul(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t size, i, j;
	int err;
	const struct ehbigint *t;
	unsigned int a, b, r;
	struct ehbigint tmp;

	if (res == NULL || bi1 == NULL || bi2 == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (res->bytes == NULL || bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
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
	}
	tmp.bytes_len = size;
	err = err || ehbi_zero(&tmp);
	err = err || ehbi_zero(res);
	if (err) {
		goto ehbi_mul_end;
	}

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
	return err;
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

	if (quotient == NULL || remainder == NULL || numerator == NULL
	    || denominator == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (quotient->bytes == NULL || remainder->bytes == NULL
	    || numerator->bytes == NULL || denominator->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	if (remainder->bytes_len < numerator->bytes_len) {
		Ehbi_log_error0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
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
		ehbi_zero(quotient);
		err = ehbi_set(remainder, abs_numer);
		goto ehbi_div_end;
	}
	if (err) {
		goto ehbi_div_end;
	}

	/* base 256 "long division" */
	ehbi_zero(quotient);
	ehbi_zero(remainder);

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
	return err;
}

int ehbi_inc(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i;
	unsigned char a, b, c;
	int err;
	struct ehbigint temp;

	if (bi == NULL || val == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (bi->bytes == NULL || val->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}
	if (val->bytes_used > bi->bytes_len) {
		Ehbi_log_error0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
	}

	err = 0;
	if (ehbi_is_negative(val, &err)) {
		temp.bytes_used = 0;
		temp.bytes_len = 0;
		temp.sign = 0;
		temp.bytes = (unsigned char *)ehbi_stack_alloc(val->bytes_used);
		if (!temp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)val->bytes_used);
			err = EHBI_STACK_TOO_SMALL;
			return err;
		}
		temp.bytes_len = val->bytes_used;
		err = ehbi_set(&temp, val);
		err = err || ehbi_negate(&temp);
		err = err || ehbi_dec(bi, &temp);
		ehbi_stack_free(temp.bytes, temp.bytes_len);
		return err;
	}
	if (err) {
		return err;
	}

	c = 0;

	for (i = 1; i <= val->bytes_used; ++i) {
		a = val->bytes[val->bytes_len - i];
		b = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		c = c + a + b;

		bi->bytes[bi->bytes_len - i] = c;
		if (bi->bytes_used < i) {
			bi->bytes_used = i;
		}

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
	}
	while (c) {
		if (i > bi->bytes_len) {
			Ehbi_log_error0("byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		a = c;
		b = (bi->bytes_used < i) ? 0 : bi->bytes[bi->bytes_len - i];
		c = a + b;

		bi->bytes[bi->bytes_len - i] = c;
		if (bi->bytes_used < i) {
			bi->bytes_used = i;
		}

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
		++i;
	}
	if (bi->bytes[bi->bytes_len - bi->bytes_used] == 0xFF) {
		if (bi->bytes_used == bi->bytes_len) {
			Ehbi_log_error0("byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		++bi->bytes_used;
		bi->bytes[bi->bytes_len - bi->bytes_used] = 0x00;
	}

	return EHBI_SUCCESS;
}

int ehbi_inc_l(struct ehbigint *bi, long val)
{
	size_t i, j;
	unsigned long v;
	unsigned char c, val_negative;
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

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

	return val_negative ? ehbi_dec(bi, &temp) : ehbi_inc(bi, &temp);
}

int ehbi_dec(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i, j, size;
	unsigned char a, b, c;
	struct ehbigint tmp;
	int err;

	if (bi == NULL || val == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (bi->bytes == NULL || val->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	if (ehbi_is_negative(val, &err)) {
		size = val->bytes_used;
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			return EHBI_STACK_TOO_SMALL;
		}
		tmp.bytes_len = size;
		tmp.bytes_used = 0;
		tmp.sign = 0;
		err = err || ehbi_set(&tmp, val);
		err = err || ehbi_negate(&tmp);
		err = err || ehbi_inc(bi, &tmp);
		ehbi_stack_free(tmp.bytes, tmp.bytes_len);
		return err;
	}

	err = 0;
	if (ehbi_greater_than(val, bi, &err)) {
		size = val->bytes_used;
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(size);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)size);
			return EHBI_STACK_TOO_SMALL;
		}
		tmp.bytes_len = size;
		tmp.bytes_used = 0;
		tmp.sign = 0;
		err = ehbi_subtract(&tmp, val, bi);
		err = err || ehbi_set(bi, &tmp);
		ehbi_stack_free(tmp.bytes, tmp.bytes_len);
		err = err || ehbi_negate(bi);
		return err;
	}
	if (err) {
		return err;
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
				return EHBI_BYTES_TOO_SMALL_FOR_BORROW;
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

	return EHBI_SUCCESS;
}

int ehbi_subtract(struct ehbigint *res, const struct ehbigint *bi1,
		  const struct ehbigint *bi2)
{
	size_t i, size;
	unsigned char a, b, c, negate;
	const struct ehbigint *swp;
	struct ehbigint abs;
	int err;

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
			return EHBI_BYTES_TOO_SMALL;
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

	err = err || EHBI_SUCCESS;

ehbi_subtract_end:
	if (err && res) {
		ehbi_zero(res);
	}
	if (abs.bytes) {
		ehbi_stack_free(abs.bytes, abs.bytes_len);
	}
	return EHBI_SUCCESS;
}

int ehbi_bytes_shift_left(struct ehbigint *bi, size_t num_bytes)
{
	size_t i;

	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	if (num_bytes == 0) {
		return EHBI_SUCCESS;
	}

	/* by ensuring that we have at least one extra byte of freespace,
	 * we avoid some werid sign issues */
	if (bi->bytes_len < (bi->bytes_used + 1 + num_bytes)) {
		Ehbi_log_error0("Result byte[] too small for shift");
		return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
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

	return EHBI_SUCCESS;
}

int ehbi_negate(struct ehbigint *bi)
{
	if (bi == NULL) {
		Ehbi_log_error0("Null argument");
		return EHBI_NULL_ARGS;
	}

	bi->sign = (bi->sign == 0) ? 1 : 0;

	return EHBI_SUCCESS;
}

int ehbi_is_negative(const struct ehbigint *bi, int *err)
{
	if (bi == NULL) {
		Ehbi_log_error0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		return 0;
	}

	return (bi->sign == 0) ? 0 : 1;
}

int ehbi_compare(const struct ehbigint *bi1, const struct ehbigint *bi2,
		 int *err)
{
	size_t i;
	unsigned char a, b;
	int b1_pos, b2_pos;

	if (bi1 == NULL || bi2 == NULL || err == NULL) {
		Ehbi_log_error0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		return 0;
	}
	if (bi1->bytes == NULL || bi2->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		if (err) {
			*err = EHBI_NULL_BYTES;
		}
		return 0;
	}

	*err = EHBI_SUCCESS;

	b1_pos = !ehbi_is_negative(bi1, err);
	b2_pos = !ehbi_is_negative(bi2, err);

	if (b1_pos != b2_pos) {
		return b1_pos ? 1 : -1;
	}

	if (bi1->bytes_used > bi2->bytes_used) {
		return b1_pos ? 1 : -1;
	} else if (bi1->bytes_used < bi2->bytes_used) {
		return b1_pos ? -1 : 1;
	}

	for (i = 0; i < bi1->bytes_used; ++i) {
		a = bi1->bytes[(bi1->bytes_len - bi1->bytes_used) + i];
		b = bi2->bytes[(bi2->bytes_len - bi2->bytes_used) + i];
		if (a > b) {
			return b1_pos ? 1 : -1;
		} else if (a < b) {
			return b1_pos ? -1 : 1;
		}
	}
	return 0;
}

int ehbi_equals(const struct ehbigint *bi1, const struct ehbigint *bi2,
		int *err)
{
	int terr;
	err = (err != NULL) ? err : &terr;
	return ((ehbi_compare(bi1, bi2, err) == 0) && (*err == EHBI_SUCCESS));
}

int ehbi_less_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		   int *err)
{
	int terr;
	err = (err != NULL) ? err : &terr;
	return ((ehbi_compare(bi1, bi2, err) < 0) && (*err == EHBI_SUCCESS));
}

int ehbi_greater_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		      int *err)
{
	int terr;
	err = (err != NULL) ? err : &terr;
	return ((ehbi_compare(bi1, bi2, err) > 0) && (*err == EHBI_SUCCESS));
}

static int ehbi_zero(struct ehbigint *bi)
{
	size_t i;
	if (bi == NULL) {
		Ehbi_log_error0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	for (i = 0; i < bi->bytes_len; ++i) {
		bi->bytes[i] = 0x00;
	}
	bi->bytes_used = 1;
	bi->sign = 0;

	return EHBI_SUCCESS;
}
