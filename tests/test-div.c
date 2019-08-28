/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-div.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include "../src/ehbigint-log.h"	/* set_ehbi_log_file */

int test_div(int verbose, const char *snumerator, const char *sdenominator,
	     const char *squotient, const char *sremainder)
{
	int err, failures;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&denominator, bytes_denominator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	err =
	    ehbi_set_decimal_string(&numerator, snumerator, strlen(snumerator));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&numerator, snumerator);
	if (failures) {
		Test_log_error1("round trip failed %s\n", snumerator);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err =
	    ehbi_set_decimal_string(&denominator, sdenominator,
				    strlen(sdenominator));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&denominator, sdenominator);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sdenominator);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		Test_log_error1("error %d from ehbi_div\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&quotient, squotient);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	return failures;
}

int test_div_l(int verbose, const char *snumerator, long ldenominator,
	       const char *squotient, const char *sremainder)
{
	int err, failures;

	unsigned char bytes_numerator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	err =
	    ehbi_set_decimal_string(&numerator, snumerator, strlen(snumerator));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&numerator, snumerator);
	if (failures) {
		Test_log_error1("round trip failed %s\n", snumerator);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_div_l(&quotient, &remainder, &numerator, ldenominator);
	if (err) {
		Test_log_error1("error %d from ehbi_div\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&quotient, squotient);

	failures += Check_ehbigint_dec(&remainder, sremainder);

	return failures;
}

int test_div_by_zero(int verbose)
{
	int err, failures;
	FILE *log;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;
	log = tmpfile();
	set_ehbi_log_file(log);

	ehbi_init(&numerator, bytes_numerator, 10);
	ehbi_init(&denominator, bytes_denominator, 10);
	ehbi_init(&quotient, bytes_quotient, 10);
	ehbi_init(&remainder, bytes_remainder, 10);

	ehbi_set_l(&numerator, 10);
	ehbi_set_l(&denominator, 0);

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (!err) {
		++failures;
		Test_log_error("no error from ehbi_div by zero?\n");
	}
	failures += log_contains(log, "denominator == 0");
	fclose(log);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_div(v, "20000", "100", "200", "0");
	failures += test_div(v, "20001", "100", "200", "1");
	failures += test_div(v, "20013", "200", "100", "13");
	failures += test_div(v, "287713", "571", "503", "500");

	failures += test_div(v, "-13", "6", "-2", "1");
	failures += test_div(v, "20", "-7", "-2", "6");
	failures += test_div(v, "-100", "-9", "11", "1");

	/*
	   lldiv_t result = lldiv(5088824049625,33554393);
	   result.quot: 151658, result.remainder: 31916031
	 */
	failures +=
	    test_div(v, "5088824049625", "33554393", "151658", "31916031");

	failures += test_div_by_zero(v);

	failures += test_div_l(v, "-13", 6, "-2", "1");
	failures += test_div_l(v, "600851475143", 65521, "9170364", "55499");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
