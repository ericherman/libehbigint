/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-dec.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_dec_hex(int verbose, const char *str_1, const char *str_2,
		      const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);

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

	ehbi_dec(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_dec");
		log->append_eol(log);
	}
	failures += Check_ehbigint_hex(&bi1, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_dec_hex(");
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

unsigned test_dec_inner(int verbose, const char *str_1, const char *str_2,
			const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);

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

	ehbi_dec(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_dec");
		log->append_eol(log);
	}
	failures += Check_ehbigint_dec(&bi1, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_dec(");
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

unsigned test_dec(int v)
{
	unsigned failures = 0;
	const char *hexstr_1, *hexstr_2, *hexexpct;

	failures += test_dec_inner(v, "5", "2", "3");
	failures += test_dec_inner(v, "7", "0", "7");
	failures += test_dec_inner(v, "0", "1", "-1");
	failures += test_dec_inner(v, "9", "11", "-2");

	/* u64_max = "0xFFFFFFFFFFFFFFFF" */
	hexstr_1 = "0x700002000000000001";
	hexstr_2 = "0x100000100000000001";
	hexexpct = "0x600001F00000000000";
	failures += test_dec_hex(v, hexstr_1, hexstr_2, hexexpct);

	return failures;
}

ECHECK_TEST_MAIN_V(test_dec)
