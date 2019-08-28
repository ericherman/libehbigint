/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-inc.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_inc_hex(int verbose, const char *hexs1, const char *hexs2,
		 const char *expct)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);

	err = ehbi_set_hex_string(&bi1, hexs1, strlen(hexs1));
	err += ehbi_set_hex_string(&bi2, hexs2, strlen(hexs2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_hex(&bi1, expct);

	if (failures) {
		Test_log_error1("%d failures in test_inc_hex\n", failures);
	}
	return failures;
}

int test_inc(int verbose, const char *v1, const char *v2, const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);

	err = ehbi_set_decimal_string(&bi1, v1, strlen(v1));
	err += ehbi_set_decimal_string(&bi2, v2, strlen(v2));
	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&bi1, expect);

	check_int(bi1.sign, expect[0] == '-');

	if (failures) {
		Test_log_error1("%d failures in test_inc\n", failures);
	}
	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	const char *hexs1, *hexs2, *expct;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_inc(v, "254", "1", "255");
	failures += test_inc(v, "254", "-1", "253");
	failures += test_inc(v, "-1", "1", "0");

	/*
	   $ echo "9415273 + 3154116455" | bc
	   3163531728
	 */
	failures += test_inc(v, "9415273", "3154116455", "3163531728");

	/* u64max = "0xFFFFFFFFFFFFFFFF" */
	hexs1 = "0x00F00000F00000000001";
	hexs2 = "0x00100000100000000001";
	expct = "0x01000001000000000002";
	failures += test_inc_hex(v, hexs1, hexs2, expct);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
