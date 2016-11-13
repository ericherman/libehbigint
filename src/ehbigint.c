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

int ehbi_set_ul(struct ehbigint *bi, unsigned long val)
{
	int err;
	err = ehbi_zero(bi);
	if (err) {
		return err;
	}
	return ehbi_inc_ul(bi, val);
}

int ehbi_set(struct ehbigint *bi, const struct ehbigint *val)
{
	int err;
	err = ehbi_zero(bi);
	if (err) {
		return err;
	}
	return ehbi_inc(bi, val);
}

int ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t i;
	unsigned char a, b, c;
	const struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (bi1->bytes_used < bi2->bytes_used) {
		tmp = bi1;
		bi1 = bi2;
		bi2 = tmp;
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
	size_t size;
	int err;
	struct ehbigint bidx, zero, one;
	const struct ehbigint *t1;
	unsigned char zero_bytes[2];
	unsigned char one_bytes[2];

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (ehbi_less_than(bi1, bi2, &err)) {
		t1 = bi1;
		bi1 = bi2;
		bi2 = t1;
	}
	if (err) {
		return err;
	}

	zero.bytes = zero_bytes;
	zero.bytes_len = 2;
	err = ehbi_zero(&zero);
	if (err) {
		return err;
	}

	one.bytes = one_bytes;
	one.bytes_len = 2;
	err = ehbi_zero(&one);
	if (err) {
		return err;
	}
	err = ehbi_inc_ul(&one, 1);
	if (err) {
		return err;
	}

	err = ehbi_set(res, &zero);
	if (err) {
		return err;
	}

	if (bi1->bytes_used > bi2->bytes_used) {
		size = bi1->bytes_used;
	} else {
		size = bi2->bytes_used;
	}
	size += 4;

	bidx.bytes = ehbi_stack_alloc(size);
	if (!bidx.bytes) {
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		return EHBI_STACK_TOO_SMALL;
	}
	bidx.bytes_len = size;

	err = ehbi_set(&bidx, bi2);
	if (err) {
		return err;
	}
	while (ehbi_greater_than(&bidx, &zero, &err)) {
		if (err) {
			return err;
		}
		err = ehbi_inc(res, bi1);
		if (err) {
			return err;
		}
		err = ehbi_dec(&bidx, &one);
		if (err) {
			return err;
		}
	}
	ehbi_stack_free(bidx.bytes, size);
	return err;
}

int ehbi_div(struct ehbigint *quotient, struct ehbigint *remainder,
	     const struct ehbigint *numerator,
	     const struct ehbigint *denominator)
{
	int err;
	size_t i, num_idx;

	if (quotient == 0 || remainder == 0 || numerator == 0
	    || denominator == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (remainder->bytes_len < numerator->bytes_len) {
		Ehbi_log_error0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
	}

	/* just early return if denominator is bigger than numerator */
	if (ehbi_greater_than(denominator, numerator, &err)) {
		ehbi_zero(quotient);
		ehbi_set(remainder, numerator);
		goto ehbi_div_end;
	} else if (err) {
		goto ehbi_div_end;
	}

	/* base 256 "long division" */
	ehbi_zero(quotient);
	ehbi_zero(remainder);
	num_idx = numerator->bytes_len - numerator->bytes_used;
	for (i = 0; i < denominator->bytes_used; ++i) {
		if ((remainder->bytes_used > 1)
		    || (remainder->bytes[remainder->bytes_len - 1] != 0x00)) {
			err = ehbi_bytes_shift_left(remainder, 1);
			if (err) {
				goto ehbi_div_end;
			}
		}
		remainder->bytes[remainder->bytes_len - 1] =
		    numerator->bytes[num_idx++];
	}
	if (ehbi_greater_than(denominator, remainder, &err)) {
		err = ehbi_bytes_shift_left(remainder, 1);
		if (err) {
			goto ehbi_div_end;
		}
		remainder->bytes[remainder->bytes_len - 1] =
		    numerator->bytes[num_idx++];
	}
	if (err) {
		goto ehbi_div_end;
	}

	i = 0;
	while (ehbi_greater_than(remainder, denominator, &err)) {
		if (err) {
			goto ehbi_div_end;
		}
		err = ehbi_inc_ul(quotient, 1);
		if (err) {
			goto ehbi_div_end;
		}
		err = ehbi_dec(remainder, denominator);
		if (err) {
			goto ehbi_div_end;
		}
		while (ehbi_less_than(remainder, denominator, &err)
		       && (num_idx < numerator->bytes_len)) {
			err = ehbi_bytes_shift_left(quotient, 1);
			if (err) {
				goto ehbi_div_end;
			}

			err = ehbi_bytes_shift_left(remainder, 1);
			if (err) {
				goto ehbi_div_end;
			}
			remainder->bytes[remainder->bytes_len - 1] =
			    numerator->bytes[num_idx++];
		}
		if (err) {
			goto ehbi_div_end;
		}
	}

ehbi_div_end:
	/* if error, let's not return garbage or 1/2 an answer */
	if (err) {
		ehbi_zero(quotient);
		ehbi_zero(remainder);
	}
	return err;
}

int ehbi_inc(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i;
	unsigned char a, b, c;

	if (bi == 0 || val == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (val->bytes_used > bi->bytes_len) {
		Ehbi_log_error0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
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

int ehbi_inc_ul(struct ehbigint *bi, unsigned long val)
{
	size_t i, j;
	unsigned char c;
	unsigned char bytes[1 + sizeof(unsigned long)];
	struct ehbigint temp;

	temp.bytes = bytes;
	temp.bytes_len = 1 + sizeof(unsigned long);
	temp.bytes_used = sizeof(unsigned long);

	temp.bytes[0] = 0x00;
	for (i = 0; i < temp.bytes_used; ++i) {
		c = (val >> (8 * i));
		j = (temp.bytes_len - 1) - i;
		temp.bytes[j] = c;
	}
	for (i = 0; i < temp.bytes_len; ++i) {
		if (temp.bytes[i] != 0x00) {
			break;
		}
	}
	temp.bytes_used = temp.bytes_len - i;
	if ((temp.bytes_used == 0)
	    || (temp.bytes[temp.bytes_len - temp.bytes_used] > 0x7F)) {
		++temp.bytes_used;
	}

	return ehbi_inc(bi, &temp);
}

int ehbi_dec(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i, j;
	unsigned char a, b, c;

	if (bi == 0 || val == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
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
	    && (bi->bytes[0] != 0xFF) && (bi->bytes[i] > 0x7F) && (i > 0)
	    && (i <= bi->bytes_len) && (bi->bytes[i - 1] == 0x00)) {
		++bi->bytes_used;
	}

	return EHBI_SUCCESS;
}

int ehbi_subtract(struct ehbigint *res, const struct ehbigint *bi1,
		  const struct ehbigint *bi2)
{
	size_t i, max_len;
	unsigned char a, b, c;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (bi1->bytes_used >= bi2->bytes_used) {
		max_len = bi1->bytes_used;
	} else {
		max_len = bi2->bytes_used;
	}

	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= max_len; ++i) {
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
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		while (i <= res->bytes_len) {
			res->bytes[res->bytes_len - i] = 0xFF;
			++i;
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_bytes_shift_left(struct ehbigint *bi, size_t num_bytes)
{
	size_t i;

	if (bi == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
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

int ehbi_is_negative(const struct ehbigint *bi)
{
	if (bi == NULL || bi->bytes_used == 0) {
		return 0;
	}

	if (bi->bytes[0] > 0x7F) {
		return 1;
	}

	return 0;
}

int ehbi_compare(const struct ehbigint *bi1, const struct ehbigint *bi2,
		 int *err)
{
	size_t i;
	unsigned char a, b;
	int b1_pos, b2_pos;

	if (bi1 == 0 || bi2 == 0 || err == 0) {
		Ehbi_log_error0("Null argument(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		return 0;
	}

	*err = EHBI_SUCCESS;

	b1_pos = !ehbi_is_negative(bi1);
	b2_pos = !ehbi_is_negative(bi2);

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
	return ((ehbi_compare(bi1, bi2, err) == 0) && (*err == EHBI_SUCCESS));
}

int ehbi_less_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		   int *err)
{
	return ((ehbi_compare(bi1, bi2, err) < 0) && (*err == EHBI_SUCCESS));
}

int ehbi_greater_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		      int *err)
{
	return ((ehbi_compare(bi1, bi2, err) > 0) && (*err == EHBI_SUCCESS));
}

static int ehbi_zero(struct ehbigint *bi)
{
	size_t i;
	if (bi == 0) {
		Ehbi_log_error0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	for (i = 0; i < bi->bytes_len; ++i) {
		bi->bytes[i] = 0x00;
	}
	bi->bytes_used = 1;

	return EHBI_SUCCESS;
}
