/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint-str.h: additional prototypes for ehbigint-str.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#ifndef EHBIGINT_STR_H
#define EHBIGINT_STR_H

#ifdef __cplusplus
#define Ehbigint_str_begin_C_functions extern "C" {
#define Ehbigint_str_end_C_functions }
#else
#define Ehbigint_str_begin_C_functions
#define Ehbigint_str_end_C_functions
#endif

#include "ehbigint.h"

Ehbigint_str_begin_C_functions
#undef Ehbigint_str_begin_C_functions
/* would a union make this nicer? */
int ehbi_byte_to_hex_chars(unsigned char byte, char *high, char *low);
int ehbi_hex_chars_to_byte(char high, char low, unsigned char *byte);

Ehbigint_str_end_C_functions
#undef Ehbigint_str_end_C_functions
#endif /* EHBIGINT_STR_H */
