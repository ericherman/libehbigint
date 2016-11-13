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

int test_div(int verbose)
{
	int err, failures;

	unsigned long ulnumerator = 287713;
	unsigned long uldenominator = 571;
	unsigned long uliquotient = 503;
	unsigned long ulremainder = 500;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	char as_string[BUFLEN];
	char expected[BUFLEN];
	char *str;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	numerator.bytes = bytes_numerator;
	numerator.bytes_len = 10;
	numerator.bytes_used = 0;

	denominator.bytes = bytes_denominator;
	denominator.bytes_len = 10;
	denominator.bytes_used = 0;

	quotient.bytes = bytes_quotient;
	quotient.bytes_len = 10;
	quotient.bytes_used = 0;

	remainder.bytes = bytes_remainder;
	remainder.bytes_len = 10;
	remainder.bytes_used = 0;

	sprintf(as_string, "%lu", ulnumerator);
	err = ehbi_set_decimal_string(&numerator, as_string, strlen(as_string));
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&numerator, as_string, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("round trip failed %lu\n", ulnumerator);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	sprintf(as_string, "%lu", uldenominator);
	err =
	    ehbi_set_decimal_string(&denominator, as_string, strlen(as_string));
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&denominator, as_string, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("round trip failed %lu\n", uldenominator);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		LOG_ERROR1("error %d from ehbi_div\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	sprintf(expected, "%lu", uliquotient);
	failures +=
	    check_ehbigint_dec(&quotient, expected, __LINE__, TEST_FUNC);

	sprintf(expected, "%lu", ulremainder);
	failures +=
	    check_ehbigint_dec(&remainder, expected, __LINE__, TEST_FUNC);

	/*
	   lldiv_t result = lldiv(5088824049625,33554393);
	   result.quot: 151658, result.remainder: 31916031
	 */
	str = "5088824049625";
	err = ehbi_set_decimal_string(&numerator, str, strlen(str));
	if (err) {
		LOG_ERROR3
		    ("error %d ehbi_set_decimal_string(&numerator, %s, %lu)\n",
		     err, str, (unsigned long)strlen(str));
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	str = "33554393";
	denominator.bytes_used = 0;
	ehbi_set_decimal_string(&denominator, str, strlen(str));
	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		LOG_ERROR1("error %d from ehbi_div\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures +=
	    check_ehbigint_dec(&quotient, "151658", __LINE__, TEST_FUNC);
	failures +=
	    check_ehbigint_dec(&remainder, "31916031", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_div\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_div(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
