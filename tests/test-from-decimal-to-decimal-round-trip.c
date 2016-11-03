/*
test-from-decimal-to-decimal-round-trip.c
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

int test_from_decimal_to_decimal_round_trip(int verbose)
{
	int err, failures;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];

	/*   const char *u64_max =    "18446744073709551615" */
	const char *expected_str = "12345678901234567890000";

	struct ehbigint a_bigint;

	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err =
	    ehbi_from_decimal_string(&a_bigint, expected_str,
				     strlen(expected_str));

	if (err) {
		LOG_ERROR1("error %d ehbi_from_decimal_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_to_decimal_string(&a_bigint, as_string, BUFLEN);
	if (err) {
		LOG_ERROR1("error %d ehbi_to_decimal_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(as_string, expected_str);

	if (failures) {
		LOG_ERROR1("%d failures in "
			   "check_decimal_to_decimal_round_trip\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_from_decimal_to_decimal_round_trip(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
