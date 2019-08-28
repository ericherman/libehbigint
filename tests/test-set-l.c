/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set-l.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_set_l(int verbose, long v1)
{
	int failures, err;

	char expect[25], buf[80];
	unsigned char bytes[4];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes, 4);

	err = ehbi_set_l(&bi, 0);
	failures += check_int_m(err, 0, "ehbi_set_l(0)");
	failures += Check_ehbigint_hex(&bi, "0x00");
	failures += Check_ehbigint_dec(&bi, "0");

	sprintf(buf, "ehbi_set_l(%ld)", v1);
	err = ehbi_set_l(&bi, v1);
	failures += check_int_m(err, 0, buf);

	sprintf(expect, "%ld", v1);
	failures += Check_ehbigint_dec(&bi, expect);

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
	failures += test_set_l(v, -1);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
