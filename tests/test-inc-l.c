/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-inc-ul.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_inc_lv(int verbose, const char *dec_str1, long v2,
		     const char *expect)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi1;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bi1, bytes_buf1, 20);

	err = ehbi_set_decimal_string(&bi1, dec_str1, eembed_strlen(dec_str1));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, dec_str1);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	err = ehbi_inc_l(&bi1, v2);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_inc_l");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&bi1, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_inc_l(");
		log->append_s(log, dec_str1);
		log->append_s(log, ", ");
		log->append_l(log, v2);
		log->append_s(log, ", ");
		log->append_s(log, expect);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_inc_l(int v)
{
	unsigned failures = 0;

	failures += test_inc_lv(v, "700000000000", 134124, "700000134124");
	failures += test_inc_lv(v, "700000000000", -2, "699999999998");
	failures += test_inc_lv(v, "1", -3, "-2");

	return failures;
}

ECHECK_TEST_MAIN_V(test_inc_l)
