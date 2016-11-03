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
#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"

int test_to_string(int verbose)
{
	int failures;

	unsigned char bytes[4] = { 0x00, 0x01, 0x00, 0x45 };
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = bytes;
	a_bigint.bytes_len = 4;
	a_bigint.bytes_used = 3;

	failures +=
	    check_ehbigint_hex(&a_bigint, "0x010045", __LINE__, TEST_FUNC);
	failures += check_ehbigint_dec(&a_bigint, "65605", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_to_string\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_to_string(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
