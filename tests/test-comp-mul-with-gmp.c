/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-compare-mul-with-gmp.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"
#include <stdio.h>
#include <gmp.h>

int test_comp_mul_with_gmp(int verbose, int max_iterations, char *cmp_init_val)
{
	struct eembed_log *log = eembed_err_log;
	int failures, i, err;
	char ebuf[BUFLEN];
	char gbuf[BUFLEN];
	const char *in_str;
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
			log->append_l(log, i);
			log->append_c(log, ':');
			log->append_s(log, in_str);
			log->append_eol(log);
		}

		mpz_set_str(gin, in_str, 10);
		err =
		    ehbi_set_decimal_string(&ein, in_str,
					    eembed_strlen(in_str));
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_set_decimal_string error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}

		mpz_get_str(gbuf, 10, gin);
		ehbi_to_decimal_string(&ein, ebuf, BUFLEN, &err);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_to_decimal_string error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}
		failures += check_str_m(ebuf, gbuf, "from_decimal_string");

		mpz_mul(gres, gin, gmul);
		err = ehbi_mul(&eres, &ein, &emul);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_mul error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}

		mpz_get_str(gbuf, 10, gres);
		ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_to_decimal_string error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_mul");

		mpz_tdiv_qr(gquot, grem, gres, gdiv);
		err = ehbi_div(&equot, &erem, &eres, &ediv);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_div error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}

		mpz_get_str(gbuf, 10, gquot);
		ehbi_to_decimal_string(&equot, ebuf, BUFLEN, &err);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_to_decimal_string error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_div (quot)");
		mpz_get_str(gbuf, 10, grem);
		ehbi_to_decimal_string(&erem, ebuf, BUFLEN, &err);
		if (err) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "ehbi_to_decimal_string error: ");
			log->append_l(log, err);
			log->append_eol(log);
		}
		failures += check_str_m(ebuf, gbuf, "ehbi_div (rem)");

		if (failures) {
			STDERR_FILE_LINE_FUNC(log);
			log->append_s(log, "iteration ");
			log->append_l(log, i);
			log->append_s(log, ": in_str: ");
			log->append_s(log, in_str);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, gin);
			ehbi_to_decimal_string(&ein, ebuf, BUFLEN, &err);
			log->append_s(log, "\tein: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgin: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, gmul);
			ehbi_to_decimal_string(&emul, ebuf, BUFLEN, &err);
			log->append_s(log, "\temul: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgmul: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, gres);
			ehbi_to_decimal_string(&eres, ebuf, BUFLEN, &err);
			log->append_s(log, "\teres: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgres: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, gdiv);
			ehbi_to_decimal_string(&ediv, ebuf, BUFLEN, &err);
			log->append_s(log, "\tediv: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgdiv: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, gquot);
			ehbi_to_decimal_string(&equot, ebuf, BUFLEN, &err);
			log->append_s(log, "\tequot: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgquot: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

			mpz_get_str(gbuf, 10, grem);
			ehbi_to_decimal_string(&erem, ebuf, BUFLEN, &err);
			log->append_s(log, "\terem: ");
			log->append_s(log, ebuf);
			log->append_eol(log);
			log->append_s(log, "\tgrem: ");
			log->append_s(log, gbuf);
			log->append_eol(log);

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
	struct eembed_log *log = eembed_err_log;
	int v;
	unsigned failures;
	int slow_iterations;
	char *cmp_init_val;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	slow_iterations = (argc > 2) ? atoi(argv[2]) : 10;
	cmp_init_val = (argc > 3) ? argv[3] : NULL;
	failures = 0;

	failures += test_comp_mul_with_gmp(v, slow_iterations, cmp_init_val);

	if (failures) {
		log->append_ul(log, failures);
		log->append_s(log, " failures in ");
		log->append_s(log, __FILE__);
		log->append_eol(log);
	}

	return failures;
}
