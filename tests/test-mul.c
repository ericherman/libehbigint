/*
test-mul.c
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

int test_mul(int verbose, long al, long bl, char *expected)
{
	int err, failures;

	unsigned char a_bytes[16];
	struct ehbigint a_bigint;

	unsigned char b_bytes[16];
	struct ehbigint b_bigint;

	unsigned char result_bytes[16];
	struct ehbigint result;

	char buf[BUFLEN];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, a_bytes, 16);
	ehbi_init(&b_bigint, b_bytes, 16);
	ehbi_init(&result, result_bytes, 16);

	sprintf(buf, "%ld", al);
	err = ehbi_set_l(&a_bigint, al);
	if (err) {
		Test_log_error1("error %d from ehbi_set_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&a_bigint, buf);
	if (failures) {
		Test_log_error1("assign failed %s\n", buf);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	sprintf(buf, "%ld", bl);
	err = ehbi_set_l(&b_bigint, bl);
	if (err) {
		Test_log_error1("error %d from ehbi_set_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&b_bigint, buf);
	if (failures) {
		Test_log_error1("assign failed %s\n", buf);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_mul(&result, &a_bigint, &b_bigint);
	if (err) {
		Test_log_error1("error %d from ehbi_mul\n", err);
	}

	failures += Check_ehbigint_dec(&result, expected);

	if (failures) {
		Test_log_error4("%d failures in test_mul(%lu,%lu,%s)\n",
				failures, al, bl, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_mul(v, 2, 256, "512");

	failures += test_mul(v, 7, -3, "-21");
	failures += test_mul(v, -41, -2, "82");

	failures += test_mul(v, 500, 333, "166500");

	/*
	   $ bc <<< "9415273 * 252533"
	   2377667136509
	 */
	failures += test_mul(v, 9415273, 252533, "2377667136509");
	failures += test_mul(v, 239862259L, 581571519L, "139497058317401421");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
