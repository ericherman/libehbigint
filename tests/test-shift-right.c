/*
test-shift-right.c
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

int test_shift_right(int verbose, const char *val, unsigned long bits,
		     const char *expected)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi;
	struct ehbigint expect_bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes_buf1, 20);
	ehbi_init(&expect_bi, bytes_buf2, 20);

	err = ehbi_set_hex_string(&bi, val, strlen(val));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	err = ehbi_set_hex_string(&expect_bi, expected, strlen(expected));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_shift_right(&bi, bits);
	if (err) {
		Test_log_error1("error %d from ehbi_shift_right\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint(&bi, &expect_bi);

	if (failures) {
		Test_log_error4
		    ("%d failures in test_shift_right(%s, %lu, %s)\n",
		     failures, val, bits, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_shift_right(v, "0x02", 0, "0x02");
	failures += test_shift_right(v, "0x0300", 4, "0x30");
	failures += test_shift_right(v, "0x050000", 16, "0x05");
	failures += test_shift_right(v, "0x17000000", 20, "0x0170");
	failures += test_shift_right(v, "0x00FF000000", 24, "0x00FF");
	failures += test_shift_right(v, "0x03A8F057", 1, "0x01D4782B");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
