/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-add.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_add_decimal(int verbose, const char *str_1, const char *str_2,
			  const char *str_3)
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

	ehbi_add(&bi3, &bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_add. Aborting test.");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&bi3, str_3);
	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_add_decimal(");
		log->append_s(log, str_1);
		log->append_s(log, ", ");
		log->append_s(log, str_2);
		log->append_s(log, ", ");
		log->append_s(log, str_3);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_add_hex(int verbose, const char *str_1, const char *str_2,
		      const char *str_3)
{
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;
	struct eembed_log *log = eembed_err_log;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);
	ehbi_init(&bi3, bytes_buf3, 20);

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

	ehbi_add(&bi3, &bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_add. Aborting test.");
		log->append_eol(log);
	}

	failures += Check_ehbigint_hex(&bi3, str_3);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_add_hex(");
		log->append_s(log, str_1);
		log->append_s(log, ",");
		log->append_s(log, str_2);
		log->append_s(log, ",");
		log->append_s(log, str_3);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_add(int v)
{
	/*      char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F20000F0000000000A";
	const char *str_2__ = "0x32000020000000000A";
	const char *str_3 = "0x01240001100000000014";

	unsigned failures = 0;

	failures += test_add_hex(v, str_1, str_2__, str_3);

	failures += test_add_decimal(v, "1000", "2000", "3000");
	failures += test_add_decimal(v, "0", "5000", "5000");
	failures += test_add_decimal(v, "12000", "13000", "25000");
	failures += test_add_decimal(v, "-1", "-2", "-3");
	failures += test_add_decimal(v, "1", "-2", "-1");
	failures += test_add_decimal(v, "-6", "2", "-4");
	failures += test_add_decimal(v, "-6", "11", "5");

	return failures;
}

ECHECK_TEST_MAIN_V(test_add)
