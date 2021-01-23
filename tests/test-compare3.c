/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare3.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <limits.h>

unsigned test_compare3_ab(int verbose, long a, long b)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	int result, result_l;
	unsigned char bytes_buf1[1 + sizeof(long)];
	unsigned char bytes_buf2[sizeof(long)];
	struct ehbigint bi_a, bi_b;

	VERBOSE_ANNOUNCE(verbose);
	if (verbose) {
		log->append_c(log, '(');
		log->append_l(log, a);
		log->append_s(log, ", ");
		log->append_l(log, b);
		log->append_c(log, ')');
		log->append_eol(log);
	}

	failures = 0;
	err = 0;

	ehbi_init(&bi_a, bytes_buf1, 1 + sizeof(long), &err);
	ehbi_init(&bi_b, bytes_buf2, sizeof(long), &err);

	err = 0;
	ehbi_set_l(&bi_a, a, &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, a);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_l(&bi_b, b, &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, a);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	result = ehbi_compare(&bi_a, &bi_b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare");
		log->append_eol(log);
	}
	failures += check_int((result == 0), (a == b));
	failures += check_int((result > 0), (a > b));
	failures += check_int((result < 0), (a < b));

	result_l = ehbi_compare_l(&bi_a, b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_compare_l");
		log->append_eol(log);
	}
	failures += check_int(result, result_l);

	result = ehbi_less_than(&bi_a, &bi_b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than");
		log->append_eol(log);
	}
	failures += check_int(result == 0, !(a < b));

	result_l = ehbi_less_than_l(&bi_a, b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_less_than_l");
		log->append_eol(log);
	}
	failures += check_int(result, result_l);

	result = ehbi_greater_than(&bi_a, &bi_b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than");
		log->append_eol(log);
	}
	failures += check_int((result == 0), !(a > b));

	result_l = ehbi_greater_than_l(&bi_a, b, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_greater_than_l");
		log->append_eol(log);
	}
	failures += check_int(result, result_l);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_compare3_ab(");
		log->append_l(log, a);
		log->append_s(log, ",");
		log->append_l(log, b);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_compare_both_ways(int verbose, long a, long b)
{
	unsigned failures = 0;

	failures += test_compare3_ab(verbose, a, b);
	failures += test_compare3_ab(verbose, b, a);

	return failures;
}

unsigned test_compare3(int v)
{
	unsigned failures = 0;

	failures += test_compare_both_ways(v, 0, 0);
	failures += test_compare_both_ways(v, 1, 0);
	failures += test_compare_both_ways(v, 1, 1);
	failures += test_compare_both_ways(v, 1, 2);
	failures += test_compare_both_ways(v, -1, 1);
	failures += test_compare_both_ways(v, -1, 0);
	failures += test_compare_both_ways(v, LONG_MAX, LONG_MIN);

	return failures;
}

ECHECK_TEST_MAIN_V(test_compare3)
