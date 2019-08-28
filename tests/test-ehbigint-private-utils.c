/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-ehbigint-private-utils.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <errno.h>

int check_ehbigint(struct ehbigint *val, struct ehbigint *expected, int line,
		   const char *msg)
{
	int err;
	char buf[BUFLEN];
	char vbuf[BUFLEN];
	char ebuf[BUFLEN];

	err = 0;
	if (!val && expected) {
		sprintf(buf, "%s:%d", msg, line);
		Test_log_error2("%s: val is NULL, expected: %s\n", buf,
				ehbi_to_decimal_string(expected, ebuf, BUFLEN,
						       &err));
		return 1;
	}
	if (val && !expected) {
		sprintf(buf, "%s:%d", msg, line);
		Test_log_error2("%s val is %s, expected: NULL\n", buf,
				ehbi_to_decimal_string(val, vbuf, BUFLEN,
						       &err));
		return 1;
	}

	if (ehbi_equals(val, expected, &err)) {
		return 0;
	}

	sprintf(buf, "%s:%d", msg, line);
	ehbi_to_decimal_string(val, vbuf, BUFLEN, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_to_decimal_string\n", err);
		Test_log_error1("%s Aborting test\n", msg);
		return 1;
	}
	ehbi_to_decimal_string(expected, ebuf, BUFLEN, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_to_decimal_string\n", err);
		Test_log_error1("%s Aborting test\n", msg);
		return 1;
	}
	Test_log_error3("%s val is %s, expected %s\n", buf, vbuf, ebuf);
	return 1;
}

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

int log_contains(FILE *log, const char *expected)
{
	char buffer[4096];
	rewind(log);
	while (fgets(buffer, 4096, log)) {
		if (strstr(buffer, expected)) {
			return 0;
		}
	}
	fprintf(stderr, "'%s' not found in log:\n", expected);
	rewind(log);
	while (fgets(buffer, 4096, log)) {
		fprintf(stderr, "%s", buffer);
	}
	fprintf(stderr, "\n(end of log)\n");

	return 1;
}
