/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-set-l.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_set_l_v(int verbose, long v1)
{
	struct eembed_log *log = eembed_err_log;
	unsigned failures;
	int err;

	char expect[25], buf[80];
	unsigned char bytes[4];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&bi, bytes, 4, &err);

	ehbi_set_l(&bi, 0, &err);
	failures += check_int_m(err, 0, "ehbi_set_l(0)");
	failures += Check_ehbigint_hex(&bi, "0x00");
	failures += Check_ehbigint_dec(&bi, "0");

	eembed_long_to_str(expect, 25, v1);

	buf[0] = '\0';
	eembed_strcat(buf, "ehbi_set_l(");
	eembed_strcat(buf, expect);
	eembed_strcat(buf, ")");
	ehbi_set_l(&bi, v1, &err);
	failures += check_int_m(err, 0, buf);

	failures += Check_ehbigint_dec(&bi, expect);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_set_l_v(");
		log->append_l(log, v1);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_set_l(int v)
{
	unsigned failures = 0;

	failures += test_set_l_v(v, 1234567890);
	failures += test_set_l_v(v, -1);

	return failures;
}

ECHECK_TEST_MAIN_V(test_set_l)
