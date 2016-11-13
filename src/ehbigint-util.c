/*
ehbigint-util.c: support-functions for ehbigint.c
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

#include "ehbigint-util.h"
#include "ehbigint-log.h"

#ifdef EHBI_NO_ALLOCA
void ehbi_do_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		Ehbi_log_error2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
	free(ptr);
}
#else
void ehbi_no_stack_free(void *ptr, size_t size)
{
	if (size == 0) {
		Ehbi_log_error2("size is 0? (%p, %lu)\n", ptr,
				(unsigned long)size);
	}
}
#endif
