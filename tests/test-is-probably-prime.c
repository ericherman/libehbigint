/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-is-probably-prime.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_is_probably_prime_s(int verbose, const char *val, int expected)
{
	struct eembed_log *log = eembed_err_log;
	int err, failures, actual;
	const size_t bytes_buf1_len = 20;
	unsigned char bytes_buf1[20];
	const size_t buf_len = 45;
	char buf[45];
	struct ehbigint bi;
	unsigned accuracy;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi, bytes_buf1, bytes_buf1_len);

	ehbi_set_decimal_string(&bi, val, eembed_strlen(val), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, val);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_to_decimal_string(&bi, buf, buf_len, &err);
	failures += check_str_m(buf, val, "failed to round-trip?");
	if (err || failures) {
		Test_log_error("Aborting\n");
		return 1;
	}

	accuracy = EHBI_MIN_TRIALS_FOR_IS_PROBABLY_PRIME;
	err = 0;
	actual = ehbi_is_probably_prime(&bi, accuracy, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_is_probably_prime");
		log->append_eol(log);
	}
	failures += check_int_m(actual, expected, val);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_is_probably_prime_s.");
		log->append_s(log, " val == ");
		log->append_s(log, val);
		log->append_s(log, ", actual == ");
		log->append_l(log, actual);
		log->append_s(log, ", expected == ");
		log->append_l(log, expected);
		log->append_eol(log);
	}

	return failures;
}

unsigned test_is_probably_prime(int v)
{
	const char *Primes[] = {
		"2", "3", "5", "7", "11", "13", "17",
		"853", "857", "859", "863", "877", "881", "883", "887", "907",
#if EEMBED_HOSTED
		"911", "919", "929", "937", "941", "947", "953", "967", "971",
		"977", "983", "991", "997",
		"7901", "7907", "7919",
		"765409", "7654319", "982451579", "982451581", "982451609",
		"982451629", "982451653", "9999999769", "9999999781",
		"9999999787", "9999999817", "9999999833", "9999999851",
		"9999999881", "9999999929", "9999999943", "9999999967",
		"49999999957", "49999999961", "49999999967",
#endif
		"900000070289", "900000070331",
		"170141183460469231731687303715884105727",
		NULL
	};

	const char *Composites[] = {
		"-3", "0", "1", "4", "6", "8", "9", "10", "12", "14", "15",
		"851", "861", "867", "873", "879", "885", "889", "903", "909",
#if EEMBED_HOSTED
		"913", "917", "927", "931", "943", "949", "951", "957", "973",
		"979", "981", "987", "993", "7900", "7902", "7903", "7904",
		"7905", "7906", "7908",
		"982451577", "982451583", "982451607", "982451611", "982451627",
		"982451631", "982451651", "9999999771", "9999999783",
		"9999999789", "9999999961", "49999999959", "49999999963",
#endif
		"49999999969", "900000070301",
		"810000126558004943495659",
		NULL
	};

	unsigned failures = 0;
	size_t i = 0;
	int isprime = 0;

	for (i = 0; Primes[i] != NULL; ++i) {
		isprime = 1;
		failures += test_is_probably_prime_s(v, Primes[i], isprime);
	}

	for (i = 0; Composites[i] != NULL; ++i) {
		isprime = 0;
		failures += test_is_probably_prime_s(v, Composites[i], isprime);
	}

	return failures;
}

ECHECK_TEST_MAIN_V(test_is_probably_prime)
