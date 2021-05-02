/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-dec-corner-case.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_dec_corner_case(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[BILEN];
	unsigned char bytes_buf2[BILEN];
	unsigned char bytes_buf3[BILEN];
	struct ehbigint bi1, bi2, expect_bi;

	/*
	   $ bc <<< "50331548-33554393"
	   16777155
	 */
	const char *str_1 = "50331548";
	const char *str_2 = "33554393";
	const char *str_3 = "16777155";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);
	ehbi_init(&expect_bi, bytes_buf3, BILEN);

	ehbi_set_decimal_string(&bi1, str_1, eembed_strlen(str_1), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, str_1);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_decimal_string(&bi2, str_2, eembed_strlen(str_2), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, str_2);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_decimal_string(&expect_bi, str_3, eembed_strlen(str_3), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, str_3);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_dec(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_dec");
		log->append_eol(log);
	}
	failures += Check_ehbigint(&bi1, &expect_bi);

	return failures;
}

ECHECK_TEST_MAIN_V(test_dec_corner_case)
