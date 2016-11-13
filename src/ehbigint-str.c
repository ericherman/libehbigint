/*
ehbigint-str.c: to and from string functions for ehbighint structs
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

#include "ehbigint-str.h"
#include "ehbigint-log.h"
#include "ehbigint-util.h"

#include <string.h>		/* strlen */
#include <ehstr.h>		/* decimal_to_hex */

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len);

static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len);

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len);

static int ehbi_nibble_to_hex(unsigned char nibble, char *c);

static int ehbi_from_hex_nibble(unsigned char *nibble, char c);

/* public functions */
int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
{
	size_t i, j;
	unsigned char high, low;

	if (bi == 0) {
		Ehbi_log_error0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (bi->bytes == 0) {
		Ehbi_log_error0("Null bytes[]");
		return EHBI_NULL_BYTES;
	}
	if (str == 0) {
		Ehbi_log_error0("Null string");
		return EHBI_NULL_STRING;
	}
	if (str_len == 0 || str[0] == 0) {
		Ehbi_log_error0("Zero length string");
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
			Ehbi_log_error0("byte[] too small");
			return EHBI_BYTES_TOO_SMALL;
		}
		if (ehbi_hex_chars_to_byte(high, low, &(bi->bytes[--i]))) {
			Ehbi_log_error2("Bad data (high: %c, low: %c)", high,
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
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
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

int ehbi_to_hex_string(const struct ehbigint *bi, char *buf, size_t buf_len)
{
	int err;
	size_t i, j;

	if (bi == 0) {
		Ehbi_log_error0("Null struct");
		return EHBI_NULL_STRUCT;
	}
	if (buf == 0) {
		Ehbi_log_error0("Null buffer");
		return EHBI_NULL_STRING_BUF;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		Ehbi_log_error0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			Ehbi_log_error0("Buffer too small, partially written");
			return EHBI_STRING_BUF_TOO_SMALL_PARTIAL;
		}
		err =
		    ehbi_byte_to_hex_chars(bi->bytes[i], buf + j, buf + j + 1);
		if (err) {
			Ehbi_log_error0("Corrupted data?");
			return err;
		}
		j += 2;
	}
	if (j > buf_len) {
		Ehbi_log_error0("Unable to write trailing NULL to buffer");
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
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
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

int ehbi_byte_to_hex_chars(unsigned char byte, char *high, char *low)
{
	int err;

	err = 0;
	err += ehbi_nibble_to_hex((byte & 0xF0) >> 4, high);
	err += ehbi_nibble_to_hex((byte & 0x0F), low);

	return err;
}

int ehbi_hex_chars_to_byte(char high, char low, unsigned char *byte)
{
	int err;
	unsigned char nibble;

	err = ehbi_from_hex_nibble(&nibble, high);
	if (err) {
		Ehbi_log_error1("Error with high nibble (%c)", high);
		return EHBI_BAD_HIGH_NIBBLE;
	}
	*byte = (nibble << 4);

	err = ehbi_from_hex_nibble(&nibble, low);
	if (err) {
		Ehbi_log_error1("Error with low nibble (%c)", high);
		return EHBI_BAD_LOW_NIBBLE;
	}
	*byte += nibble;

	return EHBI_SUCCESS;
}

/* private functions */
static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len)
{
	char *rv;

	if (dec_str == 0 || buf == 0) {
		Ehbi_log_error0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 5) {
		Ehbi_log_error0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	rv = decimal_to_hex(dec_str, dec_len, buf, buf_len);

	if (rv == NULL) {
		Ehbi_log_error1("Character not decimal? (%s)", dec_str);
		return EHBI_BAD_INPUT;
	}

	return EHBI_SUCCESS;
}

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len)
{
	char *rv;

	if (hex == 0 || buf == 0) {
		Ehbi_log_error0("Null argument");
		return EHBI_NULL_ARGS;
	}

	if (buf_len < 2 || buf_len < hex_len) {
		Ehbi_log_error0("Buffer too small");
		return EHBI_STRING_BUF_TOO_SMALL;
	}

	rv = hex_to_decimal(hex, hex_len, buf, buf_len);

	if (rv == NULL) {
		Ehbi_log_error1("Character not hex? (%s)", hex);
		return EHBI_BAD_INPUT;
	}

	return EHBI_SUCCESS;
}

static int ehbi_nibble_to_hex(unsigned char nibble, char *c)
{
	if (c == 0) {
		Ehbi_log_error0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (nibble < 10) {
		*c = '0' + nibble;
	} else if (nibble < 16) {
		*c = 'A' + nibble - 10;
	} else {
		Ehbi_log_error1("Bad input '%x'", nibble);
		return EHBI_BAD_INPUT;
	}
	return EHBI_SUCCESS;
}

static int ehbi_from_hex_nibble(unsigned char *nibble, char c)
{

	if (nibble == 0) {
		Ehbi_log_error0("Null char pointer");
		return EHBI_NULL_CHAR_PTR;
	}
	if (c >= '0' && c <= '9') {
		*nibble = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*nibble = 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		*nibble = 10 + c - 'A';
	} else {
		Ehbi_log_error1("Not hex (%c)", c);
		return EHBI_NOT_HEX;
	}

	return EHBI_SUCCESS;
}
