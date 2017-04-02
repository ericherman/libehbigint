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

#if EHBI_DEBUG
extern unsigned EHBI_DBUG_i;
extern unsigned EHBI_DBUG_depth;
extern char EHBI_DBUG_Buf0[80];
extern char EHBI_DBUG_Buf1[80];

#ifdef _GNU_SOURCE
#define EHBI_FUNC __PRETTY_FUNCTION__
#else
#if (__STDC_VERSION__ >= 199901L)
#define EHBI_FUNC __func__
#else
#define EHBI_FUNC NULL
#endif /* _GNU_SOURCE */
#endif /* __STDC_VERSION__ */

#define Ehbi_trace_in() \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC)

#define Ehbi_trace_fprintf_bi(bi, name) \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "\n%s\t\t%s{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		name, \
		((bi)) ? (unsigned long)((bi)->bytes_len) : 0, \
		((bi)) ? (unsigned long)((bi)->bytes_used) : 0, \
		((bi)) ? (unsigned)((bi)->sign) : 0); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (((bi))? (bi)->bytes_len : 0); ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  ((bi)->bytes) ? (bi)->bytes[EHBI_DBUG_i] : 0); \
	} \
	fprintf(stderr, "}")

#define Ehbi_trace_fprintf_s(name, val) \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "\n%s\t\t%s: \"%s\"", EHBI_DBUG_Buf1, name, val)

#define Trace_bi(bi) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi, "bi"); \
	fprintf(stderr, ")\n")

#define Trace_bi_l(bi, l) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi, "bi"); \
	fprintf(stderr, ",\n%s\t\t%ld)\n", EHBI_DBUG_Buf1, l)

#define Trace_bi_s(bi, s) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi, "bi"); \
	fprintf(stderr, ",\n%s\t\t%s)\n", EHBI_DBUG_Buf1, s)

#define Trace_bi_bi(bi1, bi2) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi1, "bi1"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi2, "bi2"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi(bi1, bi2, bi3) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi1, "bi1"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi2, "bi2"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi3, "bi3"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4) \
	Ehbi_trace_in(); \
	Ehbi_trace_fprintf_bi(bi1, "bi1"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi2, "bi2"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi3, "bi3"); \
	fprintf(stderr, ","); \
	Ehbi_trace_fprintf_bi(bi4, "bi4"); \
	fprintf(stderr, ")\n")

#define Trace_msg_s_bi(msg, bi) \
	Ehbi_trace_fprintf_bi(bi, msg); \
	fprintf(stderr, "\n")

#define Trace_msg_s_s(msg, val) \
	Ehbi_trace_fprintf_s(msg, val); \
	fprintf(stderr, "\n")

#define Return_stack() \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	--EHBI_DBUG_depth

#define Return_i(val) \
	Return_stack(); \
	fprintf(stderr, "%s%s return (%d)\n", EHBI_DBUG_Buf0, EHBI_FUNC, val); \
	return val

#define Return_s(val) \
	Return_stack(); \
	fprintf(stderr, "%s%s return (\"%s\")\n", EHBI_DBUG_Buf0, EHBI_FUNC, val); \
	return val

#define Return_void() \
	Return_stack(); \
	fprintf(stderr, "%s%s return\n", EHBI_DBUG_Buf0, EHBI_FUNC); \
	return

#else /* no EHBI_DEBUG */

#define Trace_bi(bi)
#define Trace_bi_l(bi, l)
#define Trace_bi_s(bi, s)
#define Trace_bi_bi(bi1, bi2)
#define Trace_bi_bi_bi(bi1, bi2, bi3)
#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4)
#define Trace_msg_s_bi(s, bi)
#define Trace_msg_s_s(msg, val)
#define Return_i(val) return val
#define Return_s(val) return val
#define Return_void() return

#endif /* EHBI_DEBUG */

#define Ehbi_struct_is_not_null(bi) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null struct"); \
			Return_i(EHBI_NULL_STRUCT); \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			Return_i(EHBI_NULL_BYTES); \
		} \
	} while(0)

#define Ehbi_struct_is_not_null_e(bi, err) \
	do { \
		if (bi == NULL) { \
			Ehbi_log_error0("Null argument(s)"); \
			if (err) { \
				*err = EHBI_NULL_ARGS; \
			} \
			Return_i(0); \
		} \
		if (bi->bytes == NULL) { \
			Ehbi_log_error0("Null bytes[]"); \
			if (err) { \
				*err = EHBI_NULL_BYTES; \
			} \
			Return_i(0); \
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
