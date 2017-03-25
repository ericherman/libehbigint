/*
test-bytes-shift-right.c
Copyright (C) 2017 Eric Herman <eric@freesa.org>

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

int test_bytes_shift_right(int verbose, char *val, size_t bytes, char *expected)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi.bytes = bytes_buf1;
	bi.bytes_len = 20;
	bi.sign = 0;

	err = ehbi_set_hex_string(&bi, val, strlen(val));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_bytes_shift_right(&bi, bytes);
	if (err) {
		Test_log_error1("error %d from ehbi_bytes_shift_right\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_hex(&bi, expected);

	if (failures) {
		Test_log_error4
		    ("%d failures in test_bytes_shift_right(%s, %lu, %s)\n",
		     failures, val, (unsigned long)bytes, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_bytes_shift_right(v, "0x02", 0, "0x02");
	failures += test_bytes_shift_right(v, "0x0300", 1, "0x03");
	failures += test_bytes_shift_right(v, "0x050000", 2, "0x05");
	failures += test_bytes_shift_right(v, "0x17000000", 3, "0x17");
	failures += test_bytes_shift_right(v, "0x00FF000000", 3, "0x00FF");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}