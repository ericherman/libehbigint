#ifndef EHBIGINT_H
#define EHBIGINT_H

#include <stdlib.h>

struct ehbigint {
	unsigned char *bytes;
	size_t bytes_len;
	size_t bytes_used;
};

/*
   populates an ehbigint with a hex string value e.g. "0x4badc0de"
   returns 0 on success or error_code on error
*/
int ehbi_from_hex_string(struct ehbigint *bi, const char *str, size_t str_len);

/*
   populates the passed in buffer with a hex string representation
   of the ehbigint
   returns 0 on success or error_code on error
*/
int ehbi_to_hex_string(struct ehbigint *bi, char *buf, size_t buf_len);

#endif /* EHBIGINT_H */
