/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_set_v(int verbose, long lval, const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char a_bytes[10];
	struct ehbigint a_bigint;

	unsigned char b_bytes[10];
	struct ehbigint b_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&a_bigint, a_bytes, 10);
	ehbi_init(&b_bigint, b_bytes, 10);

	ehbi_set_l(&a_bigint, lval, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, lval);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	failures += Check_ehbigint_dec(&a_bigint, expect);

	ehbi_set(&b_bigint, &a_bigint, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set");
		log->append_eol(log);
	}
	failures += Check_ehbigint_dec(&b_bigint, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_set_v(");
		log->append_l(log, lval);
		log->append_s(log, ",");
		log->append_s(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_set_dec_str(int verbose, const char *val)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char bi_bytes[20];
	struct ehbigint bi;
	char buf[80];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi, bi_bytes, 20);

	ehbi_set_decimal_string(&bi, val, eembed_strlen(val), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, val);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_to_decimal_string(&bi, buf, 80, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_to_decimal_string");
		log->append_eol(log);
	}
	failures += check_str(buf, val);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_set_dec_str(");
		log->append_s(log, val);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_set_too_small(int verbose)
{
	struct eembed_log *log = eembed_err_log;

	const size_t buflen = 250;
	char buf[250];
	struct eembed_str_buf sbuf;
	struct eembed_log slog;
	struct eembed_log *orig;

	int err;
	long lval;
	const size_t lbuf_size = 21;
	char lbuf[21];
	unsigned failures;

	unsigned char a_bytes[4];
	struct ehbigint a_bigint;

	unsigned char b_bytes[2];
	struct ehbigint b_bigint;

	struct ehbigint *rv;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	orig = ehbi_log_get();
	eembed_memset(buf, 0x00, buflen);
	log = eembed_char_buf_log_init(&slog, &sbuf, buf, buflen);

	err = 0;
	ehbi_init(&a_bigint, a_bytes, 4);
	ehbi_init(&b_bigint, b_bytes, 2);

	lval = (1L << (8 * 3));
	eembed_long_to_str(lbuf, lbuf_size, lval);

	rv = ehbi_set_l(&a_bigint, lval, &err);
	failures += check_ptr_not_null_m(rv, lbuf);
	failures += check_int_m(err, 0, "err?");
	failures += Check_ehbigint_dec(&a_bigint, lbuf);
	failures += check_int(a_bigint.bytes_used, 4);

	failures += check_int(b_bigint.bytes_len, 2);
	if (log) {
		ehbi_log_set(log);
	}
	rv = ehbi_set(&b_bigint, &a_bigint, &err);
	ehbi_log_set(orig);
	failures += check_int_m(rv ? 0 : 1, 1, "expected NULL");
	failures += check_int_m(err, EHBI_BYTES_TOO_SMALL, "BYTES_TOO_SMALL");

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures test_set_too_small");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_set(int v)
{
	unsigned failures = 0;

	failures += test_set_v(v, 3, "3");
	failures += test_set_v(v, -5, "-5");

	failures += test_set_dec_str(v, "3");
	failures += test_set_dec_str(v, "-3");

	failures += test_set_too_small(v);

	return failures;
}

ECHECK_TEST_MAIN_V(test_set)
