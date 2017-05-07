/*
test-ehbigint-private-utils.c
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
#include "test-ehbigint-private-utils.h"
#include <errno.h>

int check_ehbigint_dec(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual1[BUFLEN];
	char *actual2;
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	actual2 = ehbi_to_decimal_string(val, actual1, BUFLEN, &err);
	if (err) {
		Test_log_error2("error %d ehbi_to_decimal_string (%s)\n", err,
				buf);
		return 1;
	}

	return check_str_m(actual1, expected, buf) || check_str_m(actual2,
								  expected,
								  buf);
}

int check_ehbigint_hex(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual[BUFLEN];
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	ehbi_to_hex_string(val, actual, BUFLEN, &err);
	if (err) {
		Test_log_error2("error %d ehbi_to_hex_string (%s)\n", err, buf);
		return 1;
	}

	return check_str_m(actual, expected, buf);
}

unsigned long ehbigint_to_unsigned_long(struct ehbigint *val, int *err)
{
	int base;
	char dec[BUFLEN];
	char *endptr;
	unsigned long result;

	ehbi_to_decimal_string(val, dec, BUFLEN, err);
	if (*err) {
		Test_log_error1("error %d ehbi_to_decimal_string\n", *err);
		return 0;
	}
	base = 10;
	errno = 0;
	result = strtoul(dec, &endptr, base);
	*err = errno;
	if (*err || (endptr && strlen(endptr) > 0)) {
		Test_log_error4("strtoul('%s'); (invalid:'%s') (%d: %s)\n", dec,
				endptr, *err, strerror(*err));
	}
	return result;
}
