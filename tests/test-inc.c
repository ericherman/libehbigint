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
#include "test-ehbigint-private-utils.h"

int test_inc(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F00000F00000000001";
	const char *str_2 = "0x00100000100000000001";
	const char *str_3 = "0x01000001000000000002";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	err = ehbi_set_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_hex_string(&bi2, str_2, strlen(str_2));
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

	failures += check_ehbigint_hex(&bi1, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		Test_log_error1("%d failures in test_inc\n", failures);
	}

	/*
	   $ echo "9415273 + 3154116455" | bc
	   3163531728
	 */
	err = ehbi_set_ul(&bi1, 9415273UL);
	err += ehbi_set_ul(&bi2, 3154116455UL);
	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&bi1, "3163531728", __LINE__, TEST_FUNC);

	err = ehbi_set_ul(&bi1, 254);
	err += ehbi_set_ul(&bi2, 1);
	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&bi1, "255", __LINE__, TEST_FUNC);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_inc(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
