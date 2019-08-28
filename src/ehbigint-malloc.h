/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint-malloc.h: malloc wrapper for ehbigints */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#ifndef EHBIGINT_MALLOC_H
#define EHBIGINT_MALLOC_H

#ifdef __cplusplus
#define Ehbigint_malloc_begin_C_functions extern "C" {
#define Ehbigint_malloc_end_C_functions }
#else
#define Ehbigint_malloc_begin_C_functions
#define Ehbigint_malloc_end_C_functions
#endif

#include "ehbigint.h"

Ehbigint_malloc_begin_C_functions
#undef Ehbigint_malloc_begin_C_functions
/****************************************************************************/
/* function pointer to use if malloc fails */
extern void (*ehbi_alloc_fail)(int err);

struct ehbigint *ehbi_alloc(size_t bytes);
struct ehbigint *ehbi_alloc_l(size_t bytes, long val);
void ehbi_free(struct ehbigint *bi);

/****************************************************************************/
Ehbigint_malloc_end_C_functions
#undef Ehbigint_malloc_end_C_functions
#endif /* EHBIGINT_MALLOC_H */
