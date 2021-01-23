/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-shift-right.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_shift_right_v(int verbose, const char *val, unsigned long bits,
			    const char *expected)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi;
	struct ehbigint expect_bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi, bytes_buf1, 20, &err);
	ehbi_init(&expect_bi, bytes_buf2, 20, &err);

	ehbi_set_hex_string(&bi, val, eembed_strlen(val), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, val);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_hex_string(&expect_bi, expected, eembed_strlen(expected),
			    &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, expected);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_shift_right(&bi, bits, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_shift_right");
		log->append_eol(log);
	}

	failures += Check_ehbigint(&bi, &expect_bi);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_shift_right_v(");
		log->append_s(log, val);
		log->append_s(log, ",");
		log->append_ul(log, bits);
		log->append_s(log, ",");
		log->append_s(log, expected);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_shift_right(int v)
{
	unsigned failures = 0;

	failures += test_shift_right_v(v, "0x02", 0, "0x02");
	failures += test_shift_right_v(v, "0x0300", 4, "0x30");
	failures += test_shift_right_v(v, "0x050000", 16, "0x05");
	failures += test_shift_right_v(v, "0x17000000", 20, "0x0170");
	failures += test_shift_right_v(v, "0x00FF000000", 24, "0x00FF");
	failures += test_shift_right_v(v, "0x03A8F057", 1, "0x01D4782B");

	return failures;
}

ECHECK_TEST_MAIN_V(test_shift_right)
