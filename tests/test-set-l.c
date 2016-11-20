/*
test-set-ul.c
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

int test_set_l(int verbose, long v1)
{
	int failures, err;

	char expect[25], buf[80];
	unsigned char bytes[4];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi.bytes = bytes;
	bi.bytes_len = 4;
	bi.sign = 0;

	err = ehbi_set_l(&bi, 0);
	failures += check_int_m(err, 0, "ehbi_set_l(0)");
	failures += check_ehbigint_hex(&bi, "0x00", __LINE__, TEST_FUNC);
	failures += check_ehbigint_dec(&bi, "0", __LINE__, TEST_FUNC);

	sprintf(buf, "ehbi_set_l(%ld)", v1);
	err = ehbi_set_l(&bi, v1);
	failures += check_int_m(err, 0, buf);

	sprintf(expect, "%ld", v1);
	failures += check_ehbigint_dec(&bi, expect, __LINE__, TEST_FUNC);

	if (failures) {
		Test_log_error1("%d failures in test_set_l\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_set_l(v, 1234567890);
/*
	failures += test_set_l(v, -1);
*/

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
