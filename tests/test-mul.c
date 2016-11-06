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
#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"
int test_mul(int verbose, unsigned long aul, unsigned long bul, char *expected)
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

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 16;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 16;
	b_bigint.bytes_used = 0;

	result.bytes = result_bytes;
	result.bytes_len = 16;

	sprintf(buf, "%lu", aul);
	err = ehbi_set_ul(&a_bigint, aul);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&a_bigint, buf, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("assign failed %s\n", buf);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	sprintf(buf, "%lu", bul);
	err = ehbi_set_ul(&b_bigint, bul);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&b_bigint, buf, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("assign failed %s\n", buf);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_mul(&result, &a_bigint, &b_bigint);
	if (err) {
		LOG_ERROR1("error %d from ehbi_mul\n", err);
	}

	failures += check_ehbigint_dec(&result, expected, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR4("%d failures in test_mul(%lu,%lu,%s)\n", failures,
			   aul, bul, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_mul(v, 2, 256, "512");

	failures += test_mul(v, 500, 333, "166500");

	/*
	   $ bc <<< "9415273 * 252533"
	   2377667136509
	 */
	failures += test_mul(v, 9415273, 252533, "2377667136509");

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
