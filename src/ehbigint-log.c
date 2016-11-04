/*
ehbigint-error.c: support-functions for ehbigint
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

#include "ehbigint-log.h"

#include "ehbigint-util.h" /* ehbi_to_hex */

#include <stdarg.h> /* va_list */
#include <stdlib.h> /* exit() used in ehbi_debug_to_string */

int ehbi_debug_log_level = 0;
int ehbi_debugf(int level, const char *fmt, ...)
{
	va_list ap;
	int r;

	if (ehbi_debug_log_level < level) {
		return 0;
	}

	va_start(ap, fmt);
	r = vfprintf(stderr, fmt, ap);
	va_end(ap);
	return r;
}

void ehbi_debug_to_string(int level, struct ehbigint *bi, const char *name)
{
	char *buf, h, l;
	size_t size, i;

	if (ehbi_debug_log_level < level) {
		return;
	}

	fprintf(stderr,
		"%s (%p) => {\n\tbytes => (%p),\n" "\tbytes_len => %lu,\n"
		"\tbytes_used => %lu,\n", name, (void *)bi,
		(void *)bi->bytes, (unsigned long)bi->bytes_len,
		(unsigned long)bi->bytes_used);

	fprintf(stderr, "\tused  => ");
	for (i = bi->bytes_len; i > 0; --i) {
		fprintf(stderr, "%s", i > bi->bytes_used ? "XX" : "__");
	}
	fprintf(stderr, ",\n");

	fprintf(stderr, "\tbytes => ");
	for (i = bi->bytes_len; i > 0; --i) {
		h = '?';
		l = '?';
		ehbi_to_hex(bi->bytes[bi->bytes_len - i], &h, &l);
		fprintf(stderr, "%c%c", h, l);
	}
	fprintf(stderr, ",\n");

	size = 5 + (4 * bi->bytes_used);
	buf = ehbi_stack_alloc(size);
	if (!buf) {
		EHBI_LOG_ERROR2("Could not %s(%lu) bytes", ehbi_stack_alloc_str,
				(unsigned long)size);
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "\thex => ");
	for (i = 0; i < (bi->bytes_len - bi->bytes_used); ++i) {
		fprintf(stderr, "  ");
	}
	ehbi_to_hex_string(bi, buf, size);
	fprintf(stderr, "%s,\n", buf);

	ehbi_to_decimal_string(bi, buf, size);
	fprintf(stderr, "\tdec => %s,\n", buf);
	ehbi_stack_free(buf, size);

	fprintf(stderr, "}\n");
}

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
#if EHBI_CAN_BACKTRACE
	void *array[4096];
	size_t size;

	size = backtrace(array, 4096);
	backtrace_symbols_fd(array, size, fileno(log));
#else
	fprintf(log, "(backtrace unavailable)\n");
#endif /* EHBI_CAN_BACKTRACE */
}
