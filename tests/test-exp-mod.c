/*
test-exp-mod.c
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
#include "../src/ehbigint-log.h"	/* set_ehbi_log_file */

int test_exp_mod(int verbose, char *sbase, char *sexponent, char *smodulus,
		 char *sresult)
{
	int err, failures;

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

	base.bytes = bytes_base;
	base.bytes_len = 10;
	base.sign = 0;
	base.bytes_used = 0;

	exponent.bytes = bytes_exponent;
	exponent.bytes_len = 10;
	exponent.sign = 0;
	exponent.bytes_used = 0;

	modulus.bytes = bytes_modulus;
	modulus.bytes_len = 10;
	modulus.sign = 0;
	modulus.bytes_used = 0;

	result.bytes = bytes_result;
	result.bytes_len = 10;
	result.sign = 0;
	result.bytes_used = 0;

	err = ehbi_set_decimal_string(&base, sbase, strlen(sbase));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&base, sbase);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sbase);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_set_decimal_string(&exponent, sexponent, strlen(sexponent));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&exponent, sexponent);
	if (failures) {
		Test_log_error1("round trip failed %s\n", sexponent);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_set_decimal_string(&modulus, smodulus, strlen(smodulus));
	if (err) {
		Test_log_error1("error %d from ehbi_set_hex_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	failures += Check_ehbigint_dec(&modulus, smodulus);
	if (failures) {
		Test_log_error1("round trip failed %s\n", smodulus);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_exp_mod(&result, &base, &exponent, &modulus);
	if (err) {
		Test_log_error1("error %d from ehbi_exp_mod\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	failures += Check_ehbigint_dec(&result, sresult);

	return failures;
}

static int log_contains(FILE *log, const char *expected)
{
	char buffer[4096];
	rewind(log);
	while (fgets(buffer, 4096, log)) {
		if (strstr(buffer, expected)) {
			return 0;
		}
	}
	fprintf(stderr, "'%s' not found in log:\n", expected);
	rewind(log);
	while (fgets(buffer, 4096, log)) {
		fprintf(stderr, "%s", buffer);
	}
	fprintf(stderr, "\n(end of log)\n");

	return 1;
}

int test_exp_mod_by_zero(int verbose)
{
	int err, failures;
	FILE *log;

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
	log = tmpfile();
	set_ehbi_log_file(log);

	base.bytes = bytes_base;
	base.bytes_len = 10;
	base.sign = 0;
	base.bytes_used = 0;

	exponent.bytes = bytes_exponent;
	exponent.bytes_len = 10;
	exponent.sign = 0;
	exponent.bytes_used = 0;

	modulus.bytes = bytes_modulus;
	modulus.bytes_len = 10;
	modulus.sign = 0;
	modulus.bytes_used = 0;

	result.bytes = bytes_result;
	result.bytes_len = 10;
	result.sign = 0;
	result.bytes_used = 0;

	ehbi_set_l(&base, 10);
	ehbi_set_l(&exponent, 2);
	ehbi_set_l(&modulus, 0);

	err = ehbi_exp_mod(&result, &base, &exponent, &modulus);
	if (!err) {
		++failures;
		Test_log_error("no error from ehbi_exp_mod by zero?\n");
	}
	failures += log_contains(log, "modulus == 0");
	fclose(log);

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_exp_mod(v, "10", "2", "7", "2");
	failures += test_exp_mod(v, "5", "3", "13", "8");
	failures += test_exp_mod(v, "4", "13", "497", "445");

	failures += test_exp_mod_by_zero(v);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
