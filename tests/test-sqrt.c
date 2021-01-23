/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-sqrt.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_sqrt_v(int verbose, const char *sval, const char *ssqrt,
		     const char *sremainder)
{
	int err;
	unsigned failures;

	const size_t buflen = 250;
	char buf[250];
	struct eembed_str_buf sbuf;
	struct eembed_log slog;
	struct eembed_log *log;
	struct eembed_log *orig;

	unsigned char bytes_val[50];
	unsigned char bytes_sqrt[50];
	unsigned char bytes_remainder[50];

	struct ehbigint val;
	struct ehbigint sqrt;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	orig = ehbi_log_get();
	eembed_memset(buf, 0x00, buflen);
	log = eembed_char_buf_log_init(&slog, &sbuf, buf, buflen);
	if (log) {
		ehbi_log_set(log);
	}

	err = 0;
	ehbi_init(&val, bytes_val, 50, &err);
	ehbi_init(&sqrt, bytes_sqrt, 50, &err);
	ehbi_init(&remainder, bytes_remainder, 50, &err);

	ehbi_set_decimal_string(&val, sval, eembed_strlen(sval), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, sval);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&val, sval);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? (");
		log->append_s(log, sval);
		Test_log_error(") Aborting test\n");
		return 1;
	}

	ehbi_sqrt(&sqrt, &remainder, &val, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_sqrt");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&sqrt, ssqrt);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	ehbi_log_set(orig);

	return failures;
}

unsigned test_sqrt_negative(int verbose)
{
	int err;
	unsigned failures;

	const size_t buflen = 250;
	char buf[250];
	struct eembed_str_buf sbuf;
	struct eembed_log slog;
	struct eembed_log *log;
	struct eembed_log *orig;

	unsigned char bytes_val[10];
	unsigned char bytes_sqrt[10];
	unsigned char bytes_remainder[10];

	struct ehbigint val;
	struct ehbigint sqrt;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	orig = ehbi_log_get();
	eembed_memset(buf, 0x00, buflen);
	log = eembed_char_buf_log_init(&slog, &sbuf, buf, buflen);
	if (log) {
		ehbi_log_set(log);
	}

	err = 0;
	ehbi_init(&val, bytes_val, 10, &err);
	ehbi_init(&sqrt, bytes_sqrt, 10, &err);
	ehbi_init(&remainder, bytes_remainder, 10, &err);

	ehbi_set_l(&val, -100, &err);

	ehbi_sqrt(&sqrt, &remainder, &val, &err);
	if (!err) {
		++failures;
		STDERR_FILE_LINE_FUNC(orig);
		orig->append_s(orig, "no error from ehbi_sqrt of negative?");
		orig->append_eol(orig);
	}
	failures += check_str_contains(buf, "complex");

	ehbi_log_set(orig);
	return failures;
}

unsigned test_sqrt(int v)
{
	unsigned failures = 0;

	failures += test_sqrt_v(v, "100", "10", "0");
	failures += test_sqrt_v(v, "10001", "100", "1");
	failures += test_sqrt_v(v, "10000000000000001", "100000000", "1");
	failures +=
	    test_sqrt_v(v, "10000000000000000000001", "100000000000", "1");
	failures +=
	    test_sqrt_v(v,
			"22934986159900715116108208953020869407965649891682811",
			"151443012912120561509118328",
			"111863686247280161986167227");

	failures += test_sqrt_v(v, "0", "0", "0");
	failures += test_sqrt_negative(v);

	return failures;
}

ECHECK_TEST_MAIN_V(test_sqrt)
