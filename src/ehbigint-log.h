/*
ehbigint-log.h: definitions for error reporting
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
#ifndef EHBIGINT_LOG_H
#define EHBIGINT_LOG_H

#include "ehbigint.h"		/* struct ehbigint */

#ifdef __cplusplus
#define Ehbigint_log_begin_C_functions extern "C" {
#define Ehbigint_log_end_C_functions }
#else
#define Ehbigint_log_begin_C_functions
#define Ehbigint_log_end_C_functions
#endif

Ehbigint_log_begin_C_functions
#undef Ehbigint_log_begin_C_functions
/*****************************************************************************/
#ifndef Ehbi_noop
#define Ehbi_noop ((void)(0))
#endif
/*****************************************************************************/
#ifndef SKIP_STDIO_H
#include <stdio.h>		/* FILE */
/* Get the FILE pointer to where fprintf messages currently target.
   Defaults to stderr. */
FILE *ehbi_log_file(void);

/* Set the FILE pointer to where fprintf messages shall target. */
void set_ehbi_log_file(FILE *log);
#endif /* SKIP_SDIO_H */

#ifndef SKIP_STDIO_H
/* if _POSIX_C_SOURCE backtrace_symbols_fd is used */
void ehbi_log_backtrace(FILE *log);
#endif

#ifndef Ehbi_log_error0
#define Ehbi_log_error0(format) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef Ehbi_log_error1
#define Ehbi_log_error1(format, arg) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef Ehbi_log_error2
#define Ehbi_log_error2(format, arg1, arg2) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg1, arg2); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef Ehbi_log_error3
#define Ehbi_log_error3(format, arg1, arg2, arg3) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg1, arg2, arg3); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#if EHBI_SKIP_STRUCT_NULL_CHECK
#define Ehbi_struct_is_not_null(bi) Ehbi_noop
#define Ehbi_struct_is_not_null_e(bi, err, err_rv) Ehbi_noop
#define Ehbi_struct_is_not_null_e_j(bi, err, jump_target) Ehbi_noop
#else
#define Ehbi_struct_is_not_null(bi) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null struct"); \
			return EHBI_NULL_STRUCT; \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			return EHBI_NULL_BYTES; \
		} \
	} while(0)

#define Ehbi_struct_is_not_null_e(bi, err, err_rv) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null argument(s)"); \
			if (err) { \
				*err = EHBI_NULL_ARGS; \
			} \
			return err_rv; \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			if (err) { \
				*err = EHBI_NULL_BYTES; \
			} \
			return err_rv; \
		} \
	} while(0)

#define Ehbi_struct_is_not_null_e_j(bi, err, jump_target) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null argument(s)"); \
			if (err) { \
				*err = EHBI_NULL_ARGS; \
			} \
			goto jump_target; \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			if (err) { \
				*err = EHBI_NULL_BYTES; \
			} \
			goto jump_target; \
		} \
	} while(0)
#endif /* EHBI_SKIP_STRUCT_NULL_CHECK */

/*****************************************************************************/
Ehbigint_log_end_C_functions
#undef Ehbigint_log_end_C_functions
#endif /* EHBIGINT_LOG_H */
