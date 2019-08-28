/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-exp.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include "../src/ehbigint-log.h"	/* set_ehbi_log_file */

int test_exp(int verbose, const char *sbase, const char *sexponent,
	     const char *sresult)
{
	int err, failures;

	unsigned char bytes_base[20];
	unsigned char bytes_exponent[20];
	unsigned char bytes_result[40];

	struct ehbigint base;
	struct ehbigint exponent;
	struct ehbigint result;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&base, bytes_base, 20);
	ehbi_init(&exponent, bytes_exponent, 20);
	ehbi_init(&result, bytes_result, 30);

	err = ehbi_set_decimal_string(&base, sbase, strlen(sbase));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&base, sbase);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sbase);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_set_decimal_string(&exponent, sexponent, strlen(sexponent));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&exponent, sexponent);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sexponent);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_exp(&result, &base, &exponent);
	if (err) {
		Test_log_error1("error %d from ehbi_exp\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&result, sresult);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_exp(v, "10", "2", "100");
	failures += test_exp(v, "5", "4", "625");
	failures +=
	    test_exp(v, "1000", "10", "1000000000000000000000000000000");
	failures += test_exp(v, "1000000000000000000000000000000", "0", "1");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
