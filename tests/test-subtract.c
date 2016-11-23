/*
test-subtract.c
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

int test_subtract(int verbose, const char *str_1, const char *str_2,
		  const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[14];
	struct ehbigint bi1, bi2, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;

	err = ehbi_set_decimal_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_decimal_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_subtract(&bi3, &bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_subtract\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_dec(&bi3, expect, __LINE__, TEST_FUNC);

	return failures;
}

int test_subtract_big(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[14];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =    "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x0F20100F00002202040A";
	const char *str_2 = "0x0320000200004404020A";
	const char *str_3 = "0x0C00100CFFFFDDFE0200";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;
	bi1.sign = 0;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;
	bi2.sign = 0;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 14;
	bi3.sign = 0;

	err = ehbi_set_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_subtract(&bi3, &bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_subtract\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi3, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		Test_log_error1("%d failures in test_subtract\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_subtract(v, "257", "255", "2");
	failures += test_subtract(v, "0", "23", "-23");
	failures += test_subtract(v, "5", "7", "-2");
	failures += test_subtract(v, "2", "11", "-9");
	failures += test_subtract(v, "-11", "2", "-13");
	failures += test_subtract(v, "-5", "-2", "-3");
	failures += test_subtract(v, "-17", "-27", "10");

	failures += test_subtract_big(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
