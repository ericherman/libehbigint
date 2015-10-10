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

/*
   populates the first ehbigint with the sum of the second and third
   returns 0 on success or error_code on error
*/
int ehbi_add(struct ehbigint *res, struct ehbigint *bi1, struct ehbigint *bi2);

/*
   increments the first ehbigint by the value in the second parameter
   returns 0 on success or error_code on error
*/
int ehbi_inc(struct ehbigint *bi, struct ehbigint *val);

/*
   increments the ehbigint by the value in the second parameter
   returns 0 on success or error_code on error
*/
int ehbi_inc_ul(struct ehbigint *bi, unsigned long val);

/*
   populates the first ehbigint with the value of the second perameter minus
   the third
   returns 0 on success or error_code on error
*/
int ehbi_subtract(struct ehbigint *res, struct ehbigint *bi1,
		  struct ehbigint *bi2);

/*
   returns 1 if the values represented by the ehbigint arguments are equal
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_equals(struct ehbigint *bi1, struct ehbigint *bi2, int *err);

/*
   returns 1 if the first parameter is less than the second
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_less_than(struct ehbigint *bi1, struct ehbigint *bi2, int *err);

/*
   returns 1 if the first parameter is greater than the second
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_greater_than(struct ehbigint *bi1, struct ehbigint *bi2, int *err);

/*
   returns 0 if the values represented by the ehbigint arguments are equal
   returns <0 if the first ehbigint is less than the second
   returns >0 if the first ehbigint is greater than the second
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_compare(struct ehbigint *bi1, struct ehbigint *bi2, int *err);

/*
   converts an arbitrarily long string of decimal digits into hex
   returns 0 on success or error_code on error
*/
int ehbi_decimal_to_hex(const char *dec_str, size_t dec_len, char *buf,
			size_t buf_len);

/*
   converts an arbitrarily long string of hex digits into decimal
   returns 0 on success or error_code on error
*/
int ehbi_hex_to_decimal(const char *hex_str, size_t hex_len, char *buf,
			size_t buf_len);

/* error codes */
enum {
	EHBI_SUCCESS = 0,
	EHBI_NULL_CHAR_PTR,
	EHBI_BAD_INPUT,
	EHBI_NOT_HEX,
	EHBI_BAD_HIGH_NIBBLE,
	EHBI_BAD_LOW_NIBBLE,
	EHBI_NULL_STRUCT,
	EHBI_NULL_STRING,
	EHBI_NULL_STRING_BUF,
	EHBI_NULL_ARGS,
	EHBI_ZERO_LEN_STRING,
	EHBI_STRING_BUF_TOO_SMALL,
	EHBI_STRING_BUF_TOO_SMALL_PARTIAL,
	EHBI_STRING_BUF_TOO_SMALL_NO_NULL,
	EHBI_NULL_BYTES,
	EHBI_BYTES_TOO_SMALL,
	EHBI_BYTES_TOO_SMALL_FOR_CARRY,
	EHBI_BAD_DATA,
	EHBI_CORRUPT_DATA,
	EHBI_LAST
};

#ifndef EHBI_LOG_ERROR0
#define EHBI_LOG_ERROR0(format) \
 fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(stderr, format); \
 fprintf(stderr, "\n")
#endif

#ifndef EHBI_LOG_ERROR1
#define EHBI_LOG_ERROR1(format, arg) \
 fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(stderr, format, arg); \
 fprintf(stderr, "\n")
#endif

#ifndef EHBI_LOG_ERROR2
#define EHBI_LOG_ERROR2(format, arg1, arg2) \
 fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(stderr, format, arg1, arg2); \
 fprintf(stderr, "\n")
#endif

#endif /* EHBIGINT_H */
