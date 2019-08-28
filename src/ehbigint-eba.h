/*
ehbigint-eba.c: shim for eba code
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
#ifndef EHBIGINT_EBA_H
#define EHBIGINT_EBA_H

#include "ehbigint-log.h"
#include "ehbigint-util.h"

#ifdef __cplusplus
#define Ehbigint_eba_begin_C_functions extern "C" {
#define Ehbigint_eba_end_C_functions }
#else
#define Ehbigint_eba_begin_C_functions
#define Ehbigint_eba_end_C_functions
#endif

Ehbigint_eba_begin_C_functions
#undef Ehbigint_eba_begin_C_functions
/*****************************************************************************/
extern int ehbi_eba_err;
#define Eba_crash() do { \
	Ehbi_log_error0("EBA CRASH!\n"); \
	ehbi_eba_err = EHBI_EBA_CRASH; \
	return; \
	} while(0)

#define Eba_crash_uc() do { \
	Ehbi_log_error0("EBA CRASH UC!\n"); \
	ehbi_eba_err = EHBI_EBA_CRASH; \
	return 0; \
	} while(0)

#define Eba_log_error0 Ehbi_log_error0
#define Eba_log_error1 Ehbi_log_error1
#define Eba_log_error2 Ehbi_log_error2
#define Eba_log_error3 Ehbi_log_error3

#define Eba_stack_alloc ehbi_stack_alloc
#define Eba_stack_alloc_str "ehbi_stack_alloc"
#define Eba_stack_free ehbi_stack_free

#include "eba.h"
#include "eba-internal.h"

#define Ehstr_memset Eba_memset

/*****************************************************************************/
Ehbigint_eba_end_C_functions
#undef Ehbigint_eba_end_C_functions
#endif /* EHBIGINT_EBA_H */
