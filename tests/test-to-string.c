/*
test-to-string.c
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
#include <limits.h>		/* LONG_MAX */

int test_to_string_65605(int verbose)
{
	int failures;

	unsigned char bytes[4] = { 0x00, 0x01, 0x00, 0x45 };
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = bytes;
	a_bigint.bytes_len = 4;
	a_bigint.sign = 0;
	a_bigint.bytes_used = 3;

	failures += Check_ehbigint_hex(&a_bigint, "0x010045");
	failures += Check_ehbigint_dec(&a_bigint, "65605");

	if (failures) {
		Test_log_error1("%d failures in test_to_string\n", failures);
	}

	return failures;
}

int test_to_string_negative_3(int verbose)
{
	int failures;

	unsigned char bytes[20];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes, 20);
	bi.bytes[19] = 0x03;
	bi.bytes_used = 1;
	bi.sign = 1;

	failures += Check_ehbigint_hex(&bi, "0x03");
	failures += Check_ehbigint_dec(&bi, "-3");

	if (failures) {
		Test_log_error1("%d failures in test_to_string_negative_3\n",
				failures);
	}

	return failures;
}

int test_to_string_ld(int verbose, long val)
{
	int failures;
	unsigned char bytes[1 + sizeof(long)];
	struct ehbigint bi;
	char expected[80];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes, 1 + sizeof(long));

	ehbi_set_l(&bi, val);

	sprintf(expected, "%ld", val);
	failures += Check_ehbigint_dec(&bi, expected);

	if (failures) {
		Test_log_error1("%d failures in test_to_string_ld\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_to_string_65605(v);

	failures += test_to_string_negative_3(v);

	failures += test_to_string_ld(v, 0L);
	failures += test_to_string_ld(v, 3L);
	failures += test_to_string_ld(v, -3L);
	failures += test_to_string_ld(v, 12341234L);
	failures += test_to_string_ld(v, LONG_MIN);
	failures += test_to_string_ld(v, LONG_MAX);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
