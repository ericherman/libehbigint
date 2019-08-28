/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare-with-gmp.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <gmp.h>

int test_comp_exp_with_gmp(int verbose, unsigned long base, unsigned long exp,
			   unsigned long mod)
{
	int failures, err;
	char ebuf[BUFLEN];
	char gbuf[BUFLEN];
	struct ehbigint eres, ebase, eexp, emod;
	unsigned char base_bytes[BILEN], exp_bytes[BILEN], mod_bytes[BILEN];
	unsigned char res_bytes[BILEN];

	mpz_t gres, gbase, gexp, gmod;

	VERBOSE_ANNOUNCE(verbose);

	failures = 0;
	ehbi_init(&eres, res_bytes, BILEN);
	ehbi_init(&ebase, base_bytes, BILEN);
	ehbi_init(&emod, mod_bytes, BILEN);
	ehbi_init(&eexp, exp_bytes, BILEN);

	ehbi_set_l(&ebase, (long)base);
	ehbi_set_l(&eexp, (long)exp);
	ehbi_set_l(&emod, (long)mod);
	ehbi_set_l(&eres, 0);

	ehbi_exp_mod(&eres, &ebase, &eexp, &emod);
	ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
	if (err) {
		Test_log_error1("ehbi_to_decimal_string error: %d\n", err);
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
		Test_log_error3("failure in test_comp_exp_with_gmp"
				"(%lu,%lu,%lu)", base, exp, mod);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures, extra;
	unsigned long base, exp, mod;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	if (argc > 4) {
		extra = 1;
		base = atoi(argv[2]);
		exp = atoi(argv[3]);
		mod = atoi(argv[4]);
	} else {
		extra = 0;
	}

	failures = 0;

	failures += test_comp_exp_with_gmp(v, 1, 2, 3);
	failures += test_comp_exp_with_gmp(v, 10, 2, 7);
	failures += test_comp_exp_with_gmp(v, 5, 3, 13);
	failures += test_comp_exp_with_gmp(v, 4, 13, 497);
	failures += test_comp_exp_with_gmp(v, 121, 23, 13);
	failures += test_comp_exp_with_gmp(v, 1254, 523, 5513);
	failures += test_comp_exp_with_gmp(v, 3, 491225789, 982451579);

	if (extra) {
		failures += test_comp_exp_with_gmp(v, base, exp, mod);
	}

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
