/*
test-inc-ul.c
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

int test_inc_l(int verbose, const char *dec_str1, long v2, const char *expect)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi1;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);

	err = ehbi_set_decimal_string(&bi1, dec_str1, strlen(dec_str1));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_inc_l(&bi1, v2);
	if (err) {
		Test_log_error1("error %d from ehbi_inc_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&bi1, expect);

	if (failures) {
		Test_log_error1("%d failures in test_inc_l\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_inc_l(v, "700000000000", 134124, "700000134124");
	failures += test_inc_l(v, "700000000000", -2, "699999999998");
	failures += test_inc_l(v, "1", -3, "-2");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
