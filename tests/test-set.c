/*
test-set.c
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

int test_set(int verbose, long lval, const char *expect)
{
	int err, failures;

	unsigned char a_bytes[10];
	struct ehbigint a_bigint;

	unsigned char b_bytes[10];
	struct ehbigint b_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, a_bytes, 10);
	ehbi_init(&b_bigint, b_bytes, 10);

	err = ehbi_set_l(&a_bigint, lval);
	if (err) {
		Test_log_error1("error %d from ehbi_set_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&a_bigint, expect);

	err = ehbi_set(&b_bigint, &a_bigint);
	if (err) {
		Test_log_error1("error %d from ehbi_set_l\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&b_bigint, expect);

	if (failures) {
		Test_log_error1("%d failures in test_set\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_set(v, 3, "3");
	failures += test_set(v, -5, "-5");

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
