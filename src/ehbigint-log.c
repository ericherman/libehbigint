/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* ehbigint-error.c: support-functions for ehbigint */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "ehbigint-log.h"

#include "ehbigint-str.h"	/* ehbi_to_hex_string ehbi_byte_to_hex_chars */
#include "ehbigint-util.h"	/* ehbi_stack_alloc */

#ifdef _POSIX_SOURCE
#include <execinfo.h>		/* backtrace backtrace_symbols_fd */
#include <stdio.h>		/* fileno */
#endif

FILE *global_ehbi_log_file = NULL;

FILE *ehbi_log_file()
{
	if (global_ehbi_log_file == NULL) {
		global_ehbi_log_file = stderr;
	}
	return global_ehbi_log_file;
}

void set_ehbi_log_file(FILE *log)
{
	global_ehbi_log_file = log;
}

void ehbi_log_backtrace(FILE *log)
{
#if _POSIX_SOURCE
	void *array[4096];
	size_t size;

	size = backtrace(array, 4096);
	backtrace_symbols_fd(array, size, fileno(log));
#else
	fprintf(log, "(backtrace unavailable)\n");
#endif /* EHBI_CAN_BACKTRACE */
}
