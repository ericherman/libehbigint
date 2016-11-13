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

#ifdef __cplusplus
extern "C" {
#endif

#include "ehbigint.h"		/* struct ehbigint */
#include <stdio.h>		/* FILE */

/* Get the FILE pointer to where fprintf messages currently target.
   Defaults to stderr. */
FILE *ehbi_log_file();

/* Set the FILE pointer to where fprintf messages shall target. */
void set_ehbi_log_file(FILE *log);

extern int ehbi_debug_log_level;
int ehbi_debugf(int level, const char *fmt, ...);

void ehbi_debug_to_hex(int level, const struct ehbigint *bi, const char *label);

void ehbi_debug_to_string(int level, const struct ehbigint *bi,
			  const char *label);

/* if _POSIX_C_SOURCE backtrace_symbols_fd is used */
void ehbi_log_backtrace(FILE *log);

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

#ifdef __cplusplus
}
#endif

#endif /* EHBIGINT_LOG_H */
