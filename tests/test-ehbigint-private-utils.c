/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-ehbigint-private-utils.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned check_ehbigint(struct ehbigint *actual, struct ehbigint *expected,
			int line, const char *msg)
{
	int err = 0;
	char buf[BUFLEN];
	char abuf[BUFLEN];
	char ebuf[BUFLEN];
	struct eembed_log *log = eembed_err_log;

	eembed_strcpy(buf, msg);
	eembed_strcat(buf, eembed_long_to_str(abuf, BUFLEN, line));

	if (!actual && expected) {
		log->append_s(log, buf);
		log->append_s(log, ": actual is NULL, expected: ");
		ehbi_to_decimal_string(expected, ebuf, BUFLEN, &err);
		log->append_s(log, ebuf);
		log->append_eol(log);
		return 1;
	}
	if (actual && !expected) {
		log->append_s(log, buf);
		log->append_s(log, ": actual is ");
		ehbi_to_decimal_string(actual, abuf, BUFLEN, &err);
		log->append_s(log, abuf);
		log->append_s(log, ", expected NULL");
		log->append_eol(log);
		return 1;
	}

	if (ehbi_equals(actual, expected)) {
		return 0;
	}

	ehbi_to_decimal_string(actual, abuf, BUFLEN, &err);
	if (err) {
		log->append_s(log, "Error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_to_decimal_string(actual, ...)");
		log->append_eol(log);
		log->append_s(log, buf);
		log->append_s(log, " Aborting test");
		log->append_eol(log);
		return 1;
	}
	ehbi_to_decimal_string(expected, ebuf, BUFLEN, &err);
	if (err) {
		log->append_s(log, "Error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_to_decimal_string(expect, ...)");
		log->append_eol(log);
		log->append_s(log, buf);
		log->append_s(log, " Aborting test");
		log->append_eol(log);
		return 1;
	}
	check_str_m(abuf, ebuf, buf);
	return 1;
}

unsigned check_ehbigint_dec(struct ehbigint *actual, const char *expected,
			    int line, const char *msg)
{
	int err = 0;
	char abuf[BUFLEN];
	char *actual2 = NULL;
	char buf[BUFLEN];
	struct eembed_log *log = eembed_err_log;

	eembed_strcpy(buf, msg);
	eembed_strcat(buf, ":");
	eembed_strcat(buf, eembed_long_to_str(abuf, BUFLEN, line));

	actual2 = ehbi_to_decimal_string(actual, abuf, BUFLEN, &err);
	if (err) {
		log->append_s(log, "Error ");
		log->append_l(log, err);
		log->append_s(log, " ehbi_to_decimal_string returned '");
		log->append_s(log, abuf);
		log->append_s(log, "' ");
		log->append_s(log, buf);
		log->append_eol(log);
		return 1;
	}

	return check_str_m(abuf, expected, buf)
	    || check_str_m(actual2, expected, buf);
}

unsigned check_ehbigint_hex(struct ehbigint *actual, const char *expected,
			    int line, const char *msg)
{
	int err = 0;
	char abuf[BUFLEN];
	char buf[BUFLEN];
	struct eembed_log *log = eembed_err_log;

	eembed_strcpy(buf, msg);
	eembed_strcat(buf, eembed_long_to_str(abuf, BUFLEN, line));

	ehbi_to_hex_string(actual, abuf, BUFLEN, &err);
	if (err) {
		log->append_s(log, "Error ");
		log->append_l(log, err);
		log->append_s(log, " ehbi_to_hex_string returned '");
		log->append_s(log, abuf);
		log->append_s(log, "' ");
		log->append_s(log, buf);
		log->append_eol(log);
		return 1;
	}

	return check_str_m(abuf, expected, buf);
}

unsigned long ehbigint_to_unsigned_long(struct ehbigint *actual, int *err)
{
	char dec[BUFLEN];
	char *endptr;
	unsigned long result;
	struct eembed_log *log = eembed_err_log;

	ehbi_to_decimal_string(actual, dec, BUFLEN, err);
	if (*err) {
		log->append_s(log, "Error ");
		log->append_l(log, *err);
		log->append_s(log, " ehbi_to_decimal_string(actual, ...)");
		log->append_eol(log);
		return 0;
	}
	result = eembed_str_to_ulong(dec, &endptr);
	if (!endptr || eembed_strlen(endptr) > 0) {
		*err = 1;
		log->append_s(log, "eembed_str_to_ulong('");
		log->append_s(log, dec);
		log->append_s(log, "'); (invalid:'");
		log->append_s(log, endptr);
		log->append_s(log, "')");
		log->append_eol(log);
	}
	return result;
}
