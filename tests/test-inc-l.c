/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-inc-ul.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_inc_l(int verbose, const char *dec_str1, long v2, const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi1;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);

	err = ehbi_set_decimal_string(&bi1, dec_str1, strlen(dec_str1));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_inc_l(&bi1, v2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&bi1, expect);

	if (failures) {
		Test_log_error1("%d failures in test_inc_l\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_inc_l(v, "700000000000", 134124, "700000134124");
	failures += test_inc_l(v, "700000000000", -2, "699999999998");
	failures += test_inc_l(v, "1", -3, "-2");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
