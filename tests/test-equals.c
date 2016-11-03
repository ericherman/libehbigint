/*
test-equals.c
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

int test_equals(int verbose)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x720000F";
	const char *str_2 = "0x720000F";
	const char *str_3 = "0x0F";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	err += ehbi_from_hex_string(&bi3, str_3, strlen(str_3));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_equals(&bi1, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi3, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	if (failures) {
		LOG_ERROR1("%d failures in test_equals\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_equals(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
