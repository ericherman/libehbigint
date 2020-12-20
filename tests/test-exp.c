/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-exp.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_exp_v(int verbose, const char *sbase, const char *sexponent,
		    const char *sresult)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char bytes_base[20];
	unsigned char bytes_exponent[20];
	unsigned char bytes_result[40];

	struct ehbigint base;
	struct ehbigint exponent;
	struct ehbigint result;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&base, bytes_base, 20);
	ehbi_init(&exponent, bytes_exponent, 20);
	ehbi_init(&result, bytes_result, 30);

	err = ehbi_set_decimal_string(&base, sbase, eembed_strlen(sbase));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, sbase);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&base, sbase);
	if (failures) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, sbase);
		log->append_s(log, " Aborting test.");
		log->append_eol(log);
		return failures;
	}

	err =
	    ehbi_set_decimal_string(&exponent, sexponent,
				    eembed_strlen(sexponent));
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, sexponent);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&exponent, sexponent);
	if (failures) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, sexponent);
		log->append_s(log, " Aborting test.");
		log->append_eol(log);
		return failures;
	}

	err = ehbi_exp(&result, &base, &exponent);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_exp");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&result, sresult);

	return failures;
}

unsigned test_exp(int v)
{
	unsigned failures = 0;

	failures += test_exp_v(v, "10", "2", "100");
	failures += test_exp_v(v, "5", "4", "625");
	failures +=
	    test_exp_v(v, "1000", "10", "1000000000000000000000000000000");
	failures += test_exp_v(v, "1000000000000000000000000000000", "0", "1");

	return failures;
}

ECHECK_TEST_MAIN_V(test_exp)
