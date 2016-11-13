/*
test-ehbigint-private-utils.h
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

#define LOG_ERROR(format) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format)

#define LOG_ERROR1(format, arg) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg)

#define LOG_ERROR2(format, arg1, arg2) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2)

#define LOG_ERROR3(format, arg1, arg2, arg3) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3)

#define LOG_ERROR4(format, arg1, arg2, arg3, arg4) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3, arg4)

#define VERBOSE_ANNOUNCE(verbose) \
	if (verbose) { fprintf(stderr, "starting %s\n", TEST_FUNC); }

#define cap_failures(failures) \
	((failures > 127) ? 127 : ((failures < -128) ? -128 : failures))

#define BUFLEN 80
#define BILEN 10

int check_ehbigint_dec(struct ehbigint *val, const char *expected, int line,
		       const char *msg);

int check_ehbigint_hex(struct ehbigint *val, const char *expected, int line,
		       const char *msg);

unsigned long ehbigint_to_unsigned_long(struct ehbigint *val, int *err);

#endif /* TEST_EHBIGINT_PRIVATE_UTILS_H */
