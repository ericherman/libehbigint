/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-n-choose-k.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_n_choose_k(int verbose, const char *nstr, const char *kstr,
		    const char *expectstr)
{
	int err, failures;

	struct ehbigint n;
	unsigned char n_bytes[20];
	struct ehbigint k;
	unsigned char k_bytes[20];
	struct ehbigint res;
	unsigned char res_bytes[20];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&n, n_bytes, 20);
	ehbi_init(&k, k_bytes, 20);
	ehbi_init(&res, res_bytes, 20);

	err = ehbi_set_decimal_string(&n, nstr, strlen(nstr));
	if (err) {
		Test_log_error2("error %d from ehbi_set_decimal_string %s\n",
				err, nstr);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	err = ehbi_set_decimal_string(&k, kstr, strlen(kstr));
	if (err) {
		Test_log_error2("error %d from ehbi_set_decimal_string %s\n",
				err, kstr);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_n_choose_k(&res, &n, &k);
	if (err) {
		Test_log_error1("error %d from ehbi_n_choose_k\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&res, expectstr);

	if (failures) {
		Test_log_error4
		    ("%d failures in test_n_choose_k(n=>%s, k=>%s, expect=>%s)\n",
		     failures, nstr, kstr, expectstr);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_n_choose_k(v, "4", "2", "6");
	failures += test_n_choose_k(v, "5", "3", "10");
	failures += test_n_choose_k(v, "40", "20", "137846528820");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
