/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-div.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_div_v(int verbose, const char *snumerator,
		    const char *sdenominator, const char *squotient,
		    const char *sremainder)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&denominator, bytes_denominator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	err =
	    ehbi_set_decimal_string(&numerator, snumerator,
				    eembed_strlen(snumerator));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, snumerator);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&numerator, snumerator);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, snumerator);
		log->append_s(log, ". Aborting test");
		log->append_eol(log);
		return failures;
	}

	err =
	    ehbi_set_decimal_string(&denominator, sdenominator,
				    eembed_strlen(sdenominator));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, sdenominator);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&denominator, sdenominator);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, sdenominator);
		log->append_s(log, ". Aborting test");
		log->append_eol(log);
		return failures;
	}

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_div");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&quotient, squotient);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	return failures;
}

unsigned test_div_l(int verbose, const char *snumerator, long ldenominator,
		    const char *squotient, const char *sremainder)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char bytes_numerator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	err =
	    ehbi_set_decimal_string(&numerator, snumerator,
				    eembed_strlen(snumerator));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, snumerator);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&numerator, snumerator);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, snumerator);
		log->append_s(log, ". Aborting test");
		log->append_eol(log);
		return failures;
	}

	err = ehbi_div_l(&quotient, &remainder, &numerator, ldenominator);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_div");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&quotient, squotient);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	return failures;
}

unsigned test_div_by_zero(int verbose)
{
	int err;
	unsigned failures;

	const size_t buflen = 250;
	char buf[250];
	struct eembed_str_buf sbuf;
	struct eembed_log slog;
	struct eembed_log *log;
	struct eembed_log *orig;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	orig = ehbi_log_get();
	eembed_memset(buf, 0x00, buflen);
	log = eembed_char_buf_log_init(&slog, &sbuf, buf, buflen);
	if (log) {
		ehbi_log_set(log);
	}

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&denominator, bytes_denominator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	ehbi_set_l(&numerator, 10);
	ehbi_set_l(&denominator, 0);

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (!err) {
		++failures;
		Test_log_error("no error from ehbi_div by zero?\n");
	}
	failures += check_str_contains(buf, "denominator == 0");

	ehbi_log_set(orig);
	return failures;
}

unsigned test_div(int v)
{
	unsigned failures = 0;

	failures += test_div_v(v, "20000", "100", "200", "0");
	failures += test_div_v(v, "20001", "100", "200", "1");
	failures += test_div_v(v, "20013", "200", "100", "13");
	failures += test_div_v(v, "287713", "571", "503", "500");

	failures += test_div_v(v, "-13", "6", "-2", "1");
	failures += test_div_v(v, "20", "-7", "-2", "6");
	failures += test_div_v(v, "-100", "-9", "11", "1");

	/*
	   lldiv_t result = lldiv(5088824049625,33554393);
	   result.quot: 151658, result.remainder: 31916031
	 */
	failures +=
	    test_div_v(v, "5088824049625", "33554393", "151658", "31916031");

	failures += test_div_by_zero(v);

	failures += test_div_l(v, "-13", 6, "-2", "1");
	failures += test_div_l(v, "600851475143", 65521, "9170364", "55499");

	return failures;
}

ECHECK_TEST_MAIN_V(test_div)
