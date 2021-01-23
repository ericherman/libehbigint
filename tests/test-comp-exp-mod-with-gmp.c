/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare-with-gmp.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <gmp.h>

int test_comp_exp_with_gmp_bem(int verbose, unsigned long base,
			       unsigned long exp, unsigned long mod)
{
	struct eembed_log *log = eembed_err_log;
	unsigned failures;
	int err;
	char ebuf[BUFLEN];
	char gbuf[BUFLEN];
	struct ehbigint eres, ebase, eexp, emod;
	unsigned char base_bytes[BILEN], exp_bytes[BILEN], mod_bytes[BILEN];
	unsigned char res_bytes[BILEN];

	mpz_t gres, gbase, gexp, gmod;

	VERBOSE_ANNOUNCE(verbose);

	failures = 0;
	err = 0;
	ehbi_init(&eres, res_bytes, BILEN, &err);
	ehbi_init(&ebase, base_bytes, BILEN, &err);
	ehbi_init(&emod, mod_bytes, BILEN, &err);
	ehbi_init(&eexp, exp_bytes, BILEN, &err);

	ehbi_set_l(&ebase, (long)base, &err);
	ehbi_set_l(&eexp, (long)exp, &err);
	ehbi_set_l(&emod, (long)mod, &err);
	ehbi_set_l(&eres, 0, &err);

	ehbi_exp_mod(&eres, &ebase, &eexp, &emod, &err);
	ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " ehbi_to_decimal_string");
		log->append_eol(log);
	}

	mpz_init(gbase);
	mpz_set_ui(gbase, base);

	mpz_init(gexp);
	mpz_set_ui(gexp, exp);

	mpz_init(gmod);
	mpz_set_ui(gmod, mod);

	mpz_init(gres);
	mpz_set_ui(gres, 0);

	mpz_powm(gres, gbase, gexp, gmod);
	gmp_snprintf(gbuf, BUFLEN, "%Zd", gres);

	failures += check_str_m(ebuf, gbuf, "ebuf,gbuf");

	mpz_clear(gbase);
	mpz_clear(gexp);
	mpz_clear(gmod);
	mpz_clear(gres);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in test_comp_exp_with_gmp_bem(");
		log->append_ul(log, base);
		log->append_s(log, ", ");
		log->append_ul(log, exp);
		log->append_s(log, ", ");
		log->append_ul(log, mod);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_comp_exp_mod_with_gmp(int v)
{
	unsigned failures = 0;

	failures += test_comp_exp_with_gmp_bem(v, 1, 2, 3);
	failures += test_comp_exp_with_gmp_bem(v, 10, 2, 7);
	failures += test_comp_exp_with_gmp_bem(v, 5, 3, 13);
	failures += test_comp_exp_with_gmp_bem(v, 4, 13, 497);
	failures += test_comp_exp_with_gmp_bem(v, 121, 23, 13);
	failures += test_comp_exp_with_gmp_bem(v, 1254, 523, 5513);
	failures += test_comp_exp_with_gmp_bem(v, 3, 491225789, 982451579);

	return failures;
}

ECHECK_TEST_MAIN_V(test_comp_exp_mod_with_gmp)
