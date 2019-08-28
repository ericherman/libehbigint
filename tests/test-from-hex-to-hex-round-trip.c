/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-from-hex-to-hex-round-trip.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_from_hex_to_hex_round_trip(int verbose)
{
	int err, failures;
	unsigned char bytes_buf[20];

	/*         char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *expected_str = "0x123456789ABCDEF012";

	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, bytes_buf, 20);

	err =
	    ehbi_set_hex_string(&a_bigint, expected_str, strlen(expected_str));

	if (err) {
		Test_log_error1("error %d returned from ehbi_set_hex_string\n",
				err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_hex(&a_bigint, expected_str);

	if (failures) {
		Test_log_error1("%d failures in test_hex_round_trip\n",
				failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_from_hex_to_hex_round_trip(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
