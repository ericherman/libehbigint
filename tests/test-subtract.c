/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-subtract.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_subtract(int verbose, const char *str_1, const char *str_2,
		  const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);
	ehbi_init(&bi3, bytes_buf3, 20);

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

	failures += Check_ehbigint_dec(&bi3, expect);

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

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi2, bytes_buf2, 20);
	ehbi_init(&bi3, bytes_buf3, 14);

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

	failures += Check_ehbigint_hex(&bi3, str_3);

	if (failures) {
		Test_log_error1("%d failures in test_subtract\n", failures);
	}

	return failures;
}

int test_subtract_l(int verbose, const char *str_1, long l, const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi3;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);
	ehbi_init(&bi3, bytes_buf3, 20);

	err = ehbi_set_decimal_string(&bi1, str_1, strlen(str_1));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_subtract_l(&bi3, &bi1, l);
	if (err) {
		Test_log_error1("error %d from ehbi_subtract\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&bi3, expect);

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
	failures += test_subtract(v, "35813", "65521", "-29708");

	failures += test_subtract_big(v);

	failures += test_subtract_l(v, "35813", 65521, "-29708");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
