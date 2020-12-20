/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-scenario-mul-mod.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_scenario_mul_mod(int verbose)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	struct ehbigint bx, by, bz, bresult, bquot, brem;
	unsigned long x, y, z, result;
	unsigned char xb[16], yb[16], zb[16], resb[16], quotb[16], remb[16];
	const char *expect_mul;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&bx, xb, 16);
	ehbi_init(&by, yb, 16);
	ehbi_init(&bz, zb, 16);
	ehbi_init(&bresult, resb, 16);
	ehbi_init(&bquot, quotb, 16);
	ehbi_init(&brem, remb, 16);

	x = 20151125;
	err = ehbi_set_l(&bx, x);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, x);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	y = 252533;
	err = ehbi_set_l(&by, y);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, y);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	z = 33554393;
	err = ehbi_set_l(&bz, z);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_l(");
		log->append_l(log, y);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	/* 20151125 * 252533 == 5088824049625 */
	expect_mul = "5088824049625";
	err = ehbi_mul(&bresult, &bx, &by);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_mul (");
		log->append_ul(log, x);
		log->append_s(log, " * ");
		log->append_ul(log, y);
		log->append_s(log, ")");
		log->append_eol(log);
		Test_log_error("Aborting test\n");
		return 1;
	}
	failures += Check_ehbigint_dec(&bresult, expect_mul);

	/*
	   r = lldiv(5088824049625, 33554393);
	   r.quot: 151658, r.rem: 31916031
	 */
	err = ehbi_div(&bquot, &brem, &bresult, &bz);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_div (");
		log->append_s(log, expect_mul);
		log->append_s(log, " / ");
		log->append_ul(log, z);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	if (brem.bytes_used > (1 + sizeof(unsigned long))) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "rem.bytes_used > 1+sizeof(unsigned long)");
		log->append_s(log, " (");
		log->append_ul(log, brem.bytes_used);
		log->append_s(log, " > ");
		log->append_ul(log, (1 + sizeof(unsigned long)));
		log->append_s(log, ")");
		log->append_eol(log);
	}

	result = ehbigint_to_unsigned_long(&bquot, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbigint_to_unsigned_long");
		log->append_eol(log);
	}
	failures += check_unsigned_long(result, 151658);

	result = ehbigint_to_unsigned_long(&brem, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbigint_to_unsigned_long(rem)");
		log->append_eol(log);
	}
	failures += check_unsigned_long(result, 31916031);

	return failures;
}

ECHECK_TEST_MAIN_V(test_scenario_mul_mod)
