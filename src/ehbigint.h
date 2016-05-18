#ifndef EHBIGINT_H
#define EHBIGINT_H

#include <stdio.h>
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
   populates an ehbigint with a decimal string value e.g. "1234567890123456789"
   returns 0 on success or error_code on error
*/
int ehbi_from_decimal_string(struct ehbigint *bi, const char *dec, size_t len);

/*
   populates the passed in buffer with a hex string representation
   of the ehbigint
   returns 0 on success or error_code on error
*/
int ehbi_to_hex_string(const struct ehbigint *bi, char *buf, size_t buf_len);

/*
   populates the passed in buffer with a hex string representation
   of the ehbigint
   returns 0 on success or error_code on error
*/
int ehbi_to_decimal_string(const struct ehbigint *bi, char *buf,
			   size_t buf_len);

/*
   populates an ehbigint with the value
   returns 0 on success or error_code on error
*/
int ehbi_set(struct ehbigint *bi, const struct ehbigint *val);

/*
   populates an ehbigint with the value
   returns 0 on success or error_code on error
*/
int ehbi_set_ul(struct ehbigint *bi, unsigned long val);

/*
   populates the first ehbigint with the sum of the second and third
   returns 0 on success or error_code on error
*/
int ehbi_add(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2);

/*
   populates the first ehbigint with the sum of the second and third
   returns 0 on success or error_code on error
*/
int ehbi_mul(struct ehbigint *res, const struct ehbigint *bi1,
	     const struct ehbigint *bi2);

/*
   populates the first ehbigint quotient and remainder with the results
   of the numerator divided by the denominator
   returns 0 on success or error_code on error
*/
int ehbi_div(struct ehbigint *quotient, struct ehbigint *remainder,
	     const struct ehbigint *numerator,
	     const struct ehbigint *denominator);

/*
   increments the first ehbigint by the value in the second parameter
   returns 0 on success or error_code on error
*/
int ehbi_inc(struct ehbigint *bi, const struct ehbigint *val);

/*
   decrements the first ehbigint by the value in the second parameter
   returns 0 on success or error_code on error
*/
int ehbi_dec(struct ehbigint *bi, const struct ehbigint *val);

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
int ehbi_subtract(struct ehbigint *res, const struct ehbigint *bi1,
		  const struct ehbigint *bi2);

/*
   returns 1 if the values represented by the ehbigint arguments are equal
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_equals(const struct ehbigint *bi1, const struct ehbigint *bi2,
		int *err);

/*
   returns 1 if the high used byte is greather than 7F
   returns 0 otherwise
*/
int ehbi_is_negative(const struct ehbigint *bi);

/*
   returns 1 if the first parameter is less than the second
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_less_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		   int *err);

/*
   returns 1 if the first parameter is greater than the second
   returns 0 otherwise
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_greater_than(const struct ehbigint *bi1, const struct ehbigint *bi2,
		      int *err);

/*
   returns 0 if the values represented by the ehbigint arguments are equal
   returns <0 if the first ehbigint is less than the second
   returns >0 if the first ehbigint is greater than the second
   populates the contents of err with 0 on success or error_code on error
*/
int ehbi_compare(const struct ehbigint *bi1, const struct ehbigint *bi2,
		 int *err);

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
	EHBI_BYTES_TOO_SMALL_FOR_BORROW,
	EHBI_BAD_DATA,
	EHBI_CORRUPT_DATA,
	EHBI_STACK_TOO_SMALL,
	EHBI_LAST
};

/* yes, this would be better if dynamic, maybe later */
#ifndef EHBI_LOG_FILE
#define EHBI_LOG_FILE stderr
#endif

void ehbi_log_backtrace(FILE *log);

#ifndef EHBI_LOG_ERROR0
#define EHBI_LOG_ERROR0(format) \
 fprintf(EHBI_LOG_FILE, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(EHBI_LOG_FILE, format); \
 fprintf(EHBI_LOG_FILE, "\n"); \
 ehbi_log_backtrace(EHBI_LOG_FILE)
#endif

#ifndef EHBI_LOG_ERROR1
#define EHBI_LOG_ERROR1(format, arg) \
 fprintf(EHBI_LOG_FILE, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(EHBI_LOG_FILE, format, arg); \
 fprintf(EHBI_LOG_FILE, "\n"); \
 ehbi_log_backtrace(EHBI_LOG_FILE)
#endif

#ifndef EHBI_LOG_ERROR2
#define EHBI_LOG_ERROR2(format, arg1, arg2) \
 fprintf(EHBI_LOG_FILE, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(EHBI_LOG_FILE, format, arg1, arg2); \
 fprintf(EHBI_LOG_FILE, "\n"); \
 ehbi_log_backtrace(EHBI_LOG_FILE)
#endif

#ifndef EHBI_LOG_ERROR3
#define EHBI_LOG_ERROR3(format, arg1, arg2, arg3) \
 fprintf(EHBI_LOG_FILE, "%s:%d: ", __FILE__, __LINE__); \
 fprintf(EHBI_LOG_FILE, format, arg1, arg2, arg3); \
 fprintf(EHBI_LOG_FILE, "\n"); \
 ehbi_log_backtrace(EHBI_LOG_FILE)
#endif

#endif /* EHBIGINT_H */