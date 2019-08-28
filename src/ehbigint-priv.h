/*
ehbigint-priv.c: private functions for ehbigint.c
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
#ifndef EHBIGINT_PRIV_H
#define EHBIGINT_PRIV_H

#ifdef __cplusplus
#define Ehbigint_priv_begin_C_functions extern "C" {
#define Ehbigint_priv_end_C_functions }
#else
#define Ehbigint_priv_begin_C_functions
#define Ehbigint_priv_end_C_functions
#endif

#include "ehbigint.h"		/* struct ehbigint */

Ehbigint_priv_begin_C_functions
#undef Ehbigint_priv_begin_C_functions
/*****************************************************************************/
void ehbi_internal_reset_bytes_used(struct ehbigint *bi, size_t from);
/*****************************************************************************/
Ehbigint_priv_end_C_functions
#undef Ehbigint_priv_end_C_functions
#endif /* EHBIGINT_PRIV_H */
