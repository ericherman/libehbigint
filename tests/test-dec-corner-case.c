/*
test-dec-corner-case.c
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
#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"

int test_dec_corner_case(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[BILEN];
	unsigned char bytes_buf2[BILEN];
	struct ehbigint bi1, bi2;

	/*
	   $ bc <<< "50331548-33554393"
	   16777155
	 */
	char *str_1 = "50331548";
	char *str_2 = "33554393";
	char *str_3 = "16777155";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = BILEN;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = BILEN;

	err = ehbi_from_decimal_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_decimal_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d ehbi_from_decimal_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_subtract\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&bi1, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_subtract\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_dec_corner_case(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
