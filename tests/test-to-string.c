/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-to-string.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <limits.h>		/* LONG_MAX */

unsigned test_to_string_65605(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	unsigned failures;

	unsigned char bytes[4] = { 0x00, 0x01, 0x00, 0x45 };
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = bytes;
	a_bigint.bytes_len = 4;
	a_bigint.sign = 0;
	a_bigint.bytes_used = 3;

	failures += Check_ehbigint_hex(&a_bigint, "0x010045");
	failures += Check_ehbigint_dec(&a_bigint, "65605");

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_to_string_65605");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_to_string_negative_3(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	unsigned failures;

	unsigned char bytes[20];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes, 20);
	bi.bytes[19] = 0x03;
	bi.bytes_used = 1;
	bi.sign = 1;

	failures += Check_ehbigint_hex(&bi, "0x03");
	failures += Check_ehbigint_dec(&bi, "-3");

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_to_string_negative_3");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_to_string_ld(int verbose, long val)
{
	struct eembed_log *log = eembed_err_log;
	unsigned failures;
	unsigned char bytes[1 + sizeof(long)];
	struct ehbigint bi;
	char expected[80];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi, bytes, 1 + sizeof(long));

	ehbi_set_l(&bi, val);

	eembed_long_to_str(expected, 80, val);
	failures += Check_ehbigint_dec(&bi, expected);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_to_string_ld(");
		log->append_l(log, val);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_to_string(int v)
{
	unsigned failures = 0;

	failures += test_to_string_65605(v);

	failures += test_to_string_negative_3(v);

	failures += test_to_string_ld(v, 0L);
	failures += test_to_string_ld(v, 3L);
	failures += test_to_string_ld(v, -3L);
	failures += test_to_string_ld(v, 12341234L);
	failures += test_to_string_ld(v, LONG_MIN);
	failures += test_to_string_ld(v, LONG_MAX);

	return failures;
}

ECHECK_TEST_MAIN_V(test_to_string)
