/* _POSIX_C_SOURCE needed for backtrace_symbols_fd */
#ifndef _POSIX_C_SOURCE
#ifdef EHBI_ENSURE_POSIX
#define _POSIX_C_SOURCE EHBI_ENSURE_POSIX
#endif /* EHBI_ENSURE_POSIX */
#endif /* _POSIX_C_SOURCE */

#include "ehbigint.h"

#include <string.h>
#include <execinfo.h>
#include <stdarg.h>
#include <ehstr.h>

#ifdef EHBI_NO_ALLOCA
static void ehbi_do_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		EHBI_LOG_ERROR2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
	free(ptr);
}

#define ehbi_stack_alloc malloc
#define ehbi_stack_alloc_str "malloc"
#define ehbi_stack_free ehbi_do_stack_free
#else
#include <alloca.h>
static void ehbi_no_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		EHBI_LOG_ERROR2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
}

#define ehbi_stack_alloc alloca
#define ehbi_stack_alloc_str "alloca"
#define ehbi_stack_free ehbi_no_stack_free
#endif

int ehbi_debug_log_level = 0;

int debugf(int level, const char *fmt, ...)
{
	va_list ap;
	int r;

	if (ehbi_debug_log_level < level) {
		return 0;
	}

	va_start(ap, fmt);
	r = vfprintf(stderr, fmt, ap);
	va_end(ap);
	return r;
}

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len);

static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len);

static int nibble_to_hex(unsigned char nibble, char *c)
{
	if (c == 0) {
		EHBI_LOG_ERROR0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		EHBI_LOG_ERROR1("Bad input '%x'", nibble);
		return EHBI_BAD_INPUT;
	}
	return EHBI_SUCCESS;
}

static int to_hex(unsigned char byte, char *high, char *low)
{
	int err;

	err = 0;
	err += nibble_to_hex((byte & 0xF0) >> 4, high);
	err += nibble_to_hex((byte & 0x0F), low);

	return err;
}

void ehbi_debug_to_string(int level, struct ehbigint *bi, const char *name)
{
	char *buf, h, l;
	size_t size, i;

	if (ehbi_debug_log_level < level) {
		return;
	}

	fprintf(stderr,
		"%s (%p) => {\n\tbytes => (%p),\n" "\tbytes_len => %lu,\n"
		"\tbytes_used => %lu,\n", name, (void *)bi,
		(void *)bi->bytes, (unsigned long)bi->bytes_len,
		(unsigned long)bi->bytes_used);

	fprintf(stderr, "\tused  => ");
	for (i = bi->bytes_len; i > 0; --i) {
		fprintf(stderr, "%s", i > bi->bytes_used ? "XX" : "__");
	}
	fprintf(stderr, ",\n");

	fprintf(stderr, "\tbytes => ");
	for (i = bi->bytes_len; i > 0; --i) {
		h = '?';
		l = '?';
		to_hex(bi->bytes[bi->bytes_len - i], &h, &l);
		fprintf(stderr, "%c%c", h, l);
	}
	fprintf(stderr, ",\n");

	size = 5 + (4 * bi->bytes_used);
	buf = ehbi_stack_alloc(size);
	if (!buf) {
		EHBI_LOG_ERROR2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\thex => ");
	for (i = 0; i < (bi->bytes_len - bi->bytes_used); ++i) {
		fprintf(stderr, "  ");
	}
	ehbi_to_hex_string(bi, buf, size);
	fprintf(stderr, "%s,\n", buf);

	ehbi_to_decimal_string(bi, buf, size);
	fprintf(stderr, "\tdec => %s,\n", buf);
	ehbi_stack_free(buf, size);

	fprintf(stderr, "}\n");
}

static int from_hex_nibble(unsigned char *nibble, char c)
{

	if (nibble == 0) {
		EHBI_LOG_ERROR0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		EHBI_LOG_ERROR1("Not hex (%c)", c);
		return EHBI_NOT_HEX;
	}

	return EHBI_SUCCESS;
}

static int from_hex(unsigned char *byte, char high, char low)
{
	int err;
	unsigned char nibble;

	err = from_hex_nibble(&nibble, high);
	if (err) {
		EHBI_LOG_ERROR1("Error with high nibble (%c)", high);
		return EHBI_BAD_HIGH_NIBBLE;
	}
	*byte = (nibble << 4);

	err = from_hex_nibble(&nibble, low);
	if (err) {
		EHBI_LOG_ERROR1("Error with low nibble (%c)", high);
		return EHBI_BAD_LOW_NIBBLE;
	}
	*byte += nibble;

	return EHBI_SUCCESS;
}

int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
{
	size_t i, j;
	unsigned char high, low;

	if (bi == 0) {
		EHBI_LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		EHBI_LOG_ERROR0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}
	if (str == 0) {
		EHBI_LOG_ERROR0("Null string");
		return EHBI_NULL_STRING;
	}
	if (str_len == 0 || str[0] == 0) {
		EHBI_LOG_ERROR0("Zero length string");
		return EHBI_ZERO_LEN_STRING;
	}

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

	bi->bytes_used = 0;
	while (j > 0) {
		low = str[--j];
		if (j > 0) {
			high = str[--j];
		} else {
			high = '0';
		}
		if (bi->bytes_used >= bi->bytes_len) {
			EHBI_LOG_ERROR0("byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		if (from_hex(&(bi->bytes[--i]), high, low)) {
			EHBI_LOG_ERROR2("Bad data (high: %c, low: %c)", high,
					low);
			return EHBI_BAD_DATA;
		}
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for easier debug */
	high = ((i < bi->bytes_len) && (bi->bytes[i] > 0x7F)) ? 0xFF : 0x00;
	while (i-- > 0) {
		bi->bytes[i] = high;
	}

	return EHBI_SUCCESS;
}

int ehbi_from_decimal_string(struct ehbigint *bi, const char *dec, size_t len)
{
	char *hex;
	size_t size;
	int err;

	size = strlen("0x00") + len + 1;
	hex = ehbi_stack_alloc(size);
	if (!hex) {
		EHBI_LOG_ERROR2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		return EHBI_STACK_TOO_SMALL;
	}
	err = ehbi_decimal_to_hex(dec, len, hex, size);
	if (err) {
		return err;
	}
	err = ehbi_from_hex_string(bi, hex, size);
	ehbi_stack_free(hex, size);
	return err;
}

static int ehbi_zero(struct ehbigint *bi)
{
	size_t i;
	if (bi == 0) {
		EHBI_LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		EHBI_LOG_ERROR0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}

	for (i = 0; i < bi->bytes_len; ++i) {
		bi->bytes[i] = 0x00;
	}
	bi->bytes_used = 1;

	return EHBI_SUCCESS;
}

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

int ehbi_to_hex_string(const struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		EHBI_LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (buf == 0) {
		EHBI_LOG_ERROR0("Null buffer");
		return EHBI_NULL_STRING_BUF;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			EHBI_LOG_ERROR0("Buffer too small, partially written");
			return EHBI_STRING_BUF_TOO_SMALL_PARTIAL;
		}
		err = to_hex(bi->bytes[i], buf + j, buf + j + 1);
		if (err) {
			EHBI_LOG_ERROR0("Corrupted data?");
			return err;
		}
		j += 2;
	}
	if (j > buf_len) {
		EHBI_LOG_ERROR0("Unable to write trailing NULL to buffer");
		return EHBI_STRING_BUF_TOO_SMALL_NO_NULL;
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

	return EHBI_SUCCESS;
}

int ehbi_to_decimal_string(const struct ehbigint *bi, char *buf, size_t len)
{
	char *hex;
	size_t size;
	int err;

	size = strlen("0x00") + (2 * bi->bytes_used) + 1;
	hex = ehbi_stack_alloc(size);
	if (!hex) {
		EHBI_LOG_ERROR2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		return EHBI_STACK_TOO_SMALL;
	}
	err = ehbi_to_hex_string(bi, hex, size);
	if (err) {
		return err;
	}
	err = ehbi_hex_to_decimal(hex, size, buf, len);
	ehbi_stack_free(hex, size);
	return err;
}

int ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2)
{
	size_t i;
	unsigned char a, b, c;
	const struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
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
			EHBI_LOG_ERROR0("Result byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c < a) || (c == a && b != 0) ? 1 : 0;
	}
	if (c) {
		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;
		if (c == 0xFF) {
			if (res->bytes_used == res->bytes_len) {
				EHBI_LOG_ERROR0
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
		EHBI_LOG_ERROR0("Null argument(s)");
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
		EHBI_LOG_ERROR2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
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

	if (quotient == 0 || remainder == 0 || numerator == 0
	    || denominator == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}

	if (remainder->bytes_len < numerator->bytes_len) {
		EHBI_LOG_ERROR0("byte[] too small");
		return EHBI_BYTES_TOO_SMALL;
	}

	ehbi_zero(quotient);

	/* Smarter would be to do a long-division style approach, but for now,
	   I'm going do looped subtraction because that will be easy
	   but slow, of course. */

	ehbi_set(remainder, numerator);

	err = 0;
	while (ehbi_greater_than(remainder, denominator, &err)) {
		if (err) {
			return err;
		}
		err = ehbi_inc_ul(quotient, 1);
		if (err) {
			return err;
		}
		err = ehbi_dec(remainder, denominator);
		if (err) {
			return err;
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_inc(struct ehbigint *bi, const struct ehbigint *val)
{
	size_t i;
	unsigned char a, b, c;

	if (bi == 0 || val == 0) {
		EHBI_LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (val->bytes_used > bi->bytes_len) {
		EHBI_LOG_ERROR0("byte[] too small");
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
			EHBI_LOG_ERROR0("byte[] too small for carry");
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
			EHBI_LOG_ERROR0("byte[] too small for carry");
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
		EHBI_LOG_ERROR0("Null argument(s)");
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
				EHBI_LOG_ERROR0("byte[] too small for borrow");
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
	if ((bi->bytes_used < bi->bytes_len) && (bi->bytes[0] != 0xFF)
	    && (bi->bytes[i] > 0x7F) && (i > 0) && (bi->bytes[i - 1] == 0x00)) {
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
		EHBI_LOG_ERROR0("Null argument(s)");
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
			EHBI_LOG_ERROR0("Result byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = c;
		res->bytes_used++;

		c = (c > a) ? 0xFF : 0;
	}

	if (c) {
		if (i > res->bytes_len) {
			EHBI_LOG_ERROR0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		while (i <= res->bytes_len) {
			res->bytes[res->bytes_len - i] = 0xFF;
			++i;
		}
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
		EHBI_LOG_ERROR0("Null argument(s)");
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

static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len)
{
	char *rv;

	if (dec_str == 0 || buf == 0) {
		EHBI_LOG_ERROR0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 5) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	rv = decimal_to_hex(dec_str, dec_len, buf, buf_len);

	if (rv == NULL) {
		EHBI_LOG_ERROR1("Character not decimal? (%s)", dec_str);
		return EHBI_BAD_INPUT;
	}

	return EHBI_SUCCESS;
}

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len)
{
	char *rv;

	if (hex == 0 || buf == 0) {
		EHBI_LOG_ERROR0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 2 || buf_len < hex_len) {
		EHBI_LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	rv = hex_to_decimal(hex, hex_len, buf, buf_len);

	if (rv == NULL) {
		EHBI_LOG_ERROR1("Character not hex? (%s)", hex);
		return EHBI_BAD_INPUT;
	}

	return EHBI_SUCCESS;
}

void ehbi_log_backtrace(FILE *log)
{
#ifdef _POSIX_C_SOURCE
	void *array[4096];
	size_t size;

	size = backtrace(array, 4096);
	backtrace_symbols_fd(array, size, fileno(log));
#else
	fprintf(log, "(backtrace unavailable)\n");
#endif /* _POSIX_C_SOURCE */
}
