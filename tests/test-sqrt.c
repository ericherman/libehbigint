/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-sqrt.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include "../src/ehbigint-log.h"	/* set_ehbi_log_file */

int test_sqrt(int verbose, const char *sval, const char *ssqrt,
	      const char *sremainder)
{
	int err, failures;

	unsigned char bytes_val[50];
	unsigned char bytes_sqrt[50];
	unsigned char bytes_remainder[50];

	struct ehbigint val;
	struct ehbigint sqrt;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&val, bytes_val, 50);
	ehbi_init(&sqrt, bytes_sqrt, 50);
	ehbi_init(&remainder, bytes_remainder, 50);

	err = ehbi_set_decimal_string(&val, sval, strlen(sval));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&val, sval);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sval);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_sqrt(&sqrt, &remainder, &val);
	if (err) {
		Test_log_error1("error %d from ehbi_div\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&sqrt, ssqrt);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	return failures;
}

int test_sqrt_negative(int verbose)
{
	int err, failures;
	FILE *log;

	unsigned char bytes_val[10];
	unsigned char bytes_sqrt[10];
	unsigned char bytes_remainder[10];

	struct ehbigint val;
	struct ehbigint sqrt;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;
	log = tmpfile();
	set_ehbi_log_file(log);

	ehbi_init(&val, bytes_val, 10);
	ehbi_init(&sqrt, bytes_sqrt, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	ehbi_set_l(&val, -100);

	err = ehbi_sqrt(&sqrt, &remainder, &val);
	if (!err) {
		++failures;
		Test_log_error("no error from ehbi_sqrt of negative?\n");
	}
	failures += log_contains(log, "complex");
	fclose(log);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_sqrt(v, "100", "10", "0");
	failures += test_sqrt(v, "10001", "100", "1");
	failures += test_sqrt(v, "10000000000000001", "100000000", "1");
	failures +=
	    test_sqrt(v, "10000000000000000000001", "100000000000", "1");
	failures +=
	    test_sqrt(v,
		      "22934986159900715116108208953020869407965649891682811",
		      "151443012912120561509118328",
		      "111863686247280161986167227");

	failures += test_sqrt(v, "0", "0", "0");
	failures += test_sqrt_negative(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
