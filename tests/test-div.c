/*
test-div.c
Copyright (C) 2016 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/
#include "test-ehbigint-private-utils.h"
#include "../src/ehbigint-log.h"	/* set_ehbi_log_file */

int test_div(int verbose, char *snumerator, char *sdenominator, char *squotient,
	     char *sremainder)
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

	numerator.bytes = bytes_numerator;
	numerator.bytes_len = 10;
	numerator.sign = 0;
	numerator.bytes_used = 0;

	denominator.bytes = bytes_denominator;
	denominator.bytes_len = 10;
	denominator.sign = 0;
	denominator.bytes_used = 0;

	quotient.bytes = bytes_quotient;
	quotient.bytes_len = 10;
	quotient.sign = 0;
	quotient.bytes_used = 0;

	remainder.bytes = bytes_remainder;
	remainder.bytes_len = 10;
	remainder.sign = 0;
	remainder.bytes_used = 0;

	err =
	    ehbi_set_decimal_string(&numerator, snumerator, strlen(snumerator));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&numerator, snumerator, __LINE__, TEST_FUNC);
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
	failures +=
	    check_ehbigint_dec(&denominator, sdenominator, __LINE__, TEST_FUNC);
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

	failures +=
	    check_ehbigint_dec(&quotient, squotient, __LINE__, TEST_FUNC);

	failures +=
	    check_ehbigint_dec(&remainder, sremainder, __LINE__, TEST_FUNC);

	return failures;
}

int log_contains(FILE *log, const char *expected)
{
	char buffer[4096];
	rewind(log);
	while (fgets(buffer, 4096, log)) {
		if (strstr(buffer, expected)) {
			return 0;
		}
	}
	fprintf(stderr, "'%s' not found in log\n", expected);
	return 1;
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

	numerator.bytes = bytes_numerator;
	numerator.bytes_len = 10;
	numerator.sign = 0;
	numerator.bytes_used = 0;

	denominator.bytes = bytes_denominator;
	denominator.bytes_len = 10;
	denominator.sign = 0;
	denominator.bytes_used = 0;

	quotient.bytes = bytes_quotient;
	quotient.bytes_len = 10;
	quotient.sign = 0;
	quotient.bytes_used = 0;

	remainder.bytes = bytes_remainder;
	remainder.bytes_len = 10;
	remainder.sign = 0;
	remainder.bytes_used = 0;

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

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
