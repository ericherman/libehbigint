/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-inc.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_bytes_shift_left(int verbose, const char *val, size_t bytes,
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

	err = ehbi_shift_left(&bi, CHAR_BIT * bytes);
	if (err) {
		Test_log_error1("error %d from ehbi_shift_left\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint(&bi, &expect_bi);

	if (failures) {
		Test_log_error4
		    ("%d failures in test_bytes_shift_left(%s, %lu, %s)\n",
		     failures, val, (unsigned long)bytes, expected);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_bytes_shift_left(v, "0x02", 0, "0x02");
	failures += test_bytes_shift_left(v, "0x03", 1, "0x0300");
	failures += test_bytes_shift_left(v, "0x05", 2, "0x050000");
	failures += test_bytes_shift_left(v, "0x17", 3, "0x17000000");
	failures += test_bytes_shift_left(v, "0x00FF", 3, "0x00FF000000");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
