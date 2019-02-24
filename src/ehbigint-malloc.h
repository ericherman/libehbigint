/*
ehbigint-malloc.h: malloc wrapper for ehbigints
Copyright (C) 2017 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/

#ifndef EHBIGINT_MALLOC_H
#define EHBIGINT_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ehbigint.h"

/* function pointer to use if malloc fails */
extern void (*ehbi_alloc_fail)(int err);

struct ehbigint *ehbi_alloc(size_t bytes);
struct ehbigint *ehbi_alloc_l(size_t bytes, long val);
void ehbi_free(struct ehbigint *bi);

#ifdef __cplusplus
}
#endif

#endif /* EHBIGINT_MALLOC_H */
