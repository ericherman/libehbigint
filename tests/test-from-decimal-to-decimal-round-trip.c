/*
test-from-decimal-to-decimal-round-trip.c
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

int test_from_decimal_to_decimal_round_trip(int verbose,
					    const char *expected_str)
{
	int err, failures, is_negative;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];
	struct ehbigint a_bigint;
	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;
	VERBOSE_ANNOUNCE(verbose);
	failures = 0;
	is_negative = expected_str[0] == '-';
	err =
	    ehbi_set_decimal_string(&a_bigint, expected_str,
				    strlen(expected_str));
	if (err) {
		Test_log_error1("error %d ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	if (ehbi_is_negative(&a_bigint, &err) != is_negative || err) {
		Test_log_error3("ehbi_is_negative != %d for %s, (error:%d)\n",
				is_negative, expected_str, err);
		failures += 1;
	}
	err = ehbi_to_decimal_string(&a_bigint, as_string, BUFLEN);
	if (err) {
		Test_log_error1("error %d ehbi_to_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += check_str(as_string, expected_str);
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
	char *str_val;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	/* u64_max = "18446744073709551615" */
	str_val = "12345678901234567890000";
	failures += test_from_decimal_to_decimal_round_trip(v, str_val);

	str_val = "200";
	failures += test_from_decimal_to_decimal_round_trip(v, str_val);

	str_val = "-1";
	failures += test_from_decimal_to_decimal_round_trip(v, str_val);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
