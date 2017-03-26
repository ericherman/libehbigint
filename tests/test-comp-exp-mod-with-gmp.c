/*
test-compare-with-gmp.c
Copyright (C) 2016 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/
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

	ebase.bytes = base_bytes;
	eres.bytes = res_bytes;
	emod.bytes = mod_bytes;
	eexp.bytes = exp_bytes;

	ebase.bytes_len = BILEN;
	eres.bytes_len = BILEN;
	emod.bytes_len = BILEN;
	eexp.bytes_len = BILEN;

	ebase.sign = 0;
	eres.sign = 0;
	emod.sign = 0;
	eexp.sign = 0;

	ebase.bytes_used = 0;
	eres.bytes_used = 0;
	emod.bytes_used = 0;
	eexp.bytes_used = 0;

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

	if (extra) {
		failures += test_comp_exp_with_gmp(v, base, exp, mod);
	}

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
