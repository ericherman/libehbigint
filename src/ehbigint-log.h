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

#define Trace_bi(bi) \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		((bi)) ? (unsigned long)((bi)->bytes_len) : 0, \
		((bi)) ? (unsigned long)((bi)->bytes_used) : 0, \
		((bi)) ? (unsigned)((bi)->sign) : 0); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (((bi))? (bi)->bytes_len : 0); ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  ((bi)->bytes) ? (bi)->bytes[EHBI_DBUG_i] : 0); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_l(bi, l) \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		((bi)) ? (unsigned long)((bi)->bytes_len) : 0, \
		((bi)) ? (unsigned long)((bi)->bytes_used) : 0, \
		((bi)) ? (unsigned)((bi)->sign) : 0); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (((bi))? (bi)->bytes_len : 0); ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  ((bi)->bytes) ? (bi)->bytes[EHBI_DBUG_i] : 0); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ",\n%s\t\t%ld)\n", EHBI_DBUG_Buf1, l)

#define Trace_bi_bi(bi1, bi2) \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi1)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi2)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi(bi1, bi2, bi3) \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi1)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "%s\n\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi2)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi3{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi3)->bytes_len), \
		(unsigned long)((bi3)->bytes_used), \
		(unsigned)((bi3)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi3)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi3)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4) \
	++EHBI_DBUG_depth; \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s(", EHBI_DBUG_Buf0, EHBI_FUNC); \
	fprintf(stderr, "\n%s\t\tbi1{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi1)->bytes_len), \
		(unsigned long)((bi1)->bytes_used), \
		(unsigned)((bi1)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi1)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi1)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi2{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi2)->bytes_len), \
		(unsigned long)((bi2)->bytes_used), \
		(unsigned)((bi2)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi2)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi2)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi3{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi3)->bytes_len), \
		(unsigned long)((bi3)->bytes_used), \
		(unsigned)((bi3)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi3)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi3)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ","); \
	fprintf(stderr, "\n%s\t\tbi4{bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		(unsigned long)((bi4)->bytes_len), \
		(unsigned long)((bi4)->bytes_used), \
		(unsigned)((bi4)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi4)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi4)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}"); \
	fprintf(stderr, ")\n")

#define Trace_msg_s_bi(s, bi) \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%s%s: {bytes_len:%lu,bytes_used:%lu,sign:%u,0x", \
		EHBI_DBUG_Buf1, \
		s, \
		(unsigned long)((bi)->bytes_len), \
		(unsigned long)((bi)->bytes_used), \
		(unsigned)((bi)->sign)); \
	for (EHBI_DBUG_i = 0; EHBI_DBUG_i < (bi)->bytes_len; ++EHBI_DBUG_i) { \
		fprintf(stderr, "%02X",  (bi)->bytes[EHBI_DBUG_i]); \
	} \
	fprintf(stderr, "}\n")

#define Return_i(val) \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%sreturn (%d) (%s)\n", EHBI_DBUG_Buf0, val, EHBI_FUNC); \
	--EHBI_DBUG_depth; \
	return val

#define Return_void() \
	for (EHBI_DBUG_i= 0; EHBI_DBUG_i < EHBI_DBUG_depth; ++EHBI_DBUG_i) { \
		EHBI_DBUG_Buf0[EHBI_DBUG_i] = '-'; \
		EHBI_DBUG_Buf1[EHBI_DBUG_i] = ' '; \
	} \
	EHBI_DBUG_Buf0[EHBI_DBUG_i] = '\0'; \
	EHBI_DBUG_Buf1[EHBI_DBUG_i] = '\0'; \
	fprintf(stderr, "%sreturn (%s)\n", EHBI_DBUG_Buf0, EHBI_FUNC); \
	--EHBI_DBUG_depth; \
	return

#else
#define Trace_bi(bi)
#define Trace_bi_l(bi, l)
#define Trace_bi_bi(bi1, bi2)
#define Trace_bi_bi_bi(bi1, bi2, bi3)
#define Trace_bi_bi_bi_bi(bi1, bi2, bi3, bi4)
#define Trace_msg_s_bi(s, bi)
#define Return_i(val) return val
#define Return_void() return
#endif

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
