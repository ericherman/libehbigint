/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-exp-mod.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_exp_mod_v(int verbose, const char *sbase, const char *sexponent,
			const char *smodulus, const char *sresult)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;

	unsigned char bytes_base[10];
	unsigned char bytes_exponent[10];
	unsigned char bytes_modulus[10];
	unsigned char bytes_result[10];

	struct ehbigint base;
	struct ehbigint exponent;
	struct ehbigint modulus;
	struct ehbigint result;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = 0;
	ehbi_init(&base, bytes_base, 10);
	ehbi_init(&exponent, bytes_exponent, 10);
	ehbi_init(&modulus, bytes_modulus, 10);
	ehbi_init(&result, bytes_result, 10);

	ehbi_set_decimal_string(&base, sbase, eembed_strlen(sbase), &err);
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

	ehbi_set_decimal_string(&exponent, sexponent, eembed_strlen(sexponent),
				&err);
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

	ehbi_set_decimal_string(&modulus, smodulus, eembed_strlen(smodulus),
				&err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_decimal_string(");
		log->append_s(log, smodulus);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}
	failures += Check_ehbigint_dec(&modulus, smodulus);
	if (failures) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "round trip failed? ");
		log->append_s(log, smodulus);
		log->append_s(log, " Aborting test.");
		log->append_eol(log);
		return failures;
	}

	ehbi_exp_mod(&result, &base, &exponent, &modulus, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_exp_mod");
		log->append_eol(log);
	}

	failures += Check_ehbigint_dec(&result, sresult);

	return failures;
}

unsigned test_exp_mod_by_zero(int verbose)
{
	int err;
	unsigned failures;

	const size_t buflen = 250;
	char buf[250];
	struct eembed_str_buf sbuf;
	struct eembed_log slog;
	struct eembed_log *log;
	struct eembed_log *orig;

	unsigned char bytes_base[10];
	unsigned char bytes_exponent[10];
	unsigned char bytes_modulus[10];
	unsigned char bytes_result[10];

	struct ehbigint base;
	struct ehbigint exponent;
	struct ehbigint modulus;
	struct ehbigint result;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	orig = ehbi_log_get();
	eembed_memset(buf, 0x00, buflen);
	log = eembed_char_buf_log_init(&slog, &sbuf, buf, buflen);
	if (log) {
		ehbi_log_set(log);
	}

	err = 0;
	ehbi_init(&base, bytes_base, 10);
	ehbi_init(&exponent, bytes_exponent, 10);
	ehbi_init(&modulus, bytes_modulus, 10);
	ehbi_init(&result, bytes_result, 10);

	ehbi_set_l(&base, 10, &err);
	ehbi_set_l(&exponent, 2, &err);
	ehbi_set_l(&modulus, 0, &err);

	if (err) {
		STDERR_FILE_LINE_FUNC(orig);
		orig->append_s(orig, "init error?");
		orig->append_eol(orig);
		return 1;
	}

	err = 0;
	ehbi_exp_mod(&result, &base, &exponent, &modulus, &err);
	if (!err) {
		++failures;
		STDERR_FILE_LINE_FUNC(orig);
		orig->append_s(orig, "no error from ehbi_exp_mod by zero?");
		orig->append_eol(orig);
	}
	failures += check_str_contains(buf, "modulus == 0");

	ehbi_log_set(orig);
	return failures;
}

unsigned test_exp_mod(int v)
{
	unsigned failures = 0;

	failures += test_exp_mod_v(v, "10", "2", "7", "2");
	failures += test_exp_mod_v(v, "5", "3", "13", "8");
	failures += test_exp_mod_v(v, "4", "13", "497", "445");
	failures += test_exp_mod_v(v, "255", "255", "63", "27");
	failures += test_exp_mod_v(v, "16", "16", "10000000000", "3709551616");
	failures += test_exp_mod_v(v, "999", "999", "10000000000", "499998999");

	failures += test_exp_mod_by_zero(v);

	return failures;
}

ECHECK_TEST_MAIN_V(test_exp_mod)
