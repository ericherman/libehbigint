/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-from-decimal-to-decimal-round-trip.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned from_dec_to_dec_round_trip(int verbose, const char *dec)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	int is_negative;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, bytes_buf, 20, &err);

	is_negative = dec[0] == '-';
	ehbi_set_decimal_string(&a_bigint, dec, eembed_strlen(dec), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, dec);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	if (ehbi_is_negative(&a_bigint, &err) != is_negative || err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " ehbi_is_negative != ");
		log->append_l(log, is_negative);
		log->append_s(log, " for ");
		log->append_s(log, dec);
		log->append_eol(log);
	}

	ehbi_to_decimal_string(&a_bigint, as_string, BUFLEN, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, as_string);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
	}

	failures += check_str_m(as_string, dec, "round-trip");

	return failures;
}

unsigned test_from_decimal_to_decimal_round_trip(int v)
{
	unsigned failures = 0;
	const char *dec_str = NULL;

	/* u64_max = "18446744073709551615" */
	dec_str = "12345678901234567890000";
	failures += from_dec_to_dec_round_trip(v, dec_str);

	dec_str = "200";
	failures += from_dec_to_dec_round_trip(v, dec_str);

	dec_str = "-1";
	failures += from_dec_to_dec_round_trip(v, dec_str);

	return failures;
}

ECHECK_TEST_MAIN_V(test_from_decimal_to_decimal_round_trip)
