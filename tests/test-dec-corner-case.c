/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-dec-corner-case.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_dec_corner_case(int verbose)
{
	int err, failures;
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

	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);
	ehbi_init(&expect_bi, bytes_buf3, BILEN);

	err = ehbi_set_decimal_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_decimal_string(&bi2, str_2, strlen(str_2));
	err += ehbi_set_decimal_string(&expect_bi, str_3, strlen(str_3));
	if (err) {
		Test_log_error1("error %d ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_subtract\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint(&bi1, &expect_bi);

	if (failures) {
		Test_log_error1("%d failures in test_subtract\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_dec_corner_case(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
