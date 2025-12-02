/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-ehbigint-private-utils.h */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#ifndef TEST_EHBIGINT_PRIVATE_UTILS_H
#define TEST_EHBIGINT_PRIVATE_UTILS_H

#include "ehbigint.h"
#include "echeck.h"

#define TEST_FUNC \
	((ECHECK_FUNC == NULL) ? "" : ECHECK_FUNC)

#define STDERR_FILE_LINE_FUNC(log) \
	do { \
		log->append_s(log, __FILE__); \
		log->append_c(log, ':'); \
		log->append_ul(log, __LINE__); \
		log->append_s(log, (ECHECK_FUNC == NULL) ? "" : ":"); \
		log->append_s(log, TEST_FUNC); \
		log->append_s(log, (ECHECK_FUNC == NULL) ? "" : "()"); \
		log->append_s(log, ": "); \
	} while (0)

#define Test_log_error(s) \
	do {\
		STDERR_FILE_LINE_FUNC(eembed_err_log); \
		eembed_err_log->append_s(eembed_err_log, s); \
		eembed_err_log->append_eol(eembed_err_log); \
	} while (0)

#define VERBOSE_ANNOUNCE(verbose) \
	if (verbose) { \
		print_s("starting "); \
		print_s(TEST_FUNC); \
		print_eol(); \
	}

#define BUFLEN 80
#define BILEN 10

unsigned check_ehbigint(struct ehbigint *val, struct ehbigint *expected,
			int line, const char *msg);

#define Check_ehbigint(val, expected) \
	check_ehbigint(val, expected, __LINE__, TEST_FUNC)

unsigned check_ehbigint_dec(struct ehbigint *val, const char *expected,
			    int line, const char *msg);

#define Check_ehbigint_dec(val, expected) \
	check_ehbigint_dec(val, expected, __LINE__, TEST_FUNC)

unsigned check_ehbigint_hex(struct ehbigint *val, const char *expected,
			    int line, const char *msg);

#define Check_ehbigint_hex(val, expected) \
	check_ehbigint_hex(val, expected, __LINE__, TEST_FUNC)

unsigned long ehbigint_to_unsigned_long(struct ehbigint *val, int *err);

#endif /* TEST_EHBIGINT_PRIVATE_UTILS_H */
