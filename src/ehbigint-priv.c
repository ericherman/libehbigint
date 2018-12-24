/*
ehbigint.c: slow Big Int library hopefully somewhat suitable for 8bit CPUs
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

#include "ehbigint-priv.h"
#include "ehbigint-log.h"
#include "ehbigint-util.h"
#include "ehbigint-eba.h"

void ehbi_internal_reset_bytes_used(struct ehbigint *bi, size_t from)
{
	size_t i;

	if (from > bi->bytes_len) {
		from = bi->bytes_len;
	}
	for (i = (bi->bytes_len - from); i < bi->bytes_len; ++i) {
		if (bi->bytes[i] != 0) {
			break;
		}
	}
	bi->bytes_used = (bi->bytes_len - i);
	if (bi->bytes_used == 0) {
		bi->bytes_used = 1;
	}

	if ((bi->bytes_used == 1) && (bi->bytes[bi->bytes_len - 1] == 0x00)) {
		bi->sign = 0;
	}
}

void ehbi_internal_zero(struct ehbigint *bi)
{
	Eba_memset(bi->bytes, 0x00, bi->bytes_len);
	bi->bytes_used = 1;
	bi->sign = 0;
}

void ehbi_internal_clear_null_struct(struct ehbigint *bi)
{
	bi->bytes = NULL;
	bi->bytes_len = 0;
	bi->bytes_used = 0;
	bi->sign = 0;
}
