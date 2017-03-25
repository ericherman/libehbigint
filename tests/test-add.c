/*
test-add.c
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

int test_add(int verbose, const char *str_1, const char *str_2,
	     const char *str_3)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;
	bi1.sign = 0;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;
	bi2.sign = 0;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;
	bi3.sign = 0;

	err = ehbi_set_decimal_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_decimal_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_add(&bi3, &bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_add\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&bi3, str_3);

	if (failures) {
		Test_log_error1("%d failures in test_add\n", failures);
	}

	return failures;
}

int test_add_hex(int verbose, const char *str_1, const char *str_2,
		 const char *str_3)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;
	bi1.sign = 0;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;
	bi2.sign = 0;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;
	bi3.sign = 0;

	err = ehbi_set_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_add(&bi3, &bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_add\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_hex(&bi3, str_3);

	if (failures) {
		Test_log_error1("%d failures in test_add\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	/*      char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F20000F0000000000A";
	const char *str_2__ = "0x32000020000000000A";
	const char *str_3 = "0x01240001100000000014";

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_add_hex(v, str_1, str_2__, str_3);

	failures += test_add(v, "1000", "2000", "3000");
	failures += test_add(v, "0", "5000", "5000");
	failures += test_add(v, "12000", "13000", "25000");
	failures += test_add(v, "-1", "-2", "-3");
	failures += test_add(v, "1", "-2", "-1");
	failures += test_add(v, "-6", "2", "-4");
	failures += test_add(v, "-6", "11", "5");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
