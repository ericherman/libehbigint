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
		return EHBI_NULL_CHAR_PTR;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		LOG_ERROR1("Bad input '%x'", nibble);
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

static int from_hex_nibble(unsigned char *nibble, char c)
{

	if (nibble == 0) {
		LOG_ERROR0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		LOG_ERROR1("Not hex (%c)", c);
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
		LOG_ERROR1("Error with high nibble (%c)", high);
		return EHBI_BAD_HIGH_NIBBLE;
	}
	*byte = (nibble << 4);

	err = from_hex_nibble(&nibble, low);
	if (err) {
		LOG_ERROR1("Error with low nibble (%c)", high);
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
		LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		LOG_ERROR0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}
	if (str == 0) {
		LOG_ERROR0("Null string");
		return EHBI_NULL_STRING;
	}
	if (str_len == 0 || str[0] == 0) {
		LOG_ERROR0("Zero length string");
		return EHBI_ZERO_LEN_STRING;
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
			return EHBI_BYTES_TOO_SMALL;
		}
		if (from_hex(&(bi->bytes[--i]), high, low)) {
			LOG_ERROR2("Bad data (high: %c, low: %c)", high, low);
			return EHBI_BAD_DATA;
		}
		bi->bytes_used++;
	}

	/* let's just zero out the rest of the bytes, for easier debug */
	while (i-- > 0) {
		bi->bytes[i] = 0;
	}

	return EHBI_SUCCESS;
}

int ehbi_to_hex_string(struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		LOG_ERROR0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (buf == 0) {
		LOG_ERROR0("Null buffer");
		return EHBI_NULL_STRING_BUF;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			LOG_ERROR0("Buffer too small, partially written");
			return EHBI_STRING_BUF_TOO_SMALL_PARTIAL;
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
		return EHBI_STRING_BUF_TOO_SMALL_NO_NULL;
	}
	buf[j] = '\0';

	/* strip leading '0's ("0x0123" -> "0x123") */
	while (buf[2] == '0') {
		for (j = 2; j < buf_len - 1 && buf[j] != 0; ++j) {
			buf[j] = buf[j + 1];
		}
	}

	return EHBI_SUCCESS;
}

int ehbi_add(struct ehbigint *res, struct ehbigint *bi1, struct ehbigint *bi2)
{
	size_t i;
	int a, b, c;
	struct ehbigint *tmp;

	if (res == 0 || bi1 == 0 || bi2 == 0) {
		LOG_ERROR0("Null argument(s)");
		return EHBI_NULL_ARGS;
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
			return EHBI_BYTES_TOO_SMALL;
		}
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}
	while (c) {
		if (res->bytes_used + 1 > res->bytes_len) {
			LOG_ERROR0("Result byte[] too small for carry");
			return EHBI_BYTES_TOO_SMALL_FOR_CARRY;
		}
		res->bytes[res->bytes_len - i] = (unsigned char)c;
		res->bytes_used++;
		c = c >> 8;
	}

	return EHBI_SUCCESS;
}

int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			size_t buf_len)
{
	size_t i, j, k, hex_len;
	int err;
	unsigned char *hex_buf;

	if (dec_str == 0 || buf == 0) {
		LOG_ERROR0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 4) {
		LOG_ERROR0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	buf[0] = '0';
	buf[1] = 'x';

	/* first operate with binary data, convert to ASCII later */
	/* start by adjusting the buf for our needs - needs to be unsigned */
	hex_buf = (unsigned char *)buf + 2;	/* skip past leading "0x" */
	hex_len = buf_len - 3;	/* and leave room for the NULL terminator */

	/* zero out the buffer */
	for (i = 0; i < hex_len; ++i) {
		hex_buf[i] = 0;
	}

	for (i = 0; i < dec_len && dec_str[i] != 0; ++i) {
		if (dec_str[i] < '0' || dec_str[i] > '9') {
			LOG_ERROR1("Character not decimal (%c)", dec_str[i]);
			return EHBI_BAD_INPUT;
		}
		/* we're doing another digit, multiply previous by 10 */
		for (j = 0; j < hex_len; ++j) {
			k = hex_len - 1 - j;
			hex_buf[k] *= 10;
		}

		hex_buf[hex_len - 1] += (dec_str[i] - '0');

		/* carry */
		for (j = 0; j < hex_len; ++j) {
			k = hex_len - 1 - j;
			if (hex_buf[k] >= 16) {
				hex_buf[k - 1] += (hex_buf[k] / 16);
				hex_buf[k] = (hex_buf[k] % 16);
			}
		}
	}

	/* convert to ASCII */
	for (j = 0; j < hex_len; ++j) {
		err = nibble_to_hex(hex_buf[j], buf + 2 + j);
		if (err) {
			LOG_ERROR0("Null char pointer");
			return EHBI_CORRUPT_DATA;
		}
	}

	/* left shift away leading zeros */
	/* first find the index (j) of the first non-zero */
	for (j = 0; hex_buf[j] == '0' && j < hex_len; ++j) {
		;
	}
	/* next, shift all the contents "j" places to the left */
	for (i = 0; i < (hex_len - j); ++i) {
		hex_buf[i] = hex_buf[i + j];
	}

	/* add a trailing NULL */
	buf[buf_len - 1 - j] = 0;

	return 0;
}
