/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-subtract.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_subtract_v(int verbose, const char *str_1, const char *str_2,
			 const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);
	ehbi_init(&bi3, bytes_buf3, 20);

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

	ehbi_subtract(&bi3, &bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_subtract");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&bi3, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_subtract_v(");
		log->append_s(log, str_1);
		log->append_s(log, ",");
		log->append_s(log, str_2);
		log->append_s(log, ",");
		log->append_s(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_subtract_big(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[14];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =    "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x0F20100F00002202040A";
	const char *str_2 = "0x0320000200004404020A";
	const char *str_3 = "0x0C00100CFFFFDDFE0200";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);
	ehbi_init(&bi3, bytes_buf3, 14);

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

	ehbi_subtract(&bi3, &bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_subtract");
		log->append_eol(log);
	}
	failures += Check_ehbigint_hex(&bi3, str_3);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, "failures in test_subtract_big");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_subtract_l(int verbose, const char *str_1, long l,
			 const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi3, bytes_buf3, 20);

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

	ehbi_subtract_l(&bi3, &bi1, l, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_subtract_l");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&bi3, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_subtract_l(");
		log->append_s(log, str_1);
		log->append_s(log, ",");
		log->append_l(log, l);
		log->append_s(log, ",");
		log->append_s(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	return failures;
}

unsigned test_subtract(int v)
{
	unsigned failures = 0;

	failures += test_subtract_v(v, "257", "255", "2");
	failures += test_subtract_v(v, "0", "23", "-23");
	failures += test_subtract_v(v, "5", "7", "-2");
	failures += test_subtract_v(v, "2", "11", "-9");
	failures += test_subtract_v(v, "-11", "2", "-13");
	failures += test_subtract_v(v, "-5", "-2", "-3");
	failures += test_subtract_v(v, "-17", "-27", "10");
	failures += test_subtract_v(v, "35813", "65521", "-29708");

	failures += test_subtract_big(v);

	failures += test_subtract_l(v, "35813", 65521, "-29708");

	return failures;
}

ECHECK_TEST_MAIN_V(test_subtract)
