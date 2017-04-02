/*
test-dec.c
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

int test_dec_hex(int verbose, const char *str_1, const char *str_2,
		 const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);

	err = ehbi_set_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_hex(&bi1, expect);

	if (failures) {
		Test_log_error1("%d failures in test_dec\n", failures);
	}

	return failures;
}

int test_dec(int verbose, const char *str_1, const char *str_2,
	     const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, BILEN);
	ehbi_init(&bi2, bytes_buf2, BILEN);

	err = ehbi_set_decimal_string(&bi1, str_1, strlen(str_1));
	err += ehbi_set_decimal_string(&bi2, str_2, strlen(str_2));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&bi1, expect);

	if (failures) {
		Test_log_error1("%d failures in test_dec\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	const char *hexstr_1, *hexstr_2, *hexexpct;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	failures = 0;

	failures += test_dec(v, "5", "2", "3");
	failures += test_dec(v, "7", "0", "7");
	failures += test_dec(v, "0", "1", "-1");
	failures += test_dec(v, "9", "11", "-2");

	/* u64_max = "0xFFFFFFFFFFFFFFFF" */
	hexstr_1 = "0x700002000000000001";
	hexstr_2 = "0x100000100000000001";
	hexexpct = "0x600001F00000000000";
	failures += test_dec_hex(v, hexstr_1, hexstr_2, hexexpct);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
