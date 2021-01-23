/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-mul.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_mul_v(int verbose, long al, long bl, const char *expected)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char a_bytes[16];
	struct ehbigint a_bigint;

	unsigned char b_bytes[16];
	struct ehbigint b_bigint;

	unsigned char result_bytes[16];
	struct ehbigint result;

	char buf[BUFLEN];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&a_bigint, a_bytes, 16, &err);
	ehbi_init(&b_bigint, b_bytes, 16, &err);
	ehbi_init(&result, result_bytes, 16, &err);

	ehbi_set_l(&a_bigint, al, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l");
		log->append_eol(log);
	}
	eembed_long_to_str(buf, BUFLEN, al);
	failures += Check_ehbigint_dec(&a_bigint, buf);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, " assign failed (");
		log->append_s(log, buf);
		log->append_s(log, "). Aborting test");
		log->append_eol(log);
		return failures;
	}

	ehbi_set_l(&b_bigint, bl, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l");
		log->append_eol(log);
		err = 0;
	}
	eembed_long_to_str(buf, BUFLEN, bl);
	failures += Check_ehbigint_dec(&b_bigint, buf);
	if (failures) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, " assign failed (");
		log->append_s(log, buf);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	ehbi_mul(&result, &a_bigint, &b_bigint, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_mul");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&result, expected);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_mul_v(");
		log->append_l(log, al);
		log->append_s(log, ",");
		log->append_l(log, bl);
		log->append_s(log, ",");
		log->append_s(log, expected);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_mul(int v)
{
	unsigned failures = 0;

	failures += test_mul_v(v, 2, 256, "512");

	failures += test_mul_v(v, 7, -3, "-21");
	failures += test_mul_v(v, -41, -2, "82");

	failures += test_mul_v(v, 500, 333, "166500");

	/*
	   $ bc <<< "9415273 * 252533"
	   2377667136509
	 */
	failures += test_mul_v(v, 9415273, 252533, "2377667136509");
	failures += test_mul_v(v, 239862259L, 581571519L, "139497058317401421");

	return failures;
}

ECHECK_TEST_MAIN_V(test_mul)
