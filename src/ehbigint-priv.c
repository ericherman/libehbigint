/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint.c: slow Big Int library hopefully somewhat suitable for 8bit CPUs */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

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
