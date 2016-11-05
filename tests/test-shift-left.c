/*
test-inc.c
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

int test_shift_left(int verbose, char *val, size_t bytes, char *expected)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi.bytes = bytes_buf1;
	bi.bytes_len = 20;

	err = ehbi_from_hex_string(&bi, val, strlen(val));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_bytes_shift_left(&bi, bytes);
	if (err) {
		LOG_ERROR1("error %d from ehbi_bytes_shift_left\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi, expected, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR4("%d failures in test_shift_left(%s, %lu, %s)\n",
			   failures, val, (unsigned long)bytes, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_shift_left(v, "0x02", 0, "0x02");
	failures += test_shift_left(v, "0x03", 1, "0x0300");
	failures += test_shift_left(v, "0x05", 2, "0x050000");
	failures += test_shift_left(v, "0x17", 3, "0x17000000");
	failures += test_shift_left(v, "0x00FF", 3, "0x00FF000000");

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
