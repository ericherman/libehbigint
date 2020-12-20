/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-equals.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_equals(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	int result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x720000F";
	const char *str_2 = "0x720000F";
	const char *str_3 = "0x0F";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 10);
	ehbi_init(&bi3, bytes_buf3, 20);

	err = ehbi_set_hex_string(&bi1, str_1, eembed_strlen(str_1));
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

	err = ehbi_set_hex_string(&bi2, str_2, eembed_strlen(str_2));
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

	err = ehbi_set_hex_string(&bi3, str_3, eembed_strlen(str_3));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, str_3);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	result = ehbi_equals(&bi1, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_equals");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_equals");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi3, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_equals");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	return failures;
}

ECHECK_TEST_MAIN_V(test_equals)
