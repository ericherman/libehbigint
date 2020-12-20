/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_compare_chain(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	int result;
	unsigned failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x60000000000";
	const char *str_2 = "0x6F000000000";
	const char *str_3 = "0x7F";

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

	err = 0;
	result = ehbi_compare(&bi1, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_less_than(&bi1, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_greater_than(&bi1, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, -1);

	err = 0;
	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_compare(&bi1, &bi3, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_less_than(&bi1, &bi3, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	err = 0;
	result = ehbi_greater_than(&bi1, &bi3, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_compare(&bi3, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int(result, -1);

	err = 0;
	result = ehbi_less_than(&bi3, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result, 1);

	err = 0;
	result = ehbi_greater_than(&bi3, &bi1, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, 0);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_compare_chain");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_greater_than(int verbose, const char *a, const char *b,
			   int expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	int result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	struct ehbigint bi1, bi2;
	char buf1[50];
	char buf2[50];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 10);

	err = ehbi_set_decimal_string(&bi1, a, eembed_strlen(a));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, a);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	ehbi_to_decimal_string(&bi1, buf1, 50, &err);
	failures += check_str(buf1, a);

	err = ehbi_set_decimal_string(&bi2, b, eembed_strlen(b));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, b);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	ehbi_to_decimal_string(&bi2, buf2, 50, &err);
	failures += check_str(buf2, b);

	err = 0;
	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int(result, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_greater_than(");
		log->append_s(log, a);
		log->append_s(log, ",");
		log->append_s(log, b);
		log->append_s(log, ",");
		log->append_l(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_compare(int v)
{
	unsigned failures = 0;

	failures += test_compare_chain(v);

	failures += test_greater_than(v, "65521", "35813", 1);
	failures += test_greater_than(v, "34813", "65521", 0);

	return failures;
}

ECHECK_TEST_MAIN_V(test_compare)
