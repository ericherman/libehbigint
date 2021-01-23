/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint.c: slow Big Int library hopefully somewhat suitable for 8bit CPUs */
/* Copyright (C) 2016, 2018, 2019 Eric Herman <eric@freesa.org> */

#include "ehbigint.h"
#include "eembed.h"
#include "eba.h"
#include "ehstr.h"

#include <limits.h>		/* LONG_MAX */

#ifdef _POSIX_SOURCE
#include <execinfo.h>		/* backtrace backtrace_symbols */
#include <stdlib.h>		/* free */
#endif

static struct eembed_log *global_ehbi_log = NULL;

struct eembed_log *ehbi_log_get(void)
{
	struct eembed_log *log = global_ehbi_log;
	return log ? log : eembed_err_log;
}

struct eembed_log *ehbi_log_set(struct eembed_log *log)
{
	struct eembed_log *previous = ehbi_log_get();

	global_ehbi_log = log;

	return previous;
}

void ehbi_log_backtrace(struct eembed_log *log)
{
#if _POSIX_SOURCE
	const size_t buffer_size = 100;
	void *buffer[100];
	char **strings = NULL;
	int nptrs = 0;
	int i = 0;
#endif

	if (!log) {
		return;
	}

#if _POSIX_SOURCE
	nptrs = backtrace(buffer, buffer_size);
	log->append_s(log, "backtrace() returned ");
	log->append_l(log, nptrs);
	log->append_s(log, " addresses");
	log->append_eol(log);

	strings = backtrace_symbols(buffer, nptrs);
	if (strings != NULL) {
		for (i = 0; i < nptrs; ++i) {
			log->append_s(log, "\t");
			log->append_s(log, strings[i]);
			log->append_eol(log);
		}
		free(strings);
	}
#else
	log->append_s(log, "(backtrace unavailable)");
	log->append_eol(log);
#endif
}

static void ehbi_log_file_line(const char *file, unsigned long line,
			       struct eembed_log *log)
{
	if (!log) {
		return;
	}
	log->append_s(log, file);
	log->append_s(log, ":");
	log->append_ul(log, line);
	log->append_s(log, ": ");
}

#define Ehbi_log_file_line(log) \
	ehbi_log_file_line(__FILE__, __LINE__, log)

static void ehbi_log_error0(const char *file, unsigned long line,
			    const char *msg)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, msg);
	log->append_eol(log);
}

#define Ehbi_log_error0(msg) \
	ehbi_log_error0(__FILE__, __LINE__, msg)

static void ehbi_log_error_s_c_s(const char *file, unsigned long line,
				 const char *pre, char c, const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_c(log, c);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_c_s(pre, c, post) \
	ehbi_log_error_s_c_s(__FILE__, __LINE__, pre, c, post)

static void ehbi_log_error_s_s_s(const char *file, unsigned long line,
				 const char *pre, const char *mid,
				 const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}

	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_s(log, mid);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_s_s(pre, mid, post) \
	ehbi_log_error_s_s_s(__FILE__, __LINE__, pre, mid, post)

static void ehbi_log_error_s_ul_s(const char *file, unsigned long line,
				  const char *pre, unsigned long ul,
				  const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_ul(log, ul);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_ul_s(pre, ul, post) \
	ehbi_log_error_s_ul_s(__FILE__, __LINE__, pre, ul, post)

static void ehbi_log_error_s_l_s(const char *file, unsigned long line,
				 const char *pre, long l, const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_l(log, l);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_l_s(pre, l, post) \
	ehbi_log_error_s_l_s(__FILE__, __LINE__, pre, l, post)

static void ehbi_log_error_s_c_s_c_s(const char *file, unsigned long line,
				     const char *pre, char c1, const char *mid,
				     char c2, const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_c(log, c1);
	log->append_s(log, mid);
	log->append_c(log, c2);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_c_s_c_s(pre, c1, mid, c2, post) \
	ehbi_log_error_s_c_s_c_s(__FILE__, __LINE__, pre, c1, mid, c2, post)

static void ehbi_log_error_s_ul_s_ul_s(const char *file, unsigned long line,
				       const char *pre, unsigned long ul1,
				       const char *mid, unsigned long ul2,
				       const char *post)
{
	struct eembed_log *log = ehbi_log_get();
	if (!log) {
		return;
	}
	ehbi_log_file_line(file, line, log);
	log->append_s(log, pre);
	log->append_ul(log, ul1);
	log->append_s(log, mid);
	log->append_ul(log, ul2);
	log->append_s(log, post);
	log->append_eol(log);
}

#define Ehbi_log_error_s_ul_s_ul_s(pre, ul1, mid, ul2, post) \
	ehbi_log_error_s_ul_s_ul_s(__FILE__, __LINE__, pre, ul1, mid, ul2, post)

static void ehbi_set_error(int *err, int code)
{
	if (err) {
		*err = code;
	}
}

#if EHBI_SKIP_STRUCT_NULL_CHECK
#define Ehbi_struct_is_not_null(bi, err, action) ((void)err)
#else
#define Ehbi_struct_is_not_null(bi, err, action) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null struct"); \
			ehbi_set_error(err, EHBI_NULL_STRUCT); \
			action; \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			ehbi_set_error(err, EHBI_NULL_BYTES); \
			action; \
		} \
	} while(0)

#endif /* EHBI_SKIP_STRUCT_NULL_CHECK */
#define Ehbi_stack_alloc_struct(tmp, size, err, action) \
	do { \
		tmp.bytes = NULL; \
		tmp.bytes_len = 0; \
		tmp.bytes_used = 0; \
		tmp.sign = 0; \
		tmp.bytes = (unsigned char *)eembed_alloca(size); \
		if (!tmp.bytes) { \
			Ehbi_log_error_s_ul_s( \
				"Could not allocate ", \
				size, \
				" bytes?"); \
			ehbi_set_error(err, EHBI_STACK_TOO_SMALL); \
			action; \
		} else { \
			tmp.bytes_len = size; \
			eembed_memset(tmp.bytes, 0x00, tmp.bytes_len); \
			tmp.bytes_used = 1; \
		} \
	} while (0)

/* would a union make this nicer? */
static char *ehbi_hex_chars_from_byte(char *high, char *low, unsigned char byte,
				      int *err);
static unsigned char ehbi_hex_chars_to_byte(char high, char low, int *err);

static void ehbi_internal_reset_bytes_used(struct ehbigint *bi, size_t from);

static void ehbi_internal_zero(struct ehbigint *bi)
{
	eembed_memset(bi->bytes, 0x00, bi->bytes_len);
	bi->bytes_used = 1;
	bi->sign = 0;
}

static void ehbi_internal_clear_null_struct(struct ehbigint *bi)
{
	bi->bytes = NULL;
	bi->bytes_len = 0;
	bi->bytes_used = 0;
	bi->sign = 0;
}

static void ehbi_internal_struct_l(struct ehbigint *temp, long val)
{
	unsigned long v;
	unsigned char c;
	size_t i, j;

	temp->bytes_used = sizeof(unsigned long);

	v = (val < 0) ? (unsigned long)(-val) : (unsigned long)val;

	for (i = 0; i < temp->bytes_used; ++i) {
		c = (v >> (8 * i));
		j = (temp->bytes_len - 1) - i;
		temp->bytes[j] = c;
	}
	temp->sign = (val < 0);
	ehbi_internal_reset_bytes_used(temp, sizeof(unsigned long));
}

struct ehbigint *ehbi_init(struct ehbigint *bi, unsigned char *bytes,
			   size_t len, int *err)
{
	if (bi == NULL) {
		Ehbi_log_error0("Null struct");
		ehbi_set_error(err, EHBI_NULL_STRUCT);
		return NULL;
	}

	bi->bytes = NULL;
	bi->bytes_len = 0;
	bi->bytes_used = 0;
	bi->sign = 0;

	if (bytes == NULL) {
		Ehbi_log_error0("Null bytes[]");
		ehbi_set_error(err, EHBI_NULL_BYTES);
		return NULL;
	}

	eembed_memset(bytes, 0x00, len);

	bi->bytes = bytes;
	bi->bytes_len = len;
	bi->bytes_used = 1;

	return bi;
}

struct ehbigint *ehbi_init_l(struct ehbigint *bi, unsigned char *bytes,
			     size_t len, long val, int *err)
{
	if (!ehbi_init(bi, bytes, len, err)) {
		return NULL;
	}
	return ehbi_set_l(bi, val, err);
}

struct ehbigint *ehbi_zero(struct ehbigint *bi, int *err)
{
	Ehbi_struct_is_not_null(bi, err, return NULL);

	ehbi_internal_zero(bi);

	return bi;
}

struct ehbigint *ehbi_set_l(struct ehbigint *bi, long val, int *err)
{
	Ehbi_struct_is_not_null(bi, err, return NULL);

	ehbi_internal_zero(bi);

	return ehbi_inc_l(bi, val, err);
}

struct ehbigint *ehbi_set(struct ehbigint *bi, const struct ehbigint *val,
			  int *err)
{
	size_t offset, voffset;

	Ehbi_struct_is_not_null(bi, err, return NULL);
	Ehbi_struct_is_not_null(val, err, goto ehbi_set_error);

	if (val->bytes_used > bi->bytes_len) {
		Ehbi_log_error_s_ul_s_ul_s("Result byte[", bi->bytes_len,
					   "] too small (", val->bytes_used,
					   ")");
		ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
		goto ehbi_set_error;
	}
	bi->sign = val->sign;
	bi->bytes_used = val->bytes_used;

	offset = bi->bytes_len - bi->bytes_used;
	if (offset) {
		eembed_memset(bi->bytes, 0x00, offset);
	}

	voffset = val->bytes_len - val->bytes_used;
	eembed_memcpy(bi->bytes + offset, val->bytes + voffset,
		      val->bytes_used);

	return bi;

ehbi_set_error:
	ehbi_internal_zero(bi);
	return NULL;
}

static int ehbi_internal_is_zero(const struct ehbigint *bi)
{
	return (bi->bytes_used <= 1 && bi->bytes[bi->bytes_len - 1] == 0x00);
}

struct ehbigint *ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
			  const struct ehbigint *bi2, int *err)
{
	size_t i, size;
	unsigned char a, b, c;
	const struct ehbigint *swp;
	struct ehbigint tmp;

	ehbi_internal_clear_null_struct(&tmp);

	Ehbi_struct_is_not_null(res, err, return NULL);
	Ehbi_struct_is_not_null(bi1, err, goto ehbi_add_error);
	Ehbi_struct_is_not_null(bi2, err, goto ehbi_add_error);

	/* adding zero */
	if (ehbi_internal_is_zero(bi2)) {
		return ehbi_set(res, bi1, err);
	}

	/* adding to zero */
	if (ehbi_internal_is_zero(bi1)) {
		return ehbi_set(res, bi2, err);
	}

	if (bi1->sign != bi2->sign) {
		size = bi2->bytes_len;
		Ehbi_stack_alloc_struct(tmp, size, err, goto ehbi_add_error);
		swp = ehbi_set(&tmp, bi2, err);
		if (!swp) {
			goto ehbi_add_error;
		}
		swp = ehbi_negate(&tmp, err);
		if (!swp) {
			goto ehbi_add_error;
		}
		swp = ehbi_subtract(res, bi1, &tmp, err);
		if (!swp) {
			goto ehbi_add_error;
		}
		eembed_freea(tmp.bytes);
		return res;
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
			Ehbi_log_error_s_ul_s_ul_s("Result byte[",
						   res->bytes_len,
						   "] too small (", i, ")");
			ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
			goto ehbi_add_error;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
	}
	if (c) {
		if (i > res->bytes_len) {
			Ehbi_log_error_s_ul_s_ul_s("Result byte[",
						   res->bytes_len,
						   "] too small (", i, ")");
			ehbi_set_error(err, EHBI_BYTES_TOO_SMALL_FOR_CARRY);
			goto ehbi_add_error;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
		if (c == 0xFF) {
			if (res->bytes_used == res->bytes_len) {
				Ehbi_log_error_s_ul_s("Result byte[",
						      res->bytes_len,
						      "] too small for carry");
				ehbi_set_error(err,
					       EHBI_BYTES_TOO_SMALL_FOR_CARRY);
				goto ehbi_add_error;
			}
			res->bytes_used++;
		}
	}

	if (ehbi_internal_is_zero(res)) {
		res->sign = 0;
	}

	return res;

ehbi_add_error:
	ehbi_internal_zero(res);
	eembed_freea(tmp.bytes);
	return NULL;
}

struct ehbigint *ehbi_add_l(struct ehbigint *res, const struct ehbigint *bi1,
			    long v2, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, v2);

	return ehbi_add(res, bi1, &temp, err);
}

struct ehbigint *ehbi_mul(struct ehbigint *res, const struct ehbigint *bi1,
			  const struct ehbigint *bi2, int *err)
{
	size_t i, j;
	const struct ehbigint *t;
	unsigned int a, b, r;
	struct ehbigint tmp;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&tmp);

	Ehbi_struct_is_not_null(res, err, return NULL);
	ehbi_internal_zero(res);
	Ehbi_struct_is_not_null(bi1, err, return NULL);
	Ehbi_struct_is_not_null(bi2, err, return NULL);

	err = 0;
	if (bi1->bytes_used < bi2->bytes_used) {
		t = bi1;
		bi1 = bi2;
		bi2 = t;
	}

	Ehbi_stack_alloc_struct(tmp, res->bytes_len, err, return NULL);
	ehbi_internal_zero(&tmp);

	rp = res;
	for (i = 0; i < bi2->bytes_used; ++i) {
		for (j = 0; j < bi1->bytes_used; ++j) {
			a = bi2->bytes[(bi2->bytes_len - 1) - i];
			b = bi1->bytes[(bi1->bytes_len - 1) - j];
			r = (a * b);
			rp = ehbi_set_l(&tmp, r, err);
			if (!rp) {
				goto ehbi_mul_end;
			}
			rp = ehbi_shift_left(&tmp, i * EEMBED_CHAR_BIT, err);
			if (!rp) {
				goto ehbi_mul_end;
			}
			rp = ehbi_shift_left(&tmp, j * EEMBED_CHAR_BIT, err);
			if (!rp) {
				goto ehbi_mul_end;
			}
			rp = ehbi_inc(res, &tmp, err);
			if (!rp) {
				goto ehbi_mul_end;
			}
		}
	}

	if (bi1->sign != bi2->sign) {
		res->sign = 1;
	}

ehbi_mul_end:
	if (!rp) {
		ehbi_zero(res, err);
	}

	eembed_freea(tmp.bytes);

	return rp;
}

struct ehbigint *ehbi_mul_l(struct ehbigint *res, const struct ehbigint *bi1,
			    long v2, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, v2);

	return ehbi_mul(res, bi1, &temp, err);
}

struct ehbigint *ehbi_div(struct ehbigint *quotient, struct ehbigint *remainder,
			  const struct ehbigint *numerator,
			  const struct ehbigint *denominator, int *err)
{
	size_t i, size, num_idx;
	struct ehbigint s_abs_numer;
	struct ehbigint s_abs_denom;
	const struct ehbigint *abs_numer;
	const struct ehbigint *abs_denom;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&s_abs_numer);
	ehbi_internal_clear_null_struct(&s_abs_denom);

	Ehbi_struct_is_not_null(quotient, err, return NULL);
	ehbi_internal_zero(quotient);
	Ehbi_struct_is_not_null(remainder, err, return NULL);
	ehbi_internal_zero(remainder);
	Ehbi_struct_is_not_null(numerator, err, return NULL);
	Ehbi_struct_is_not_null(denominator, err, return NULL);

	rp = quotient;

	if (remainder->bytes_len < numerator->bytes_used) {
		Ehbi_log_error_s_ul_s_ul_s("byte[] too small;"
					   " remainder->bytes_len < numerator->bytes_used"
					   " (", remainder->bytes_len, " < ",
					   numerator->bytes_used, ")");
		ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
		return NULL;
	}

	if (numerator->sign == 0) {
		abs_numer = numerator;
	} else {
		s_abs_numer.bytes_used = 0;
		s_abs_numer.bytes_len = 0;
		s_abs_numer.sign = 0;
		size = numerator->bytes_used;
		Ehbi_stack_alloc_struct(s_abs_numer, size, err, return NULL);
		rp = ehbi_set(&s_abs_numer, numerator, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		rp = ehbi_negate(&s_abs_numer, err);
		if (!rp) {
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
		Ehbi_stack_alloc_struct(s_abs_denom, size, err, return NULL);
		rp = ehbi_set(&s_abs_denom, denominator, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		rp = ehbi_negate(&s_abs_denom, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		abs_denom = &s_abs_denom;
	}

	/* just early return if abs_denom is bigger than abs_numer */
	if (ehbi_greater_than(abs_denom, abs_numer, err)) {
		ehbi_internal_zero(quotient);
		rp = ehbi_set(remainder, abs_numer, err);
		goto ehbi_div_end;
	}

	/* base 256 "long division" */

	if (ehbi_equals(abs_denom, quotient, err)) {
		Ehbi_log_error0("denominator == 0");
		ehbi_set_error(err, EHBI_DIVIDE_BY_ZERO);
		goto ehbi_div_end;
	}

	num_idx = abs_numer->bytes_len - abs_numer->bytes_used;
	for (i = 0; i < abs_denom->bytes_used; ++i) {
		if (!ehbi_internal_is_zero(remainder)) {
			rp = ehbi_shift_left(remainder, EEMBED_CHAR_BIT, err);
			if (!rp) {
				goto ehbi_div_end;
			}
		}
		rp = ehbi_inc_l(remainder, abs_numer->bytes[num_idx++], err);
		if (!rp) {
			goto ehbi_div_end;
		}
	}
	if (ehbi_greater_than(abs_denom, remainder, err)) {
		rp = ehbi_shift_left(remainder, EEMBED_CHAR_BIT, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		rp = ehbi_inc_l(remainder, abs_numer->bytes[num_idx++], err);
		if (!rp) {
			goto ehbi_div_end;
		}
	}

	i = 0;
	while (ehbi_greater_than(remainder, abs_denom, err)
	       || ehbi_equals(remainder, abs_denom, err)) {
		rp = ehbi_inc_l(quotient, 1, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		rp = ehbi_dec(remainder, abs_denom, err);
		if (!rp) {
			goto ehbi_div_end;
		}
		while (ehbi_less_than(remainder, abs_denom, err)
		       && (num_idx < abs_numer->bytes_len)) {
			rp = ehbi_shift_left(quotient, EEMBED_CHAR_BIT, err);
			if (!rp) {
				goto ehbi_div_end;
			}

			rp = ehbi_shift_left(remainder, EEMBED_CHAR_BIT, err);
			if (!rp) {
				goto ehbi_div_end;
			}
			remainder->bytes[remainder->bytes_len - 1] =
			    abs_numer->bytes[num_idx++];
		}
	}

	if (numerator->sign != denominator->sign) {
		quotient->sign = 1;
	}

ehbi_div_end:
	eembed_freea(s_abs_denom.bytes);
	eembed_freea(s_abs_numer.bytes);

	/* if error, let's not return garbage or 1/2 an answer */
	if (!rp) {
		ehbi_zero(quotient, err);
		ehbi_zero(remainder, err);
		return NULL;
	}
	return quotient;
}

struct ehbigint *ehbi_div_l(struct ehbigint *quotient,
			    struct ehbigint *remainder,
			    const struct ehbigint *numerator, long denominator,
			    int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, denominator);

	return ehbi_div(quotient, remainder, numerator, &temp, err);
}

struct ehbigint *ehbi_sqrt(struct ehbigint *result, struct ehbigint *remainder,
			   const struct ehbigint *val, int *err)
{
	struct ehbigint zero, one, two;
	struct ehbigint guess, temp, junk;
	unsigned char zbytes[2];
	unsigned char obytes[2];
	unsigned char tbytes[2];
	size_t size;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&zero);
	ehbi_internal_clear_null_struct(&one);
	ehbi_internal_clear_null_struct(&two);
	ehbi_internal_clear_null_struct(&guess);
	ehbi_internal_clear_null_struct(&temp);
	ehbi_internal_clear_null_struct(&junk);

	zero.bytes = zbytes;
	zero.bytes_len = 2;
	ehbi_internal_zero(&zero);

	one.bytes = obytes;
	one.bytes_len = 2;
	ehbi_internal_zero(&one);
	ehbi_inc_l(&one, 1, err);

	two.bytes = tbytes;
	two.bytes_len = 2;
	ehbi_internal_zero(&two);
	ehbi_inc_l(&two, 2, err);

	Ehbi_struct_is_not_null(result, err, return NULL);
	ehbi_internal_zero(result);
	Ehbi_struct_is_not_null(remainder, err, return NULL);
	ehbi_internal_zero(remainder);
	Ehbi_struct_is_not_null(val, err, return NULL);

	if (ehbi_less_than(val, &zero, err)) {
		Ehbi_log_error0("square root of a negative would be complex");
		ehbi_set_error(err, EHBI_SQRT_NEGATIVE);
		return NULL;
	}

	if (ehbi_equals(val, &zero, err)) {
		return result;
	}

	size = 1 + val->bytes_used;
	Ehbi_stack_alloc_struct(guess, size, err, return NULL);

	size = 1 + (val->bytes_used * 2);
	Ehbi_stack_alloc_struct(temp, size, err, goto ehbi_sqrt_end);
	Ehbi_stack_alloc_struct(junk, size, err, goto ehbi_sqrt_end);

	/* odd cases below square root of 4 */
	rp = ehbi_set_l(&temp, 4, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}
	if (ehbi_less_than(val, &temp, err)) {
		rp = ehbi_set_l(result, 1, err);
		if (!rp) {
			goto ehbi_sqrt_end;
		}
		rp = ehbi_subtract(remainder, val, result, err);
		goto ehbi_sqrt_end;
	}

	/* Initial estimate, never low */
	/* result = (val / 2) + 1; */
	rp = ehbi_div(result, &junk, val, &two, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}
	rp = ehbi_inc(result, &one, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}

	/* guess = (result + (val / result)) / 2; */
	rp = ehbi_div(&temp, &junk, val, result, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}
	rp = ehbi_inc(&temp, result, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}
	rp = ehbi_div(&guess, &junk, &temp, &two, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}

	while (ehbi_less_than(&guess, result, err)) {
		/* result = guess; */
		rp = ehbi_set(result, &guess, err);
		if (!rp) {
			goto ehbi_sqrt_end;
		}

		/* guess = (result + (val / result)) / 2; */
		rp = ehbi_div(&temp, &junk, val, result, err);
		if (!rp) {
			goto ehbi_sqrt_end;
		}
		rp = ehbi_inc(&temp, result, err);
		if (!rp) {
			goto ehbi_sqrt_end;
		}
		rp = ehbi_div(&guess, &junk, &temp, &two, err);
		if (!rp) {
			goto ehbi_sqrt_end;
		}
	}
	rp = ehbi_mul(&temp, result, result, err);
	if (!rp) {
		goto ehbi_sqrt_end;
	}
	rp = ehbi_subtract(remainder, val, &temp, err);

ehbi_sqrt_end:
	eembed_freea(guess.bytes);
	eembed_freea(temp.bytes);
	eembed_freea(junk.bytes);

	if (!rp) {
		ehbi_zero(result, err);
		ehbi_zero(remainder, err);
		return NULL;
	}

	return result;
}

struct ehbigint *ehbi_exp(struct ehbigint *result, const struct ehbigint *base,
			  const struct ehbigint *exponent, int *err)
{
	struct ehbigint loop;
	struct ehbigint tmp;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&loop);

	Ehbi_stack_alloc_struct(loop, exponent->bytes_used, err,
				goto ehbi_exp_end);
	Ehbi_stack_alloc_struct(tmp, result->bytes_len, err, goto ehbi_exp_end);

	rp = ehbi_zero(&loop, err);
	if (!rp) {
		goto ehbi_exp_end;
	}
	rp = ehbi_set_l(result, 1, err);
	if (!rp) {
		goto ehbi_exp_end;
	}

	while (ehbi_less_than(&loop, exponent, err)) {
		rp = ehbi_mul(&tmp, result, base, err);
		if (!rp) {
			goto ehbi_exp_end;
		}
		rp = ehbi_set(result, &tmp, err);
		if (!rp) {
			goto ehbi_exp_end;
		}
		rp = ehbi_inc_l(&loop, 1, err);
		if (!rp) {
			goto ehbi_exp_end;
		}
	}

ehbi_exp_end:
	eembed_freea(loop.bytes);
	eembed_freea(tmp.bytes);

	if (!rp) {
		ehbi_zero(result, err);
		return 0;
	}

	return result;
}

struct ehbigint *ehbi_exp_l(struct ehbigint *result,
			    const struct ehbigint *base, long exp, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, exp);

	return ehbi_exp(result, base, &temp, err);
}

struct ehbigint *ehbi_exp_mod(struct ehbigint *result,
			      const struct ehbigint *base,
			      const struct ehbigint *exponent,
			      const struct ehbigint *modulus, int *err)
{
	size_t size;
	struct ehbigint zero, tmp1, tjunk, texp, tbase;
	struct ehbigint *rp;
	unsigned char zero_bytes[2];

	ehbi_internal_clear_null_struct(&zero);
	ehbi_internal_clear_null_struct(&tmp1);
	ehbi_internal_clear_null_struct(&tbase);
	ehbi_internal_clear_null_struct(&texp);
	ehbi_internal_clear_null_struct(&tjunk);
	rp = NULL;

	Ehbi_struct_is_not_null(result, err, return NULL);
	ehbi_internal_zero(result);
	Ehbi_struct_is_not_null(base, err, return NULL);
	Ehbi_struct_is_not_null(exponent, err, return NULL);
	Ehbi_struct_is_not_null(modulus, err, return NULL);

	ehbi_init(&zero, zero_bytes, 2, err);

	size = 8 + (4 * base->bytes_used) + (4 * exponent->bytes_used);

	Ehbi_stack_alloc_struct(tmp1, size, err, return NULL);
	Ehbi_stack_alloc_struct(tbase, size, err, goto ehbi_mod_exp_end);
	Ehbi_stack_alloc_struct(texp, size, err, goto ehbi_mod_exp_end);
	Ehbi_stack_alloc_struct(tjunk, size, err, goto ehbi_mod_exp_end);

	/* prevent divide by zero */
	ehbi_internal_zero(&tmp1);
	if (ehbi_equals(modulus, &tmp1, err)) {
		Ehbi_log_error0("modulus == 0");
		ehbi_set_error(err, EHBI_DIVIDE_BY_ZERO);
		goto ehbi_mod_exp_end;
	}

	/* prevent negative eponent */
	if (ehbi_is_negative(exponent, err)) {
		Ehbi_log_error0("exponent < 0");
		ehbi_set_error(err, EHBI_BAD_DATA);
		goto ehbi_mod_exp_end;
	}

	/*
	   The following is an example in pseudocode based on Applied
	   Cryptography: Protocols, Algorithms, and Source Code in C,
	   Second Edition (2nd ed.), page 244.
	   Schneier, Bruce (1996). Wiley. ISBN 978-0-471-11709-4.

	   function modular_pow(base, exponent, modulus)
	   if modulus = 1 then return 0
	   Assert :: (modulus - 1) * (modulus - 1) does not overflow base
	   result := 1
	   base := base mod modulus
	   while exponent > 0
	   if (exponent mod 2 == 1):
	   result := (result * base) mod modulus
	   exponent := exponent >> 1
	   base := (base * base) mod modulus
	   return result

	   See Also:
	   https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
	 */

	/* if modulus == 1 then return 0 */
	rp = ehbi_set_l(&tmp1, 1, err);
	if (!rp) {
		goto ehbi_mod_exp_end;
	}

	if (ehbi_equals(modulus, &tmp1, err)) {
		ehbi_internal_zero(result);
		goto ehbi_mod_exp_end;
	}

	rp = ehbi_set(&tbase, base, err);
	if (!rp) {
		goto ehbi_mod_exp_end;
	}
	rp = ehbi_set(&texp, exponent, err);
	if (!rp) {
		goto ehbi_mod_exp_end;
	}

	/* result := 1 */
	rp = ehbi_set_l(result, 1, err);
	if (!rp) {
		goto ehbi_mod_exp_end;
	}

	/* base := base mod modulus */
	rp = ehbi_div(&tjunk, &tbase, base, modulus, err);
	if (!err) {
		goto ehbi_mod_exp_end;
	}

	/* while exponent > 0 */
	while (ehbi_greater_than(&texp, &zero, err)) {
		/* if (exponent mod 2 == 1): */
		if (ehbi_is_odd(&texp, err)) {
			/* result := (result * base) mod modulus */
			rp = ehbi_mul(&tmp1, result, &tbase, err);
			if (!rp) {
				goto ehbi_mod_exp_end;
			}
			rp = ehbi_div(&tjunk, result, &tmp1, modulus, err);
			if (!rp) {
				goto ehbi_mod_exp_end;
			}
		}

		/* exponent := exponent >> 1 */
		rp = ehbi_shift_right(&texp, 1, err);
		if (!rp) {
			goto ehbi_mod_exp_end;
		}

		/* base := (base * base) mod modulus */
		rp = ehbi_mul(&tmp1, &tbase, &tbase, err);
		if (!rp) {
			goto ehbi_mod_exp_end;
		}
		rp = ehbi_div(&tjunk, &tbase, &tmp1, modulus, err);
		if (!rp) {
			goto ehbi_mod_exp_end;
		}
	}

	/* return result */

ehbi_mod_exp_end:
	eembed_freea(tmp1.bytes);
	eembed_freea(tbase.bytes);
	eembed_freea(texp.bytes);
	eembed_freea(tjunk.bytes);

	if (!rp) {
		ehbi_zero(result, err);
		return NULL;
	}

	return result;
}

struct ehbigint *ehbi_exp_mod_l(struct ehbigint *result,
				const struct ehbigint *base,
				const struct ehbigint *exponent, long modulus,
				int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);

	ehbi_internal_struct_l(&temp, modulus);

	return ehbi_exp_mod(result, base, exponent, &temp, err);
}

struct ehbigint *ehbi_exp_mod_ll(struct ehbigint *result,
				 const struct ehbigint *base, long exponent,
				 long modulus, int *err)
{
	unsigned char bytes1[sizeof(unsigned long)];
	struct ehbigint temp1;
	unsigned char bytes2[sizeof(unsigned long)];
	struct ehbigint temp2;

	ehbi_internal_clear_null_struct(&temp1);
	ehbi_internal_clear_null_struct(&temp2);

	temp1.bytes = bytes1;
	temp1.bytes_len = sizeof(unsigned long);

	temp2.bytes = bytes2;
	temp2.bytes_len = sizeof(unsigned long);

	ehbi_internal_struct_l(&temp1, exponent);
	ehbi_internal_struct_l(&temp2, modulus);

	return ehbi_exp_mod(result, base, &temp1, &temp2, err);
}

struct ehbigint *ehbi_inc(struct ehbigint *bi, const struct ehbigint *val,
			  int *err)
{
	size_t size;
	struct ehbigint temp;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&temp);

	Ehbi_struct_is_not_null(bi, err, return NULL);
	Ehbi_struct_is_not_null(val, err, return NULL);

	if (val->bytes_used > bi->bytes_len) {
		Ehbi_log_error_s_ul_s_ul_s("byte[",
					   bi->bytes_len,
					   "] too small (",
					   val->bytes_used, ")");
		ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
		return NULL;
	}

	size = bi->bytes_used;
	Ehbi_stack_alloc_struct(temp, size, err, return NULL);

	rp = ehbi_set(&temp, bi, err);
	if (!rp) {
		goto ehbi_inc_end;
	}
	rp = ehbi_add(bi, &temp, val, err);

ehbi_inc_end:
	eembed_freea(temp.bytes);
	if (!rp) {
		return NULL;
	}
	return bi;
}

struct ehbigint *ehbi_inc_l(struct ehbigint *bi, long val, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);

	ehbi_internal_struct_l(&temp, val);

	return ehbi_inc(bi, &temp, err);
}

struct ehbigint *ehbi_dec(struct ehbigint *bi, const struct ehbigint *val,
			  int *err)
{
	size_t size;
	struct ehbigint temp;
	struct ehbigint *rp;

	ehbi_internal_clear_null_struct(&temp);

	Ehbi_struct_is_not_null(bi, err, return NULL);
	Ehbi_struct_is_not_null(val, err, return NULL);

	size = bi->bytes_len;

	Ehbi_stack_alloc_struct(temp, size, err, return NULL);
	ehbi_internal_zero(&temp);

	rp = ehbi_subtract(&temp, bi, val, err);
	if (!rp) {
		goto ehbi_dec_end;
	}
	rp = ehbi_set(bi, &temp, err);

ehbi_dec_end:
	eembed_freea(temp.bytes);

	if (!rp) {
		return NULL;
	}

	return bi;
}

struct ehbigint *ehbi_dec_l(struct ehbigint *bi, long val, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, val);

	return ehbi_dec(bi, &temp, err);
}

struct ehbigint *ehbi_subtract(struct ehbigint *res, const struct ehbigint *bi1,
			       const struct ehbigint *bi2, int *err)
{
	size_t i, j, size;
	unsigned char a, b, c, negate;
	const struct ehbigint *swp;
	struct ehbigint *bi1a;
	struct ehbigint tmp;
	struct ehbigint *rp;
	/* char buf[80]; */

	ehbi_internal_clear_null_struct(&tmp);

	Ehbi_struct_is_not_null(res, err, return NULL);
	Ehbi_struct_is_not_null(bi1, err, return NULL);
	Ehbi_struct_is_not_null(bi2, err, return NULL);

	rp = res;

	/* subtract zero */
	if (ehbi_internal_is_zero(bi2)) {
		rp = ehbi_set(res, bi1, err);
		goto ehbi_subtract_end;
	}

	/* subtract from 0 */
	if (ehbi_internal_is_zero(bi1)) {
		rp = ehbi_set(res, bi2, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_negate(res, err);
		goto ehbi_subtract_end;
	}

	/* subtracting a negative */
	if (bi1->sign == 0 && bi2->sign != 0) {
		size = bi2->bytes_len;
		Ehbi_stack_alloc_struct(tmp, size, err, goto ehbi_subtract_end);
		rp = ehbi_set(&tmp, bi2, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_negate(&tmp, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_add(res, bi1, &tmp, err);
		goto ehbi_subtract_end;
	}

	/* negative subtracting a positive */
	if (bi1->sign != 0 && bi2->sign == 0) {
		size = bi1->bytes_len;
		Ehbi_stack_alloc_struct(tmp, size, err, goto ehbi_subtract_end);
		rp = ehbi_set(&tmp, bi1, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_negate(&tmp, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_add(res, &tmp, bi2, err);
		if (!rp) {
			goto ehbi_subtract_end;
		}
		rp = ehbi_negate(res, err);
		goto ehbi_subtract_end;
	}

	if ((bi1->sign == 0 && bi2->sign == 0
	     && ehbi_greater_than(bi2, bi1, err))
	    || (bi1->sign != 0
		&& bi2->sign != 0 && ehbi_less_than(bi2, bi1, err))) {
		/* subtracting a bigger number */
		negate = 1;
		swp = bi1;
		bi1 = bi2;
		bi2 = swp;
	} else {
		/* subtracting normally */
		negate = 0;
	}

	/* we don't wish to modify the real bi1, so use tmp */
	size = bi1->bytes_len;
	Ehbi_stack_alloc_struct(tmp, size, err, goto ehbi_subtract_end);
	rp = ehbi_set(&tmp, bi1, err);
	if (!rp) {
		goto ehbi_subtract_end;
	}
	bi1a = &tmp;
	res->bytes_used = 0;
	c = 0;
	for (i = 1; i <= bi1a->bytes_used; ++i) {
		if (bi1a->bytes_used < i) {
			a = 0;
		} else {
			a = bi1a->bytes[bi1a->bytes_len - i];
		}
		if ((bi2->bytes_used < i)
		    || (i > bi2->bytes_len)) {
			b = 0;
		} else {
			b = bi2->bytes[bi2->bytes_len - i];
		}
		c = (a - b);
		if (i > res->bytes_len) {
			Ehbi_log_error0("Result byte[] too small");
			ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
			rp = NULL;
			goto ehbi_subtract_end;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
		/* need to borrow */
		if (b > a) {
			c = 0x01;
			j = i + 1;
			while (c == 0x01) {
				if (j > bi1a->bytes_used) {
					Ehbi_log_error0("bytes for borrow");
					ehbi_set_error(err, EHBI_CORRUPT_DATA);
					rp = NULL;
					goto ehbi_subtract_end;
				}
				c = (bi1a->bytes[bi1a->bytes_len - j] ==
				     0x00) ? 0x01 : 0x00;
				--(bi1a->bytes[bi1a->bytes_len - j]);
				++j;
			}
			ehbi_internal_reset_bytes_used(bi1a,
						       bi1a->bytes_used
						       + 1 + j);
		}
	}

	res->sign = (negate) ? !(bi1->sign) : bi1->sign;
	if (ehbi_internal_is_zero(res)) {
		res->sign = 0;
	}
	ehbi_internal_reset_bytes_used(res, res->bytes_used + 1);
ehbi_subtract_end:
	eembed_freea(tmp.bytes);

	if (!rp && res) {
		ehbi_zero(res, err);
		return NULL;
	}

	return res;
}

struct ehbigint *ehbi_subtract_l(struct ehbigint *res,
				 const struct ehbigint *bi1, long v2, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, v2);

	return ehbi_subtract(res, bi1, &temp, err);
}

struct ehbigint *ehbi_shift_right(struct ehbigint *bi, unsigned long num_bits,
				  int *err)
{
	struct eba eba;
	int ehbi_eba_err;

	eba.endian = eba_big_endian;
	eba.bits = NULL;
	eba.size_bytes = 0;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	ehbi_eba_err = eba_shift_right(&eba, num_bits);
	if (ehbi_eba_err != 0) {
		ehbi_zero(bi, err);
		ehbi_set_error(err, EHBI_EBA_CRASH);
		return NULL;
	}
	ehbi_internal_reset_bytes_used(bi, bi->bytes_used + 1);

	return bi;
}

struct ehbigint *ehbi_shift_left(struct ehbigint *bi, unsigned long num_bits,
				 int *err)
{
	struct eba eba;
	int ehbi_eba_err;
	size_t add_size;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	eba.endian = eba_big_endian;
	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	ehbi_eba_err = eba_shift_left(&eba, num_bits);
	if (ehbi_eba_err != 0) {
		ehbi_zero(bi, err);
		ehbi_set_error(err, EHBI_EBA_CRASH);
		return NULL;
	}

	add_size = 2 + (num_bits / EEMBED_CHAR_BIT);
	ehbi_internal_reset_bytes_used(bi, bi->bytes_used + add_size);

	return bi;
}

struct ehbigint *ehbi_n_choose_k(struct ehbigint *result,
				 const struct ehbigint *n,
				 const struct ehbigint *k, int *err)
{
	size_t i, size;
	int local_error;
	struct ehbigint sum_n, sum_k, tmp;
	struct ehbigint *rp;

	Ehbi_struct_is_not_null(result, err, return NULL);
	Ehbi_struct_is_not_null(n, err, return NULL);
	Ehbi_struct_is_not_null(k, err, return NULL);

	if (!err) {
		local_error = EHBI_SUCCESS;
		err = &local_error;
	}
	rp = result;

	if (ehbi_greater_than(k, n, err) || ehbi_less_than_l(k, 0, err)) {
		rp = ehbi_set_l(result, 0, err);
		goto ehbi_n_choose_k_end;
	}

	if (ehbi_equals_l(k, 0, err) || ehbi_equals(k, n, err)) {
		rp = ehbi_set_l(result, 1, err);
		goto ehbi_n_choose_k_end;
	}

	if (ehbi_equals_l(k, 1, err)) {
		rp = ehbi_set(result, n, err);
		goto ehbi_n_choose_k_end;
	}

	if (ehbi_greater_than_l(k, LONG_MAX, err)) {
		Ehbi_log_error_s_ul_s("k larger than ", LONG_MAX, "");
		ehbi_set_error(err, EHBI_BAD_DATA);
		rp = NULL;
		goto ehbi_n_choose_k_end;
	}

	size = result->bytes_len;
	if (size < n->bytes_len) {
		size = n->bytes_len;
	}
	if (size < k->bytes_len) {
		size = k->bytes_len;
	}

	Ehbi_stack_alloc_struct(tmp, size, err, goto ehbi_n_choose_k_end);
	Ehbi_stack_alloc_struct(sum_n, size, err, goto ehbi_n_choose_k_end);
	Ehbi_stack_alloc_struct(sum_k, size, err, goto ehbi_n_choose_k_end);

	rp = ehbi_inc(&sum_n, n, err);
	if (!rp) {
		goto ehbi_n_choose_k_end;
	}
	rp = ehbi_inc(&sum_k, k, err);
	if (!rp) {
		goto ehbi_n_choose_k_end;
	}
	for (i = 1; ehbi_greater_than_l(k, i, err); ++i) {
		/* sum_n *= (n - i); */
		rp = ehbi_set_l(&tmp, -((long)i), err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_set_l(&tmp, -((long)i), err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_inc(&tmp, n, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_mul(result, &sum_n, &tmp, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_set(&sum_n, result, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}

		/* sum_k *= (k - i) */
		rp = ehbi_set_l(&tmp, -((long)i), err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_inc(&tmp, k, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_mul(result, &sum_k, &tmp, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
		rp = ehbi_set(&sum_k, result, err);
		if (!rp) {
			goto ehbi_n_choose_k_end;
		}
	}
	/* result = (sum_n / sum_k); */
	rp = ehbi_div(result, &tmp, &sum_n, &sum_k, err);

ehbi_n_choose_k_end:
	eembed_freea(tmp.bytes);
	eembed_freea(sum_n.bytes);
	eembed_freea(sum_k.bytes);

	if (!rp) {
		Ehbi_log_error_s_l_s("error ", *err, ", setting result = 0");
		ehbi_internal_zero(result);
		return NULL;
	}

	return result;
}

struct ehbigint *ehbi_n_choose_k_l(struct ehbigint *result,
				   const struct ehbigint *n, long k, int *err)
{
	unsigned char bytes[sizeof(unsigned long)];
	struct ehbigint temp;

	ehbi_internal_clear_null_struct(&temp);

	temp.bytes = bytes;
	temp.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp, k);

	return ehbi_mul(result, n, &temp, err);
}

struct ehbigint *ehbi_n_choose_k_ll(struct ehbigint *result, long n, long k,
				    int *err)
{
	unsigned char n_bytes[sizeof(unsigned long)];
	struct ehbigint temp_n;
	unsigned char k_bytes[sizeof(unsigned long)];
	struct ehbigint temp_k;

	ehbi_internal_clear_null_struct(&temp_n);
	ehbi_internal_clear_null_struct(&temp_k);

	temp_n.bytes = n_bytes;
	temp_n.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp_n, n);

	temp_k.bytes = k_bytes;
	temp_k.bytes_len = sizeof(unsigned long);
	ehbi_internal_struct_l(&temp_k, k);

	return ehbi_mul(result, &temp_n, &temp_k, err);
}

#ifndef EHBI_SKIP_IS_PROBABLY_PRIME

static struct ehbigint *ehbi_get_witness(size_t i, struct ehbigint *a,
					 struct ehbigint *max_witness, int *err)
{
	int e;
	size_t j, max_rnd, shift;
	struct ehbigint *rp;

	long Small_primes[] = {
		2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
#if EEMBED_HOSTED
		31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
		73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
		127, 131, 137, 139, 149, 151, 157, 163, 167,
		173, 179, 181, 191, 193, 197, 199, 211, 223,
		227, 229, 233, 239, 241, 251, 257, 263, 269,
		271, 277, 281, 283, 293, 307, 311, 313, 317,
		331, 337, 347, 349, 353, 359, 367, 373, 379,
		383, 389, 397, 401, 409, 419, 421, 431, 433,
		439, 443, 449, 457, 461, 463, 467, 479, 487,
		491, 499, 503, 509, 521, 523, 541, 547, 557,
		563, 569, 571, 577, 587, 593, 599, 601, 607,
		613, 617, 619, 631, 641, 643, 647, 653, 659,
		661, 673, 677, 683, 691, 701, 709, 719, 727,
		733, 739, 743, 751, 757, 761, 769, 773, 787,
		797, 809, 811, 821, 823, 827, 829, 839, 853,
		857, 859, 863, 877, 881, 883, 887, 907, 911,
		919, 929, 937, 941, 947, 953, 967, 971, 977,
#endif
		983, 991, 997,
		0		/* ZERO terminated */
	};

	size_t Small_primes_len = (sizeof(Small_primes) / sizeof(long)) - 1;

	rp = a;
	if (i < EHBI_NUM_SMALL_PRIME_WITNESSES && i < Small_primes_len) {
		rp = ehbi_set_l(a, Small_primes[i], err);
	} else {
		j = 0;
		max_rnd = EHBI_MAX_TRIES_TO_GRAB_RANDOM_BYTES;
		/* pick a random integer a in the range [2, n-2] */
		do {
			e = eembed_random_bytes(a->bytes, a->bytes_len);
			if (e) {
				Ehbi_log_error_s_l_s
				    ("eembed_random_bytes returned error ", e,
				     ", continuing with poor confidence!");
				ehbi_set_error(err, EHBI_PRNG_ERROR);
			}
			a->bytes_used = a->bytes_len;
			shift = a->bytes_len - max_witness->bytes_used;
			rp = ehbi_shift_right(a, shift * EEMBED_CHAR_BIT, err);
			ehbi_internal_reset_bytes_used(a, a->bytes_used + 1);
		} while ((ehbi_greater_than(a, max_witness, err)
			  || ehbi_less_than_l(a, 2, err)) && (j++ < max_rnd));
	}
	if (ehbi_greater_than(a, max_witness, err)
	    || ehbi_less_than_l(a, 2, err)) {
		/* but, too big, so do something totally bogus: */
		rp = ehbi_set_l(a, 2 + i, err);
	}
	return rp;
}

/*
   From:
   https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test

   The algorithm can be written in pseudocode as follows:

	Input #1: n > 3, an odd integer to be tested for primality;
	Input #2: k, a parameter that determines the accuracy of the test
	Output: composite if n is composite, otherwise probably prime

	write n-1 as (2^r)*d with d odd by factoring powers of 2 from n-1
	WitnessLoop: repeat k times:
		pick a random integer a in the range [2, n-2]
		x := (a^d) mod n
		if x = 1 or x = n-1 then
			continue WitnessLoop
		repeat r-1 times:
			x := (x^2) mod n
			if x = 1 then
				return composite
			if x = n-1 then
				continue WitnessLoop
		return composite
	return probably prime
*/
int ehbi_is_probably_prime(const struct ehbigint *bi, unsigned int accuracy,
			   int *err)
{
	size_t i, k, size, size2;
	int is_probably_prime, stop, local_err;
	struct ehbigint bimin1, a, d, x, y, max_witness;
	struct ehbigint *rp;
	unsigned long r, c;

	ehbi_internal_clear_null_struct(&a);
	ehbi_internal_clear_null_struct(&d);
	ehbi_internal_clear_null_struct(&x);
	ehbi_internal_clear_null_struct(&y);
	ehbi_internal_clear_null_struct(&max_witness);

	Ehbi_struct_is_not_null(bi, err, return 0);

	if (!err) {
		local_err = EHBI_SUCCESS;
		err = &local_err;
	}

	is_probably_prime = 0;
	if (ehbi_is_negative(bi, err)) {
		return 0;
	}

	size = bi->bytes_used;
	if (size < 4) {
		size = 4;
	}
	Ehbi_stack_alloc_struct(bimin1, size, err,
				goto ehbi_is_probably_prime_end);
	Ehbi_stack_alloc_struct(max_witness, size, err,
				goto ehbi_is_probably_prime_end);
	size2 = 2 + (size * 2);
	Ehbi_stack_alloc_struct(a, size, err, goto ehbi_is_probably_prime_end);
	Ehbi_stack_alloc_struct(d, size, err, goto ehbi_is_probably_prime_end);
	Ehbi_stack_alloc_struct(x, size2, err, goto ehbi_is_probably_prime_end);
	Ehbi_stack_alloc_struct(y, size2, err, goto ehbi_is_probably_prime_end);
	/* set d to 2, the first prime */
	rp = ehbi_set_l(&d, 2, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}

	if (ehbi_less_than(bi, &d, err)) {
		is_probably_prime = 0;
		goto ehbi_is_probably_prime_end;
	}

	/* 2 is the only even prime */
	if (!ehbi_is_odd(bi, err)) {
		is_probably_prime = ehbi_equals(bi, &d, err);
		goto ehbi_is_probably_prime_end;
	}

	is_probably_prime = 1;
	/*
	   write n-1 as 2^r * d;
	   with d odd by factoring powers of 2 from n-1
	 */
	rp = ehbi_subtract_l(&d, bi, 1, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}
	/* d is now bi-1 */
	for (i = 0; 1; ++i) {
		if (ehbi_is_odd(&d, err)) {
			break;
		}
		rp = ehbi_shift_right(&d, 1, err);
		if (!rp) {
			goto ehbi_is_probably_prime_end;
		}
	}
	r = i;

	/* (bi-1) == 2^(r) * d */
	rp = ehbi_set(&bimin1, bi, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}
	rp = ehbi_dec_l(&bimin1, 1, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}

	/* we will set max_witness at n-2 */
	rp = ehbi_set(&max_witness, bi, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}
	rp = ehbi_dec_l(&max_witness, 2, err);
	if (!rp) {
		goto ehbi_is_probably_prime_end;
	}

	if (accuracy == 0) {
		k = EHBI_DEFAULT_TRIALS_FOR_IS_PROBABLY_PRIME;
	} else if (accuracy < EHBI_MIN_TRIALS_FOR_IS_PROBABLY_PRIME) {
		k = EHBI_MIN_TRIALS_FOR_IS_PROBABLY_PRIME;
	} else {
		k = accuracy;
	}

	/*
	   WitnessLoop: repeat k times:
	 */
	for (i = 0; i < k; ++i) {
		rp = ehbi_get_witness(i, &a, &max_witness, err);
		if (!rp) {
			goto ehbi_is_probably_prime_end;
		}

		/* still too big, we are done */
		if (ehbi_greater_than(&a, &max_witness, err)) {
			is_probably_prime = 1;
			goto ehbi_is_probably_prime_end;
		}

		/* x := a^d mod n */
		rp = ehbi_exp_mod(&x, &a, &d, bi, err);
		if (!rp) {
			goto ehbi_is_probably_prime_end;
		}

		/* if x == 1 or x == n-1 then continue WitnessLoop */
		if (ehbi_equals_l(&x, 1, err)) {
			continue;
		}
		if (ehbi_equals(&x, &bimin1, err)) {
			continue;
		}

		/* repeat r-1 times: */
		stop = 0;
		for (c = r - 1; !stop && c > 0; --c) {
			/* x := x^2 mod n */
			rp = ehbi_set(&y, &x, err);
			if (!rp) {
				goto ehbi_is_probably_prime_end;
			} else {
				struct ehbigint exp;
				unsigned char exp_bytes[sizeof(unsigned long)];

				ehbi_internal_clear_null_struct(&exp);
				exp.bytes = exp_bytes;
				ehbi_internal_struct_l(&exp, 2);
				rp = ehbi_exp_mod(&x, &y, &exp, bi, err);
				if (!rp) {
					goto ehbi_is_probably_prime_end;
				}
			}

			/* if x == 1 then return composite */
			if (ehbi_equals_l(&x, 1, err)) {
				is_probably_prime = 0;
				goto ehbi_is_probably_prime_end;
			}

			/* if x == n-1 then continue WitnessLoop */
			if (ehbi_equals(&x, &bimin1, err)) {
				stop = 1;
				break;
			}
		}
		if (!stop) {
			is_probably_prime = 0;
			goto ehbi_is_probably_prime_end;
		}
	}

	/* return probably prime */
	is_probably_prime = 1;

ehbi_is_probably_prime_end:
	if (!rp) {
		Ehbi_log_error_s_l_s("error ", *err,
				     ", setting is_probably_prime = 0");
		is_probably_prime = 0;
	}

	eembed_freea(bimin1.bytes);
	eembed_freea(a.bytes);
	eembed_freea(r.bytes);
	eembed_freea(d.bytes);
	eembed_freea(x.bytes);
	eembed_freea(y.bytes);
	eembed_freea(c.bytes);
	eembed_freea(max_witness.bytes);

	return is_probably_prime;
}

#endif /* EHBI_SKIP_IS_PROBABLY_PRIME */

struct ehbigint *ehbi_negate(struct ehbigint *bi, int *err)
{
	Ehbi_struct_is_not_null(bi, err, return NULL);

	bi->sign = (bi->sign == 0) ? 1 : 0;

	return bi;
}

int ehbi_is_negative(const struct ehbigint *bi, int *err)
{
	Ehbi_struct_is_not_null(bi, err, return 0);

	if (ehbi_internal_is_zero(bi)) {
		if (bi->sign == 1) {
			Ehbi_log_error0("Negative zero?");
			return 0;
		}
	}

	return (bi->sign == 0) ? 0 : 1;
}

int ehbi_compare(const struct ehbigint *bi1, const struct ehbigint *bi2,
		 int *err)
{
	size_t i;
	unsigned char a, b;
	int rv, b1_pos, b2_pos;

	rv = 0;

	Ehbi_struct_is_not_null(bi1, err, goto ehbi_compare_end);
	Ehbi_struct_is_not_null(bi2, err, goto ehbi_compare_end);

	b1_pos = !ehbi_is_negative(bi1, err);
	b2_pos = !ehbi_is_negative(bi2, err);

	if (b1_pos != b2_pos) {
		rv = b1_pos ? 1 : -1;
		goto ehbi_compare_end;
	}

	if (bi1->bytes_used > bi2->bytes_used) {
		rv = b1_pos ? 1 : -1;
		return rv;
	} else if (bi1->bytes_used < bi2->bytes_used) {
		rv = b1_pos ? -1 : 1;
		return rv;
	}

	for (i = 0; i < bi1->bytes_used; ++i) {
		a = bi1->bytes[(bi1->bytes_len - bi1->bytes_used) + i];
		b = bi2->bytes[(bi2->bytes_len - bi2->bytes_used) + i];
		if (a > b) {
			rv = b1_pos ? 1 : -1;
			return rv;
		} else if (a < b) {
			rv = b1_pos ? -1 : 1;
			return rv;
		}
	}

ehbi_compare_end:
	return rv;
}

int ehbi_compare_l(const struct ehbigint *bi1, long i2, int *err)
{
	struct ehbigint bi2;
	unsigned char bytes[sizeof(long)];

	ehbi_internal_clear_null_struct(&bi2);

	ehbi_init_l(&bi2, bytes, sizeof(long), i2, err);

	return ehbi_compare(bi1, &bi2, err);
}

#if (0)
struct eembed_log *ehbi_log(struct eembed_log *log, const struct ehbigint *bi,
			    int *err)
{
	char *buf = NULL;
	size_t size = (3 + (bi->bytes_len * 3));
	buf = (char *)eembed_alloca(size);
	if (buf) {
		ehbi_to_decimal_string(bi, buf, size, &err);
		log->append_s(log, buf);
	} else {
		log->append_s(log, "XXX");
	}
	eembed_freea(buf);
	return log;
}
#endif

int ehbi_equals(const struct ehbigint *bi1, const struct ehbigint *bi2,
		int *err)
{
	int rv = 0;

	Ehbi_struct_is_not_null(bi1, err, return 0);
	Ehbi_struct_is_not_null(bi2, err, return 0);

	rv = (ehbi_compare(bi1, bi2, err) == 0);

	return rv;
}

int ehbi_equals_l(const struct ehbigint *bi1, long i2, int *err)
{
	struct ehbigint bi2;
	unsigned char bytes[sizeof(long)];

	ehbi_internal_clear_null_struct(&bi2);

	ehbi_init_l(&bi2, bytes, sizeof(long), i2, err);

	return ehbi_equals(bi1, &bi2, err);
}

int ehbi_less_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		   int *err)
{
	Ehbi_struct_is_not_null(bi1, err, return 0);
	Ehbi_struct_is_not_null(bi2, err, return 0);

	return (ehbi_compare(bi1, bi2, err) < 0);
}

int ehbi_less_than_l(const struct ehbigint *bi1, long i2, int *err)
{
	struct ehbigint bi2;
	unsigned char bytes[sizeof(long)];

	ehbi_internal_clear_null_struct(&bi2);
	ehbi_init_l(&bi2, bytes, sizeof(long), i2, err);

	return ehbi_less_than(bi1, &bi2, err);
}

int ehbi_greater_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		      int *err)
{
	Ehbi_struct_is_not_null(bi1, err, return 0);
	Ehbi_struct_is_not_null(bi2, err, return 0);

	return (ehbi_compare(bi1, bi2, err) > 0);
}

int ehbi_greater_than_l(const struct ehbigint *bi1, long i2, int *err)
{
	struct ehbigint bi2;
	unsigned char bytes[sizeof(long)];

	ehbi_internal_clear_null_struct(&bi2);
	ehbi_init_l(&bi2, bytes, sizeof(long), i2, err);

	return ehbi_greater_than(bi1, &bi2, err);
}

int ehbi_is_odd(const struct ehbigint *bi, int *err)
{
	unsigned char bit;

	Ehbi_struct_is_not_null(bi, err, return 0);

	bit = 0x01 & bi->bytes[bi->bytes_len - 1];

	return bit ? 1 : 0;
}

struct ehbigint *ehbi_alloc_l(size_t bytes_len, long val, int *err)
{
	struct ehbigint *bi = NULL;
	unsigned char *bytes = NULL;
	struct eembed_allocator *ea = eembed_global_allocator;
	struct ehbigint *rp = NULL;
	size_t size = 0;

	size = sizeof(struct ehbigint);
	bi = (struct ehbigint *)ea->calloc(ea, 1, size);
	if (!bi) {
		Ehbi_log_error_s_ul_s("could not allocate ", size, " bytes?");
		ehbi_set_error(err, EHBI_NULL_STRUCT);
		goto ehbi_alloc_l_end;
	}

	size = bytes_len;
	bytes = (unsigned char *)ea->calloc(ea, 1, size);
	if (!bytes) {
		Ehbi_log_error_s_ul_s("could not allocate ", size, " bytes?");
		ehbi_set_error(err, EHBI_NULL_BYTES);
		goto ehbi_alloc_l_end;
	}

	rp = ehbi_init_l(bi, bytes, bytes_len, val, err);
	if (!rp) {
		Ehbi_log_error_s_l_s("error ", *err, " from ehbi_init?");
		goto ehbi_alloc_l_end;
	}

ehbi_alloc_l_end:
	if (!rp) {
		ehbi_free(bi);
		return NULL;
	}

	return bi;
}

struct ehbigint *ehbi_alloc(size_t bytes_len, int *err)
{
	return ehbi_alloc_l(bytes_len, 0, err);
}

void ehbi_free(struct ehbigint *bi)
{
	struct eembed_allocator *ea = eembed_global_allocator;
	if (bi) {
		if (bi->bytes) {
			ea->free(ea, bi->bytes);
		}
		ea->free(ea, bi);
	}
}

static char *ehbi_decimal_from_hex(char *buf, size_t buf_len, const char *hex,
				   size_t hex_len, int *err);

static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len);

static char ehbi_nibble_to_hex(unsigned char nibble, int *err);

static unsigned char ehbi_from_hex_nibble(char c, int *err);

/* public functions */
struct ehbigint *ehbi_set_binary_string(struct ehbigint *bi, const char *str,
					size_t len, int *err)
{
	size_t i, j;
	struct eba eba;

	eba.endian = eba_big_endian;
	eba.bits = NULL;
	eba.size_bytes = 0;

	Ehbi_struct_is_not_null(bi, err, return NULL);
	ehbi_zero(bi, err);

	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	if (str == 0) {
		Ehbi_log_error0("Null string");
		ehbi_set_error(err, EHBI_NULL_STRING);
		return NULL;
	}
	if (len == 0 || str[0] == 0) {
		Ehbi_log_error0("Zero length string");
		ehbi_set_error(err, EHBI_ZERO_LEN_STRING);
		return NULL;
	}
	if (len > 2 && str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) {
		str = str + 2;
		len -= 2;
	}
	len = eembed_strnlen(str, len);
	for (i = 0; i < len; ++i) {
		if (str[i] != '0' && str[i] != '1') {
			len = i;
		}
	}
	if (len > (bi->bytes_len * EEMBED_CHAR_BIT)) {
		ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
		return NULL;
	}

	for (i = 0, j = len - 1; i < len; ++i, --j) {
		eba_set(&eba, i, str[j] == '1' ? 1 : 0);
	}

	ehbi_internal_reset_bytes_used(bi, 2 + (len / 8));

	return bi;
}

struct ehbigint *ehbi_set_hex_string(struct ehbigint *bi, const char *str,
				     size_t str_len, int *err)
{
	size_t i, j;
	unsigned char high, low;
	int local_err;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	if (str == 0) {
		Ehbi_log_error0("Null string");
		ehbi_set_error(err, EHBI_NULL_STRING);
		return NULL;
	}
	if (str_len == 0 || str[0] == 0) {
		Ehbi_log_error0("Zero length string");
		ehbi_set_error(err, EHBI_ZERO_LEN_STRING);
		return NULL;
	}

	bi->sign = 0;

	/* ignore characters starting with the first NULL in string */
	for (i = 1; i < str_len; ++i) {
		if (str[i] == 0) {
			str_len = i;
			break;
		}
	}

	/* skip over leading '0x' in string */
	if (str_len >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
		str_len -= 2;
	}

	j = str_len;
	i = bi->bytes_len;

	if (!err) {
		local_err = EHBI_SUCCESS;
		err = &local_err;
	}

	bi->bytes_used = 0;
	while (j > 0) {
		low = str[--j];
		if (j > 0) {
			high = str[--j];
		} else {
			high = '0';
		}
		if (bi->bytes_used >= bi->bytes_len) {
			Ehbi_log_error0("byte[] too small");
			ehbi_set_error(err, EHBI_BYTES_TOO_SMALL);
			return NULL;
		}
		bi->bytes[--i] = ehbi_hex_chars_to_byte(high, low, err);
		if (*err) {
			Ehbi_log_error_s_c_s_c_s("Bad data (high: ", high,
						 " low: ", low, ")");
			ehbi_set_error(err, EHBI_BAD_DATA);
			return NULL;
		}
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for easier debug */
	eembed_memset(bi->bytes, 0x00, i);

	ehbi_internal_reset_bytes_used(bi, bi->bytes_used + 1);

	return bi;
}

struct ehbigint *ehbi_set_decimal_string(struct ehbigint *bi, const char *dec,
					 size_t len, int *err)
{
	char *hex;
	const char *str;
	size_t size;
	int negative;
	struct ehbigint *rp;
	int local_err;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	if (len == 0) {
		str = "0x00";
		len = 4;
	} else if (dec == NULL) {
		Ehbi_log_error0("Null string");
		ehbi_set_error(err, EHBI_NULL_STRING);
		return NULL;
	}
	size = 4 /* strlen("0x00") */  + len + 1;
	hex = (char *)eembed_alloca(size);
	if (!hex) {
		Ehbi_log_error_s_ul_s("Could not allocate ", size, " bytes?");
		ehbi_set_error(err, EHBI_STACK_TOO_SMALL);
		return NULL;
	}

	if (dec[0] == '-') {
		str = dec + 1;
		len -= 1;
		negative = 1;
	} else {
		str = dec;
		negative = 0;
	}
	if (!err) {
		err = &local_err;
	}
	*err = ehbi_decimal_to_hex(str, len, hex, size);
	if (*err) {
		rp = NULL;
		goto ehbi_set_decimal_string_end;
	}

	rp = ehbi_set_hex_string(bi, hex, size, err);
	if (!rp) {
		goto ehbi_set_decimal_string_end;
	}

	if (negative) {
		rp = ehbi_negate(bi, err);
	}

ehbi_set_decimal_string_end:
	eembed_freea(hex);
	if (!rp) {
		ehbi_internal_zero(bi);
		return NULL;
	}
	return bi;
}

char *ehbi_to_binary_string(const struct ehbigint *bi, char *buf,
			    size_t buf_len, int *err)
{
	size_t i, j, k, written;
	unsigned char bit;
	struct eba eba;

	eba.endian = eba_big_endian;
	eba.bits = NULL;
	eba.size_bytes = 0;

	Ehbi_struct_is_not_null(bi, err, goto ehbi_to_binary_string_end);

	eba.bits = bi->bytes;
	eba.size_bytes = bi->bytes_len;

	written = 0;

	if (buf && buf_len) {
		eembed_memset(buf, 0x00, buf_len);
	}

	if (buf == 0) {
		Ehbi_log_error0("Null buffer");
		ehbi_set_error(err, EHBI_NULL_STRING_BUF);
		goto ehbi_to_binary_string_end;
	}

	if (buf_len < ((bi->bytes_used * EEMBED_CHAR_BIT) + 3)) {
		Ehbi_log_error0("Buffer too small");
		ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL);
		goto ehbi_to_binary_string_end;
	}

	buf[written++] = '0';
	buf[written++] = 'b';

	for (i = 0; i < bi->bytes_used; ++i) {
		for (j = 0; j < EEMBED_CHAR_BIT; ++j) {
			if ((written + 2) >= buf_len) {
				Ehbi_log_error0("Buffer too small");
				ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL);
				goto ehbi_to_binary_string_end;
			}
			k = (bi->bytes_used * EEMBED_CHAR_BIT) - 1;
			k = k - ((i * EEMBED_CHAR_BIT) + j);
			bit = eba_get(&eba, k);
			buf[written++] = bit ? '1' : '0';
		}
	}
	buf[written] = '\0';

ehbi_to_binary_string_end:
	if (*err) {
		buf[0] = '\0';
	}
	return buf;
}

char *ehbi_to_hex_string(const struct ehbigint *bi, char *buf, size_t buf_len,
			 int *err)
{
	int local_err;
	size_t i, j;
	char *rp;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	if (buf == 0) {
		Ehbi_log_error0("Null buffer");
		ehbi_set_error(err, EHBI_NULL_STRING_BUF);
		goto ehbi_to_hex_string_end;
	}
	buf[0] = '\0';

	if (buf_len < (bi->bytes_used + 3)) {
		Ehbi_log_error0("Buffer too small");
		ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL);
		goto ehbi_to_hex_string_end;
	}

	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			Ehbi_log_error0("Buffer too small, partially written");
			ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL_PARTIAL);
			goto ehbi_to_hex_string_end;
		}
		local_err = 0;
		ehbi_hex_chars_from_byte(buf + j, buf + j + 1, bi->bytes[i],
					 &local_err);
		if (local_err) {
			Ehbi_log_error0("Corrupted data?");
			ehbi_set_error(err, local_err);
			goto ehbi_to_hex_string_end;
		}
		j += 2;
	}
	if (j > buf_len) {
		Ehbi_log_error0("Unable to write trailing NULL to buffer");
		ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL_NO_NULL);
		goto ehbi_to_hex_string_end;
	}
	buf[j] = '\0';

	/* strip leading '0's ("0x0123" -> "0x123") */
	/* strip leading "00"s ("0x000123" -> "0x0123") */
	while ((buf[2] == '0' || buf[2] == 'F') && buf[2] == buf[3]
	       && buf[2] == buf[4] && buf[2] == buf[5]) {
		for (j = 2; j < buf_len - 1 && buf[j] != 0; j += 2) {
			buf[j] = buf[j + 2];
		}
	}

ehbi_to_hex_string_end:
	if (buf && (err == NULL || *err)) {
		buf[0] = '\0';
	}
	rp = ((err == NULL || *err) ? NULL : buf);

	return rp;
}

char *ehbi_to_decimal_string(const struct ehbigint *bi, char *buf, size_t len,
			     int *err)
{
	char *hex, *rp;
	size_t size;

	hex = NULL;
	rp = NULL;

	Ehbi_struct_is_not_null(bi, err, return NULL);

	size = 0;

	if (buf == NULL || len == 0) {
		Ehbi_log_error0("Null Arguments(s)");
		if (err) {
			ehbi_set_error(err, EHBI_NULL_ARGS);
		}
		goto ehbi_to_decimal_string_end;
	}
	rp = buf;
	buf[0] = '\0';

	size = 4 /* strlen("0x00") */  + (2 * bi->bytes_used) + 1;
	hex = (char *)eembed_alloca(size);
	if (!hex) {
		Ehbi_log_error_s_ul_s("Could not allocate ", size, " bytes?");
		ehbi_set_error(err, EHBI_STACK_TOO_SMALL);
		goto ehbi_to_decimal_string_end;
	}

	if (ehbi_is_negative(bi, err)) {
		buf[0] = '-';
		buf[1] = '\0';
		buf = buf + 1;
		len -= 1;
	}

	ehbi_to_hex_string(bi, hex, size, err);
	if (*err) {
		goto ehbi_to_decimal_string_end;
	}
	ehbi_decimal_from_hex(buf, len, hex, size, err);

ehbi_to_decimal_string_end:
	eembed_freea(hex);
	if (buf && (err == NULL || *err)) {
		buf[0] = '\0';
	}

	rp = (err == NULL || *err) ? NULL : rp;

	return rp;
}

static char *ehbi_hex_chars_from_byte(char *high, char *low, unsigned char byte,
				      int *err)
{
	int local_err;

	local_err = 0;
	*high = ehbi_nibble_to_hex((byte & 0xF0) >> 4, &local_err);
	if (local_err) {
		ehbi_set_error(err, local_err);
	}
	*low = ehbi_nibble_to_hex((byte & 0x0F), &local_err);
	if (local_err) {
		ehbi_set_error(err, local_err);
	}

	return local_err ? NULL : high;
}

static unsigned char ehbi_hex_chars_to_byte(char high, char low, int *err)
{
	int local_err;
	unsigned char nibble;
	unsigned char byte;

	local_err = 0;
	nibble = ehbi_from_hex_nibble(high, &local_err);
	if (local_err) {
		Ehbi_log_error_s_c_s("Error with high nibble (", high, ")");
		ehbi_set_error(err, EHBI_BAD_HIGH_NIBBLE);
		return 0;
	}
	byte = (nibble << 4);

	nibble = ehbi_from_hex_nibble(low, &local_err);
	if (local_err) {
		Ehbi_log_error_s_c_s("Error with low nibble (", low, ")");
		ehbi_set_error(err, EHBI_BAD_LOW_NIBBLE);
		return 0;
	}
	byte += nibble;

	return byte;
}

/* private functions */
static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len)
{
	char *rp;

	if (dec_str == 0 || buf == 0) {
		Ehbi_log_error0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 5) {
		Ehbi_log_error0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	rp = decimal_to_hex(dec_str, dec_len, buf, buf_len);

	if (rp == NULL) {
		Ehbi_log_error_s_s_s("Character not decimal? (", dec_str, ")");
		return EHBI_BAD_INPUT;
	}

	return EHBI_SUCCESS;
}

static char *ehbi_decimal_from_hex(char *buf, size_t buf_len, const char *hex,
				   size_t hex_len, int *err)
{
	char *rp;

	if (hex == 0 || buf == 0) {
		Ehbi_log_error0("Null argument");
		ehbi_set_error(err, EHBI_NULL_ARGS);
		return NULL;
	}
	if (buf_len) {
		buf[0] = '\0';
	}

	if (buf_len < 2 || buf_len < hex_len) {
		Ehbi_log_error0("Buffer too small");
		ehbi_set_error(err, EHBI_STRING_BUF_TOO_SMALL);
		return NULL;
	}

	rp = hex_to_decimal(hex, hex_len, buf, buf_len);

	if (rp == NULL) {
		Ehbi_log_error_s_s_s("Character not hex? (", hex, ")");
		ehbi_set_error(err, EHBI_BAD_INPUT);
	}

	return buf;
}

static char ehbi_nibble_to_hex(unsigned char nibble, int *err)
{
	char buf[25];

	if (nibble < 10) {
		return '0' + nibble;
	}
	if (nibble < 16) {
		return 'A' + nibble - 10;
	}

	eembed_ulong_to_hex(buf, 25, nibble);
	Ehbi_log_error_s_s_s("Bad input '", buf, "'");
	ehbi_set_error(err, EHBI_BAD_INPUT);
	return 0;
}

static unsigned char ehbi_from_hex_nibble(char c, int *err)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}

	if (c >= 'a' && c <= 'f') {
		return 10 + c - 'a';
	}
	if (c >= 'A' && c <= 'F') {
		return 10 + c - 'A';
	}

	Ehbi_log_error_s_c_s("Not hex (", c, ")");
	ehbi_set_error(err, EHBI_NOT_HEX);

	return 0;
}

static void ehbi_internal_reset_bytes_used(struct ehbigint *bi, size_t from)
{
	size_t i;

	if (from > bi->bytes_len) {
		from = bi->bytes_len;
	}
	for (i = (bi->bytes_len - from); i < bi->bytes_len; ++i) {
		if (bi->bytes[i] != 0) {
			break;
		}
	}
	bi->bytes_used = (bi->bytes_len - i);
	if (bi->bytes_used == 0) {
		bi->bytes_used = 1;
	}

	if (ehbi_internal_is_zero(bi)) {
		bi->sign = 0;
	}
}
