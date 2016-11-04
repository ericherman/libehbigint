/*
ehbigint-util.c: support-functions for ehbigint.c
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

#include "ehbigint-util.h"
#include "ehbigint-log.h"

#include <ehstr.h> /* decimal_to_hex */

#include <execinfo.h>

#ifdef EHBI_NO_ALLOCA
void ehbi_do_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		EHBI_LOG_ERROR2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
	free(ptr);
}
#else
void ehbi_no_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		EHBI_LOG_ERROR2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
}
#endif

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

int ehbi_to_hex(unsigned char byte, char *high, char *low)
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

int ehbi_from_hex(unsigned char *byte, char high, char low)
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

int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
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

int ehbi_hex_to_decimal(const char *hex, size_t hex_len, char *buf,
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
