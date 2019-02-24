/*
ehbigint-malloc.c: malloc wrapper for ehbigints
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
