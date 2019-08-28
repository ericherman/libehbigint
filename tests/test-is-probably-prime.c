/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-is-probably-prime.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

int test_is_probably_prime(int verbose, const char *val, int expected)
{
	int err, failures, actual;
	unsigned char bytes_buf1[20];
	char buf[60];
	struct ehbigint bi;
	unsigned accuracy;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes_buf1, 20);

	err = ehbi_set_decimal_string(&bi, val, strlen(val));
	if (err) {
		Test_log_error1("error %d from ehbi_set_decimal_string\n", err);
		Test_log_error("Aborting test\n");
		if (1) {
			Test_log_error3("%d failures in test_is_probably_prime."
					" val = %s, expected = %d)\n",
					failures, val, expected);
			exit(EXIT_FAILURE);
		}
		return 1;
	}
	failures +=
	    check_str_m(ehbi_to_decimal_string(&bi, buf, 60, &err), val,
			"failed to round-trip?");
	if (err || failures) {
		Test_log_error("Aborting\n");
		return 1;
	}

	accuracy = EHBI_MIN_TRIALS_FOR_IS_PROBABLY_PRIME;
	actual = ehbi_is_probably_prime(&bi, accuracy, &err);
	if (err) {
		Test_log_error1("error %d from ehbi_is_probably_prime\n", err);
		Test_log_error("Aborting test\n");
		if (1) {
			Test_log_error4("%d failures in test_is_probably_prime."
					" val = %s, actual = %d, expected = %d)\n",
					failures, val, actual, expected);
			exit(EXIT_FAILURE);
		}
		return 1;
	}

	failures += check_int_m(actual, expected, val);

	if (failures) {
		Test_log_error4("%d failures in test_is_probably_prime."
				" val = %s, actual = %d, expected = %d)\n",
				failures, val, actual, expected);
	}

	return failures;
}

static const char *PRIMES[] = {
	"2", "3", "5", "7", "11", "13", "17",
	"853", "857", "859", "863", "877", "881", "883", "887", "907", "911",
	"919", "929", "937", "941", "947", "953", "967", "971", "977",
	"983", "991", "997", "7901", "7907", "7919", "765409", "7654319",
	"982451579", "982451581", "982451609", "982451629", "982451653",
	"9999999769", "9999999781", "9999999787", "9999999817", "9999999833",
	"9999999851", "9999999881", "9999999929", "9999999943", "9999999967",
	"49999999957", "49999999961", "49999999967",
	"900000070289", "900000070331",
	"170141183460469231731687303715884105727",
	NULL
};

static const char *COMPOSITES[] = {
	"-3", "0", "1", "4", "6", "8", "9", "10", "12", "14", "15",
	"851", "861", "867", "873", "879", "885", "889", "903", "909", "913",
	"917", "927", "931", "943", "949", "951", "957", "973", "979", "981",
	"987", "993", "7900", "7902", "7903", "7904", "7905", "7906", "7908",
	"982451577", "982451583", "982451607", "982451611", "982451627",
	"982451631", "982451651", "9999999771", "9999999783", "9999999789",
	"9999999961", "49999999959", "49999999963", "49999999969",
	"900000070301",
	"810000126558004943495659",
	NULL
};

int main(int argc, char **argv)
{
	int v, failures;
	size_t i;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	for (i = 0; PRIMES[i] != NULL; ++i) {
		failures += test_is_probably_prime(v, PRIMES[i], 1);
	}
	for (i = 0; COMPOSITES[i] != NULL; ++i) {
		failures += test_is_probably_prime(v, COMPOSITES[i], 0);
	}

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
