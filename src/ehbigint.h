#ifndef EHBIGINT_H
#define EHBIGINT_H

#include <stddef.h> /* size_t */

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
   shifts the value of the ehbigint up by num_bytes number of bytes
   returns 0 on success or error_code on error
*/
int ehbi_bytes_shift_left(struct ehbigint *bi, size_t num_bytes);

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

#endif /* EHBIGINT_H */
