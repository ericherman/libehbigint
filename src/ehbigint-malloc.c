/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint-malloc.c: malloc wrapper for ehbigints */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "ehbigint-malloc.h"
#include "ehbigint-log.h"
#include <stdlib.h>

void (*ehbi_alloc_fail)(int status) = NULL;

#define Ehbi_alloc_fail(err) do { \
	if (ehbi_alloc_fail) { \
		ehbi_alloc_fail(err); \
	} } while (0)

struct ehbigint *ehbi_alloc_l(size_t bytes_len, long val)
{
	struct ehbigint *bi;
	unsigned char *bytes;
	int err = EHBI_SUCCESS;

	bi = malloc(sizeof(struct ehbigint));
	if (!bi) {
		Ehbi_log_error1("could not allocate %lu bytes?\n",
				(unsigned long)sizeof(struct ehbigint));
		err = EHBI_NULL_STRUCT;
		Ehbi_alloc_fail(err);
		goto ehbi_alloc_l_end;
	}

	bytes = malloc(bytes_len);
	if (!bytes) {
		Ehbi_log_error1("could not allocate %lu bytes?\n",
				(unsigned long)bytes_len);
		err = EHBI_NULL_BYTES;
		Ehbi_alloc_fail(err);
		goto ehbi_alloc_l_end;
	}

	err = ehbi_init(bi, bytes, bytes_len);
	if (err) {
		Ehbi_log_error1("error %d from ehbi_init?\n", err);
		goto ehbi_alloc_l_end;
	}

	if (val) {
		err = ehbi_inc_l(bi, val);
		if (err) {
			Ehbi_log_error1("error %d from ehbi_inc_l?\n", err);
			goto ehbi_alloc_l_end;
		}
	}

ehbi_alloc_l_end:
	if (err) {
		ehbi_free(bi);
		return NULL;
	}

	return bi;
}

struct ehbigint *ehbi_alloc(size_t bytes_len)
{
	return ehbi_alloc_l(bytes_len, 0);
}

void ehbi_free(struct ehbigint *bi)
{
	if (bi) {
		if (bi->bytes) {
			free(bi->bytes);
		}
		free(bi);
	}
}
