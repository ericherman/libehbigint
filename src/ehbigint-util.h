/*
ehbigint-util.c: support-functions for ehbigint.c
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
#ifndef EHBIGINT_UTIL_H
#define EHBIGINT_UTIL_H

#ifdef __cplusplus
#define Ehbigint_util_begin_C_functions extern "C" {
#define Ehbigint_util_end_C_functions }
#else
#define Ehbigint_util_begin_C_functions
#define Ehbigint_util_end_C_functions
#endif

Ehbigint_util_begin_C_functions
#undef Ehbigint_util_begin_C_functions
/*****************************************************************************/
#ifdef EHBI_NO_ALLOCA
#include <stdlib.h>
void ehbi_do_stack_free(void *ptr);
#define ehbi_stack_alloc(len) malloc(len)
#define ehbi_stack_alloc_str "malloc"
#define ehbi_stack_free(p) ehbi_do_stack_free(p)
#else
#include <alloca.h>
#define ehbi_stack_alloc(len) alloca(len)
#define ehbi_stack_alloc_str "alloca"
#define ehbi_stack_free(p) ((void)0)
#endif
#ifndef NDEBUG
#define Ehbi_null_the_ptr(p) do { p = NULL; } while (0)
#else
#define Ehbi_null_the_ptr(p) ((void)0)
#endif
#define Ehbi_stack_free(p) \
	do { \
		ehbi_stack_free(p); \
		Ehbi_null_the_ptr(p); \
	} while (0)
#ifndef EHBI_SKIP_IS_PROBABLY_PRIME
#ifndef ehbi_random_bytes
#define EHBI_RANDOM_FROM_LINUX_DEV_URANDOM
int ehbi_dev_urandom_bytes(unsigned char *buf, size_t len);
#define ehbi_random_bytes ehbi_dev_urandom_bytes
#endif /* ehbi_random_bytes */
#endif /* EHBI_SKIP_IS_PROBABLY_PRIME */

/*****************************************************************************/
Ehbigint_util_end_C_functions
#undef Ehbigint_util_end_C_functions
#endif /* EHBIGINT_UTIL_H */
