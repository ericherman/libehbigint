/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-ehbigint-private-utils.h */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#ifndef TEST_EHBIGINT_PRIVATE_UTILS_H
#define TEST_EHBIGINT_PRIVATE_UTILS_H

#include "../src/ehbigint.h"
#include "echeck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_FUNC \
	((ECHECK_FUNC == NULL) ? "" : ECHECK_FUNC)

#define STDERR_FILE_LINE_FUNC \
	fprintf(stderr, "%s:%d%s%s%s: ", __FILE__, __LINE__, \
	(ECHECK_FUNC == NULL) ? "" : ":", \
	TEST_FUNC, \
	(ECHECK_FUNC == NULL) ? "" : "()")

#define Test_log_error(format) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format)

#define Test_log_error1(format, arg) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg)

#define Test_log_error2(format, arg1, arg2) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2)

#define Test_log_error3(format, arg1, arg2, arg3) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3)

#define Test_log_error4(format, arg1, arg2, arg3, arg4) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3, arg4)

#define VERBOSE_ANNOUNCE(verbose) \
	if (verbose) { fprintf(stderr, "starting %s\n", TEST_FUNC); }

#define BUFLEN 80
#define BILEN 10

int check_ehbigint(struct ehbigint *val, struct ehbigint *expected, int line,
		   const char *msg);

#define Check_ehbigint(val, expected) \
	check_ehbigint(val, expected, __LINE__, TEST_FUNC)

int check_ehbigint_dec(struct ehbigint *val, const char *expected, int line,
		       const char *msg);

#define Check_ehbigint_dec(val, expected) \
	check_ehbigint_dec(val, expected, __LINE__, TEST_FUNC)

int check_ehbigint_hex(struct ehbigint *val, const char *expected, int line,
		       const char *msg);

#define Check_ehbigint_hex(val, expected) \
	check_ehbigint_hex(val, expected, __LINE__, TEST_FUNC)

unsigned long ehbigint_to_unsigned_long(struct ehbigint *val, int *err);

int log_contains(FILE *log, const char *expected);

#endif /* TEST_EHBIGINT_PRIVATE_UTILS_H */
