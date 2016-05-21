#include "test-ehbigint-private-utils.h"

#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>
#include <errno.h>

int check_ehbigint_dec(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual[BUFLEN];
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	err = ehbi_to_decimal_string(val, actual, BUFLEN);
	if (err) {
		LOG_ERROR2("error %d ehbi_to_decimal_string (%s)\n", err, buf);
		return 1;
	}

	return check_str_m(actual, expected, buf);
}

int check_ehbigint_hex(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual[BUFLEN];
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	err = ehbi_to_hex_string(val, actual, BUFLEN);
	if (err) {
		LOG_ERROR2("error %d ehbi_to_hex_string (%s)\n", err, buf);
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

	*err = ehbi_to_decimal_string(val, dec, BUFLEN);
	if (*err) {
		LOG_ERROR1("error %d ehbi_to_decimal_string\n", *err);
		return 0;
	}
	base = 10;
	errno = 0;
	result = strtoul(dec, &endptr, base);
	*err = errno;
	if (*err || (endptr && strlen(endptr) > 0)) {
		LOG_ERROR4("strtoul('%s'); (invalid:'%s') (%d: %s)\n", dec,
			   endptr, *err, strerror(*err));
	}
	return result;
}
