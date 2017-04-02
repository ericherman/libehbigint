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

int test_comp_mul_with_gmp(int verbose, int max_iterations, char *cmp_init_val)
{
	int failures, i, err;
	char ebuf[BUFLEN];
	char gbuf[BUFLEN];
	char *in_str;
	struct ehbigint ein, emul, eres, ediv, equot, erem;
	unsigned char in_bytes[BILEN], mul_bytes[BILEN], res_bytes[BILEN];
	unsigned char div_bytes[BILEN], quot_bytes[BILEN], rem_bytes[BILEN];

	mpz_t gin, gmul, gres, gdiv, gquot, grem;

	VERBOSE_ANNOUNCE(verbose);

	failures = 0;

	in_str = cmp_init_val == NULL ? "20151125" : cmp_init_val;

	ehbi_init(&ein, in_bytes, BILEN);
	ehbi_init(&emul, mul_bytes, BILEN);
	ehbi_init(&eres, res_bytes, BILEN);
	ehbi_init(&ediv, div_bytes, BILEN);
	ehbi_init(&equot, quot_bytes, BILEN);
	ehbi_init(&erem, rem_bytes, BILEN);

	ehbi_set_l(&ein, 0);
	ehbi_set_l(&emul, 252533);
	ehbi_set_l(&eres, 0);
	ehbi_set_l(&ediv, 33554393);
	ehbi_set_l(&equot, 0);
	ehbi_set_l(&erem, 0);

	mpz_init(gin);
	mpz_init(gmul);
	mpz_set_ui(gmul, 252533);
	mpz_init(gres);
	mpz_init(gdiv);
	mpz_set_ui(gdiv, 33554393);
	mpz_init(gquot);
	mpz_init(grem);

	for (i = 1; ((i < max_iterations) && (failures == 0)); ++i) {
		/* ours */
		if (verbose) {
			fprintf(stderr, "%d:%s\n", i, in_str);
		}

		mpz_set_str(gin, in_str, 10);
		err = ehbi_set_decimal_string(&ein, in_str, strlen(in_str));
		if (err) {
			Test_log_error1("ehbi_set_decimal_string error: %d\n",
					err);
		}

		mpz_get_str(gbuf, 10, gin);
		ehbi_to_decimal_string(&ein, ebuf, BUFLEN, &err);
		if (err) {
			Test_log_error1("ehbi_to_decimal_string error: %d\n",
					err);
		}
		failures += check_str_m(ebuf, gbuf, "from_decimal_string");

		mpz_mul(gres, gin, gmul);
		err = ehbi_mul(&eres, &ein, &emul);
		if (err) {
			Test_log_error1("ehbi_mul error: %d\n", err);
		}

		mpz_get_str(gbuf, 10, gres);
		ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
		if (err) {
			Test_log_error1("ehbi_to_decimal_string error: %d\n",
					err);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_mul");

		mpz_tdiv_qr(gquot, grem, gres, gdiv);
		err = ehbi_div(&equot, &erem, &eres, &ediv);
		if (err) {
			Test_log_error1("ehbi_div error: %d\n", err);
		}

		mpz_get_str(gbuf, 10, gquot);
		ehbi_to_decimal_string(&equot, ebuf, BUFLEN, &err);
		if (err) {
			Test_log_error1("ehbi_to_decimal_string error: %d\n",
					err);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_div (quot)");
		mpz_get_str(gbuf, 10, grem);
		ehbi_to_decimal_string(&erem, ebuf, BUFLEN, &err);
		if (err) {
			Test_log_error1("ehbi_to_decimal_string error: %d\n",
					err);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_div (rem)");

		if (failures) {
			Test_log_error2("iteration %d: in_str: %s\n", i,
					in_str);

			mpz_get_str(gbuf, 10, gin);
			ehbi_to_decimal_string(&ein, ebuf, BUFLEN, &err);
			Test_log_error1("\tein: %s\n", ebuf);
			Test_log_error1("\tgin: %s\n", gbuf);

			mpz_get_str(gbuf, 10, gmul);
			ehbi_to_decimal_string(&emul, ebuf, BUFLEN, &err);
			Test_log_error1("\temul: %s\n", ebuf);
			Test_log_error1("\tgmul: %s\n", gbuf);

			mpz_get_str(gbuf, 10, gres);
			ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
			Test_log_error1("\teres: %s\n", ebuf);
			Test_log_error1("\tgres: %s\n", gbuf);

			mpz_get_str(gbuf, 10, gdiv);
			ehbi_to_decimal_string(&ediv, ebuf, BUFLEN, &err);
			Test_log_error1("\tediv: %s\n", ebuf);
			Test_log_error1("\tgdiv: %s\n", gbuf);

			mpz_get_str(gbuf, 10, gquot);
			ehbi_to_decimal_string(&equot, ebuf, BUFLEN, &err);
			Test_log_error1("\tequot: %s\n", ebuf);
			Test_log_error1("\tgquot: %s\n", gbuf);

			mpz_get_str(gbuf, 10, grem);
			ehbi_to_decimal_string(&erem, ebuf, BUFLEN, &err);
			Test_log_error1("\terem: %s\n", ebuf);
			Test_log_error1("\tgrem: %s\n", gbuf);

			goto mpz_clean_up;
		}
		in_str = ebuf;
	}

mpz_clean_up:
	mpz_clear(gin);
	mpz_clear(gmul);
	mpz_clear(gres);
	mpz_clear(gdiv);
	mpz_clear(gquot);
	mpz_clear(grem);

	if (failures) {
		Test_log_error("failure in test_comp_mul_with_gmp\n");
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures, slow_iterations;
	char *cmp_init_val;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	slow_iterations = (argc > 2) ? atoi(argv[2]) : 10;
	cmp_init_val = (argc > 3) ? argv[3] : NULL;
	failures = 0;

	failures += test_comp_mul_with_gmp(v, slow_iterations, cmp_init_val);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
