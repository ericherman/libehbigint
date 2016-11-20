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

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len);

static int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			       size_t buf_len);

static int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
			       size_t buf_len);

static int ehbi_nibble_to_hex(unsigned char nibble, char *c);

static int ehbi_from_hex_nibble(unsigned char *nibble, char c);

static char *ehbi_hex_to_decimal_raw(const char *hex, size_t hex_len, char *buf,
				     size_t buf_len);

static char *ehbi_decimal_to_hex_raw(const char *dec_str, size_t dec_len,
				     char *buf, size_t buf_len);

/* public functions */
int ehbi_set_hex_string(struct ehbigint *bi, const char *str, size_t str_len)
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
	while (i-- > 0) {
		bi->bytes[i] = 0x00;
	}

	return EHBI_SUCCESS;
}

int ehbi_set_decimal_string(struct ehbigint *bi, const char *dec, size_t len)
{
	char *hex;
	const char *str;
	size_t size;
	int err, negative;

	if (bi == 0) {
		Ehbi_log_error0("Null argument(s)");
		return EHBI_NULL_ARGS;
	}
	if (len == 0) {
		str = "0x00";
		len = 4;
	} else if (dec == NULL) {
		Ehbi_log_error0("Null string");
		return EHBI_NULL_STRING;
	}

	size = strlen("0x00") + len + 1;
	hex = (char *)ehbi_stack_alloc(size);
	if (!hex) {
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		return EHBI_STACK_TOO_SMALL;
	}
	if (dec[0] == '-') {
		str = dec + 1;
		len -= 1;
		negative = 1;
	} else {
		str = dec;
		negative = 0;
	}
	err = ehbi_decimal_to_hex(str, len, hex, size);
	if (err) {
		return err;
	}
	err = ehbi_set_hex_string(bi, hex, size);
	ehbi_stack_free(hex, size);
	if (negative) {
		err = err ? err : ehbi_negate(bi);
	}
	return err;
}

char *ehbi_to_hex_string(const struct ehbigint *bi, char *buf, size_t buf_len,
			 int *err)
{
	size_t i, j;

	if (err == NULL) {
		Ehbi_log_error0("Null err");
		goto ehbi_to_hex_string_end;
	}

	if (bi == 0) {
		Ehbi_log_error0("Null struct");
		*err = EHBI_NULL_STRUCT;
		goto ehbi_to_hex_string_end;
	}
	if (buf == 0) {
		Ehbi_log_error0("Null buffer");
		*err = EHBI_NULL_STRING_BUF;
		goto ehbi_to_hex_string_end;
	}
	if (buf_len < (bi->bytes_used + 3)) {
		Ehbi_log_error0("Buffer too small");
		*err = EHBI_STRING_BUF_TOO_SMALL;
		goto ehbi_to_hex_string_end;
	}

	*err = 0;
	j = 0;
	buf[j++] = '0';
	buf[j++] = 'x';

	for (i = bi->bytes_len - bi->bytes_used; i < bi->bytes_len; ++i) {
		if (j + 2 > buf_len) {
			Ehbi_log_error0("Buffer too small, partially written");
			*err = EHBI_STRING_BUF_TOO_SMALL_PARTIAL;
			goto ehbi_to_hex_string_end;
		}
		*err =
		    ehbi_byte_to_hex_chars(bi->bytes[i], buf + j, buf + j + 1);
		if (*err) {
			Ehbi_log_error0("Corrupted data?");
			goto ehbi_to_hex_string_end;
		}
		j += 2;
	}
	if (j > buf_len) {
		Ehbi_log_error0("Unable to write trailing NULL to buffer");
		*err = EHBI_STRING_BUF_TOO_SMALL_NO_NULL;
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
	return (err == NULL || *err) ? NULL : buf;
}

char *ehbi_to_decimal_string(const struct ehbigint *bi, char *buf, size_t len,
			     int *err)
{
	char *hex;
	size_t size;
	struct ehbigint tmp;

	hex = NULL;
	size = 0;
	tmp.bytes = NULL;
	tmp.bytes_len = 0;
	tmp.bytes_used = 0;

	if (err == NULL || bi == NULL || buf == NULL || len == 0) {
		Ehbi_log_error0("Null Arguments(s)");
		if (err) {
			*err = EHBI_NULL_ARGS;
		}
		goto ehbi_to_decimal_string_end;
	}
	*err = 0;

	size = strlen("0x00") + (2 * bi->bytes_used) + 1;
	hex = (char *)ehbi_stack_alloc(size);
	if (!hex) {
		Ehbi_log_error2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		*err = EHBI_STACK_TOO_SMALL;
		goto ehbi_to_decimal_string_end;
	}

	if (ehbi_is_negative(bi, err)) {
		tmp.bytes = (unsigned char *)ehbi_stack_alloc(bi->bytes_used);
		if (!tmp.bytes) {
			Ehbi_log_error2("Could not %s(%lu) bytes",
					ehbi_stack_alloc_str,
					(unsigned long)bi->bytes_used);
			*err = EHBI_STACK_TOO_SMALL;
			goto ehbi_to_decimal_string_end;
		}
		tmp.bytes_len = bi->bytes_used;
		ehbi_set(&tmp, bi);
		*err = ehbi_negate(&tmp);
		if (*err) {
			goto ehbi_to_decimal_string_end;
		}
		ehbi_to_hex_string(&tmp, hex, size, err);

		buf[0] = '-';
		buf[1] = '\0';
		buf = buf + 1;
		len -= 1;
	} else {
		ehbi_to_hex_string(bi, hex, size, err);
	}
	if (*err) {
		goto ehbi_to_decimal_string_end;
	}
	*err = ehbi_hex_to_decimal(hex, size, buf, len);

ehbi_to_decimal_string_end:
	if (tmp.bytes) {
		ehbi_stack_free(tmp.bytes, tmp.bytes_len);
	}
	if (hex) {
		ehbi_stack_free(hex, size);
	}
	if (buf && (err == NULL || *err)) {
		buf[0] = '\0';
	}
	return (err == NULL || *err) ? NULL : buf;
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

	rv = ehbi_decimal_to_hex_raw(dec_str, dec_len, buf, buf_len);

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

	rv = ehbi_hex_to_decimal_raw(hex, hex_len, buf, buf_len);

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

static char *ehbi_decimal_to_hex_raw(const char *dec_str, size_t dec_len,
				     char *buf, size_t buf_len)
{
	size_t i, j, k, hex_len;
	unsigned char *hex_buf, byte;

	if (dec_str == 0 || buf == 0 || buf_len < 5) {
		return NULL;
	}

	buf[0] = '0';
	buf[1] = 'x';

	/* first operate with binary data, convert to ASCII later */
	/* start by adjusting the buf for our needs - needs to be unsigned */
	hex_buf = (unsigned char *)buf + 2;	/* skip past leading "0x" */
	hex_len = buf_len - 3;	/* and leave room for the NULL terminator */

	/* zero out the buffer */
	for (i = 0; i < hex_len; ++i) {
		hex_buf[i] = '\0';
	}

	for (i = 0; i < dec_len && dec_str[i] != '\0'; ++i) {
		if (dec_str[i] < '0' || dec_str[i] > '9') {
			buf[0] = '\0';
			return NULL;
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
		ehbi_nibble_to_hex(buf[2 + j], (char *)hex_buf + j);
	}

	/* left shift away leading zeros */
	/* first find the index (j) of the first non-zero */
	for (j = 0; j < hex_len && hex_buf[j] == '0'; ++j) {
		;
	}

	/* but work on whole bytes */
	/* since j was incremented, whole bytes of '0' will be odd */
	if (j % 2 == 0) {
		j -= 1;
	}

	/* leave a "00" if the leading byte would be greater than 7F */
	if ((j >= 2) && (j + 1 < hex_len)) {
		ehbi_hex_chars_to_byte(hex_buf[j], hex_buf[j + 1], &byte);
		if (byte > 0x7F) {
			j -= 2;
		}
	}

	/* next, shift all the contents "j" places to the left */
	for (i = 0; i < (hex_len - j); ++i) {
		hex_buf[i] = hex_buf[i + j];
	}

	/* add a trailing NULL */
	buf[buf_len - 1 - j] = '\0';

	return buf;
}

static char *ehbi_hex_to_decimal_raw(const char *hex, size_t hex_len, char *buf,
				     size_t buf_len)
{
	size_t i, j, k, dec_len;
	unsigned char *dec_buf;
	char ascii_offset;
	unsigned char num_val;

	if (hex == 0 || buf == 0 || buf_len < 2 || buf_len < hex_len) {
		return NULL;
	}

	/* skip over leading '0x' in string */
	if (hex_len >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
		hex += 2;
		hex_len -= 2;
	}

	/* first operate with binary data, convert to ASCII later */
	/* start by adjusting the buf for our needs - needs to be unsigned */
	dec_buf = (unsigned char *)buf;
	dec_len = buf_len - 1;	/* leave room for the NULL terminator */

	/* zero out the buffer */
	for (i = 0; i < dec_len; ++i) {
		dec_buf[i] = 0;
	}

	for (i = 0; i < hex_len && hex[i] != 0; ++i) {
		ascii_offset = 0;
		if (hex[i] >= '0' && hex[i] <= '9') {
			ascii_offset = '0';
		} else if (hex[i] >= 'A' && hex[i] <= 'F') {
			ascii_offset = 'A';
		} else if (hex[i] >= 'a' && hex[i] <= 'f') {
			ascii_offset = 'a';
		}
		if (ascii_offset == 0) {
			dec_buf[0] = '\0';
			return NULL;
		}

		/* we're doing another digit, multiply previous by 16 */
		for (j = 0; j < dec_len; ++j) {
			k = dec_len - 1 - j;
			dec_buf[k] *= 16;
		}

		if (ascii_offset == '0') {
			num_val = (hex[i] - '0');
		} else {
			num_val = 10 + (hex[i] - ascii_offset);
		}
		dec_buf[dec_len - 1] += num_val;

		/* carry */
		for (j = 0; j < dec_len; ++j) {
			k = dec_len - 1 - j;
			if (dec_buf[k] >= 10) {
				dec_buf[k - 1] += (dec_buf[k] / 10);
				dec_buf[k] = (dec_buf[k] % 10);
			}
		}
	}

	/* convert to ASCII */
	for (j = 0; j < dec_len; ++j) {
		buf[j] = '0' + dec_buf[j];
	}

	/* left shift away leading zeros */
	/* first find the index (j) of the first non-zero */
	for (j = 0; j < dec_len && dec_buf[j] == '0'; ++j) {
		;
	}
	/* if everything is zero, include the last zero */
	if (j == dec_len) {
		--j;
	}
	/* next, shift all the contents "j" places to the left */
	for (i = 0; i < (dec_len - j); ++i) {
		dec_buf[i] = dec_buf[i + j];
	}

	/* add a trailing NULL */
	buf[buf_len - 1 - j] = '\0';

	return buf;
}
