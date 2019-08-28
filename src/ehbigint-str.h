/*
ehbigint-str.h: additional prototypes for ehbigint-str.c
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
