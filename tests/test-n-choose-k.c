/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-n-choose-k.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_n_choose_k_v(int verbose, const char *nstr, const char *kstr,
			   const char *expectstr)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	struct ehbigint n;
	unsigned char n_bytes[20];

	struct ehbigint k;
	unsigned char k_bytes[20];

	struct ehbigint res;
	unsigned char res_bytes[20];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&n, n_bytes, 20, &err);
	ehbi_init(&k, k_bytes, 20, &err);
	ehbi_init(&res, res_bytes, 20, &err);

	ehbi_set_decimal_string(&n, nstr, eembed_strlen(nstr), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, nstr);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_set_decimal_string(&k, kstr, eembed_strlen(kstr), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, kstr);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	ehbi_n_choose_k(&res, &n, &k, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_n_choose_k");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&res, expectstr);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_n_choose_k_v(n => ");
		log->append_s(log, nstr);
		log->append_s(log, ", k => ");
		log->append_s(log, kstr);
		log->append_s(log, ", expect => ");
		log->append_s(log, expectstr);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_n_choose_k(int v)
{
	unsigned failures = 0;

	failures += test_n_choose_k_v(v, "4", "2", "6");
	failures += test_n_choose_k_v(v, "5", "3", "10");
	failures += test_n_choose_k_v(v, "40", "20", "137846528820");

	return failures;
}

ECHECK_TEST_MAIN_V(test_n_choose_k)
