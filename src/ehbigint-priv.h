/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint-priv.c: private functions for ehbigint.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

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
