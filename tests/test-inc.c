/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-inc.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_inc_hex(int verbose, const char *hexs1, const char *hexs2,
		      const char *expct)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);

	err = ehbi_set_hex_string(&bi1, hexs1, eembed_strlen(hexs1));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, hexs1);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	err = ehbi_set_hex_string(&bi2, hexs2, eembed_strlen(hexs2));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, hexs2);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_shift_left");
		log->append_eol(log);
	}

	failures += Check_ehbigint_hex(&bi1, expct);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_inc_hex(");
		log->append_s(log, hexs1);
		log->append_s(log, ",");
		log->append_s(log, hexs2);
		log->append_s(log, ",");
		log->append_s(log, expct);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	return failures;
}

unsigned test_inc_v(int verbose, const char *v1, const char *v2,
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

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);

	err = ehbi_set_decimal_string(&bi1, v1, eembed_strlen(v1));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, v1);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	err = ehbi_set_decimal_string(&bi2, v2, eembed_strlen(v2));

	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, v2);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_inc");
		log->append_eol(log);
	}
	failures += Check_ehbigint_dec(&bi1, expect);

	check_int(bi1.sign, expect[0] == '-');

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_inc_hex(");
		log->append_s(log, v1);
		log->append_s(log, ",");
		log->append_s(log, v2);
		log->append_s(log, ",");
		log->append_s(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}
	return failures;
}

unsigned test_inc(int v)
{
	unsigned failures = 0;
	const char *hexs1, *hexs2, *expct;

	failures += test_inc_v(v, "254", "1", "255");
	failures += test_inc_v(v, "254", "-1", "253");
	failures += test_inc_v(v, "-1", "1", "0");

	/*
	   $ echo "9415273 + 3154116455" | bc
	   3163531728
	 */
	failures += test_inc_v(v, "9415273", "3154116455", "3163531728");

	/* u64max = "0xFFFFFFFFFFFFFFFF" */
	hexs1 = "0x00F00000F00000000001";
	hexs2 = "0x00100000100000000001";
	expct = "0x01000001000000000002";
	failures += test_inc_hex(v, hexs1, hexs2, expct);

	return failures;
}

ECHECK_TEST_MAIN_V(test_inc)
