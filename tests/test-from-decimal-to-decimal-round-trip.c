/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-from-decimal-to-decimal-round-trip.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_from_decimal_to_decimal_round_trip(int verbose, const char *dec)
{
	int err, failures, is_negative;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, bytes_buf, 20);

	is_negative = dec[0] == '-';
	err = ehbi_set_decimal_string(&a_bigint, dec, strlen(dec));
	if (err) {
		Test_log_error1("error %d ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	if (ehbi_is_negative(&a_bigint, &err) != is_negative || err) {
		Test_log_error3("ehbi_is_negative != %d for %s, (error:%d)\n",
				is_negative, dec, err);
		failures += 1;
	}
	ehbi_to_decimal_string(&a_bigint, as_string, BUFLEN, &err);
	if (err) {
		Test_log_error1("error %d ehbi_to_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_str(as_string, dec);
	if (failures) {
		Test_log_error1("%d failures in "
				"check_decimal_to_decimal_round_trip\n",
				failures);
	}
	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	const char *dec_str;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	/* u64_max = "18446744073709551615" */
	dec_str = "12345678901234567890000";
	failures += test_from_decimal_to_decimal_round_trip(v, dec_str);

	dec_str = "200";
	failures += test_from_decimal_to_decimal_round_trip(v, dec_str);

	dec_str = "-1";
	failures += test_from_decimal_to_decimal_round_trip(v, dec_str);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
