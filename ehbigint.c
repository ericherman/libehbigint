#include "ehbigint.h"

#include <stdio.h>

#ifndef LOG_ERROR0
#define LOG_ERROR0(format) \
 fprintf(stderr, "Line %d: ", __LINE__); \
 fprintf(stderr, format); \
 fprintf(stderr, "\n")
#endif

#ifndef LOG_ERROR1
#define LOG_ERROR1(format, arg) \
 fprintf(stderr, "Line %d: ", __LINE__); \
 fprintf(stderr, format, arg); \
 fprintf(stderr, "\n")
#endif

#ifndef LOG_ERROR2
#define LOG_ERROR2(format, arg1, arg2) \
 fprintf(stderr, "Line %d: ", __LINE__); \
 fprintf(stderr, format, arg1, arg2); \
 fprintf(stderr, "\n")
#endif

static int nibble_to_hex(unsigned char nibble, char *c)
{
	if (c == 0) {
		LOG_ERROR0("Null char pointer");
		return 1;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		LOG_ERROR1("Bad input '%x'", nibble);
		return 2;
	}
	return 0;
}

static int to_hex(unsigned char byte, char *high, char *low)
{
	int err;

	err = 0;
	err += nibble_to_hex((byte & 0xF0) >> 4, high);
	err += nibble_to_hex((byte & 0x0F), low);

	return err;
}

static int from_hex_nibble(unsigned char *nibble, char c)
{

	if (nibble == 0) {
		LOG_ERROR0("Null char pointer");
		return 3;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		LOG_ERROR1("Not hex (%c)", c);
		return 4;
	}

	return 0;
}

static int from_hex(unsigned char *byte, char high, char low)
{
	int err;
	unsigned char nibble;

	err = from_hex_nibble(&nibble, high);
	if (err) {
		LOG_ERROR1("Error with high nibble (%c)", high);
		return err;
	}
	*byte = (nibble << 4);

	err = from_hex_nibble(&nibble, low);
	if (err) {
		LOG_ERROR1("Error with low nibble (%c)", high);
		return err;
	}
	*byte += nibble;

	return 0;
}

int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
{
	size_t i, j;
	unsigned char high, low;

	if (bi == 0) {
		LOG_ERROR0("Null struct");
		return 5;
	}
	if (str == 0) {
		LOG_ERROR0("Null string");
		return 6;
	}
	if (str_len == 0 || str[0] == 0) {
		LOG_ERROR0("Zero length string");
		return 7;
	}

	/* ignore characters starting with the first NULL in string */
	for (i = 1; i < str_len; ++i) {
		if (str[i] == 0) {
			str_len = i - 1;
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
			LOG_ERROR0("byte[] too small");
			return 8;
		}
		if (from_hex(&(bi->bytes[--i]), high, low)) {
			LOG_ERROR2("Bad data (high: %c, low: %c)", high, low);
			return 9;
		}
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for easier debug */
	while (i-- > 0) {
		bi->bytes[i] = 0;
	}

	return 0;
}

int ehbi_to_hex_string(struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		LOG_ERROR0("Null struct");
		return 10;
	}
	if (buf == 0) {
		LOG_ERROR0("Null buffer");
		return 11;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		LOG_ERROR0("Buffer too small");
		return 12;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			LOG_ERROR0("Buffer too small, partially written");
			return 13;
		}
		err = to_hex(bi->bytes[i], buf + j, buf + j + 1);
		if (err) {
			LOG_ERROR0("Corrupted data?");
			return err;
		}
		j += 2;
	}
	if (j > buf_len) {
		LOG_ERROR0("Unable to write trailing NULL to buffer");
		return 14;
	}
	buf[j] = '\0';

	/* strip leading '0's ("0x0123" -> "0x123") */
	while (buf[2] == '0') {
		for (j = 2; j < buf_len - 1 && buf[j] != 0; ++j) {
			buf[j] = buf[j + 1];
		}
	}

	return 0;
}

int ehbi_add(struct ehbigint *res, struct ehbigint *bi1, struct ehbigint *bi2)
{
	size_t i;
	int a, b, c;
	struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		LOG_ERROR0("Null argument(s)");
		return 15;
	}

	if (bi1->bytes_used < bi2->bytes_used) {
		tmp = bi1;
		bi1 = bi2;
		bi2 = tmp;
	}

	c = 0;
	for (i = 1; i <= bi1->bytes_used; ++i) {
		a = bi1->bytes[bi1->bytes_len - i];
		b = (bi2->bytes_used < i) ? 0 : bi2->bytes[bi2->bytes_len - i];
		c = c + a + b;
		if (res->bytes_used + 1 > res->bytes_len) {
			LOG_ERROR0("Result byte[] too small");
			return 15;
		}
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}
	while (c) {
		if (res->bytes_used + 1 > res->bytes_len) {
			LOG_ERROR0("Result byte[] too small for carry");
			return 16;
		}
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}

	return 0;
}
