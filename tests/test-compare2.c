/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare2.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_compare2(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	int result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	struct ehbigint bi1, bi2;

	const char *str_1 = "0x00F513";
	const char *str_2 = "0x00023B";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 10);

	ehbi_set_hex_string(&bi1, str_1, eembed_strlen(str_1), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, str_1);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_hex_string(&bi2, str_2, eembed_strlen(str_2), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, str_2);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	result = ehbi_compare(&bi1, &bi2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	result = ehbi_less_than(&bi1, &bi2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	result = ehbi_greater_than(&bi1, &bi2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi2, &bi1);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, -1);

	result = ehbi_less_than(&bi2, &bi1);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	result = ehbi_greater_than(&bi2, &bi1);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	return failures;
}

ECHECK_TEST_MAIN_V(test_compare2)
