#ifndef EHBIGINT_ERROR_H
#define EHBIGINT_ERROR_H

#include <stdio.h>

/* Get the FILE pointer to where fprintf messages currently target.
   Defaults to stderr. */
FILE *ehbi_log_file();

/* Set the FILE pointer to where fprintf messages shall target. */
void set_ehbi_log_file(FILE *log);

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

/* if _POSIX_C_SOURCE backtrace_symbols_fd is used */
void ehbi_log_backtrace(FILE *log);

#ifndef EHBI_LOG_ERROR0
#define EHBI_LOG_ERROR0(format) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef EHBI_LOG_ERROR1
#define EHBI_LOG_ERROR1(format, arg) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef EHBI_LOG_ERROR2
#define EHBI_LOG_ERROR2(format, arg1, arg2) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg1, arg2); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#ifndef EHBI_LOG_ERROR3
#define EHBI_LOG_ERROR3(format, arg1, arg2, arg3) \
 fprintf(ehbi_log_file(), "%s:%d: ", __FILE__, __LINE__); \
 fprintf(ehbi_log_file(), format, arg1, arg2, arg3); \
 fprintf(ehbi_log_file(), "\n"); \
 ehbi_log_backtrace(ehbi_log_file())
#endif

#endif /* EHBIGINT_ERROR_H */
