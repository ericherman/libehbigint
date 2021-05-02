/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-from-hex-to-hex-round-trip.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_from_hex_to_hex_round_trip(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf[20];

	/*         char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *expected_str = "0x123456789ABCDEF012";

	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&a_bigint, bytes_buf, 20);

	ehbi_set_hex_string(&a_bigint, expected_str,
			    eembed_strlen(expected_str), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_hex_string(");
		log->append_s(log, expected_str);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	failures += Check_ehbigint_hex(&a_bigint, expected_str);

	return failures;
}

ECHECK_TEST_MAIN_V(test_from_hex_to_hex_round_trip)
