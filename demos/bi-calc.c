/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* bi-calc.c: a demo utility to show using the ehbigint library */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "../src/ehbigint.h"
#include <stdio.h>		/* printf fprintf */
#include <string.h>

int main(int argc, char *argv[])
{
	char buf[1024];
	unsigned char bbuf1[1024];
	unsigned char bbuf2[1024];
	unsigned char bbuf3[1024];
	unsigned char bbuf4[1024];
	struct ehbigint bigint_1;
	struct ehbigint bigint_2;
	struct ehbigint bigint_3;
	struct ehbigint bigint_4;
	int err;

	if (argc < 4) {
		fprintf(stderr, "usage:\n\t%s decimal1 [+-*/%%] decimal2\n",
			argv[0]);
		return -1;
	}

	bigint_1.bytes = bbuf1;
	bigint_1.bytes_len = 1024;

	bigint_2.bytes = bbuf2;
	bigint_2.bytes_len = 1024;

	bigint_3.bytes = bbuf3;
	bigint_3.bytes_len = 1024;

	bigint_4.bytes = bbuf4;
	bigint_4.bytes_len = 1024;

	ehbi_set_decimal_string(&bigint_1, argv[1], strlen(argv[1]));
	ehbi_set_decimal_string(&bigint_2, argv[3], strlen(argv[3]));

	switch (*argv[2]) {
	case '+':
		ehbi_add(&bigint_3, &bigint_1, &bigint_2);
		break;
	case '-':
		ehbi_subtract(&bigint_3, &bigint_1, &bigint_2);
		break;
	case '*':
		ehbi_mul(&bigint_3, &bigint_1, &bigint_2);
		break;
	case '/':
		ehbi_div(&bigint_3, &bigint_4, &bigint_1, &bigint_2);
		break;
	case '%':
		ehbi_div(&bigint_4, &bigint_3, &bigint_1, &bigint_2);
		break;
	default:
		fprintf(stderr, "operator '%c' not supported.\n", *argv[2]);
		return -1;
	}

	ehbi_to_decimal_string(&bigint_1, buf, 1024, &err);
	printf("   %40s\n", buf);
	ehbi_to_decimal_string(&bigint_2, buf, 1024, &err);
	printf(" %c %40s\n", *argv[2], buf);
	ehbi_to_decimal_string(&bigint_3, buf, 1024, &err);
	printf(" = %40s\n", buf);

	return 0;
}
