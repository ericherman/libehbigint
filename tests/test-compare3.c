/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare3.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <limits.h>

int test_compare3(int verbose, long a, long b)
{
	int err, failures, result, result_l;
	unsigned char bytes_buf1[1 + sizeof(long)];
	unsigned char bytes_buf2[sizeof(long)];
	struct ehbigint bi_a, bi_b;

	VERBOSE_ANNOUNCE(verbose);
	if (verbose) {
		fprintf(stderr, "(%ld, %ld)\n", a, b);
	}

	failures = 0;

	ehbi_init(&bi_a, bytes_buf1, 1 + sizeof(long));
	ehbi_init(&bi_b, bytes_buf2, sizeof(long));

	err = ehbi_set_l(&bi_a, a);
	if (err) {
		Test_log_error2("error %d from ehbi_set_l(%ld)\n", err, a);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	err = ehbi_set_l(&bi_b, b);
	if (err) {
		Test_log_error2("error %d from ehbi_set_l(%ld)\n", err, b);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_compare(&bi_a, &bi_b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_compare\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int((result == 0), (a == b));
	failures += check_int((result > 0), (a > b));
	failures += check_int((result < 0), (a < b));

	result_l = ehbi_compare_l(&bi_a, b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_compare_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, result_l);

	result = ehbi_less_than(&bi_a, &bi_b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_less_than\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result == 0, !(a < b));
	result_l = ehbi_less_than_l(&bi_a, b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_less_than_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, result_l);

	result = ehbi_greater_than(&bi_a, &bi_b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_greater_than\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int((result == 0), !(a > b));
	result_l = ehbi_greater_than_l(&bi_a, b, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_greater_than_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, result_l);

	if (failures) {
		Test_log_error1("%d failures in test_compare\n", failures);
	}

	return failures;
}

int test_compare_both_ways(int verbose, long a, long b)
{
	return test_compare3(verbose, a, b) + test_compare3(verbose, b, a);
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_compare_both_ways(v, 0, 0);
	failures += test_compare_both_ways(v, 1, 0);
	failures += test_compare_both_ways(v, 1, 1);
	failures += test_compare_both_ways(v, 1, 2);
	failures += test_compare_both_ways(v, -1, 1);
	failures += test_compare_both_ways(v, -1, 0);
	failures += test_compare_both_ways(v, LONG_MAX, LONG_MIN);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
