#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>
#include <errno.h>

#include "ehbigint.h"

#define TEST_FUNC \
	((ECHECK_FUNC == NULL) ? "" : ECHECK_FUNC)

#define STDERR_FILE_LINE_FUNC \
	fprintf(stderr, "%s:%d%s%s%s: ", __FILE__, __LINE__, \
	(ECHECK_FUNC == NULL) ? "" : ":", \
	TEST_FUNC, \
	(ECHECK_FUNC == NULL) ? "" : "()")

#define LOG_ERROR(format) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format)

#define LOG_ERROR1(format, arg) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg)

#define LOG_ERROR2(format, arg1, arg2) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2)

#define LOG_ERROR3(format, arg1, arg2, arg3) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3)

#define LOG_ERROR4(format, arg1, arg2, arg3, arg4) \
	STDERR_FILE_LINE_FUNC; fprintf(stderr, format, arg1, arg2, arg3, arg4)

#define VERBOSE_ANNOUNCE(verbose) \
	if (verbose) { fprintf(stderr, "starting %s\n", TEST_FUNC); }

#define BUFLEN 80

int check_ehbigint_dec(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual[BUFLEN];
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	err = ehbi_to_decimal_string(val, actual, BUFLEN);
	if (err) {
		LOG_ERROR2("error %d ehbi_to_decimal_string (%s)\n", err, buf);
		return 1;
	}

	return check_str_m(actual, expected, buf);
}

int check_ehbigint_hex(struct ehbigint *val, const char *expected, int line,
		       const char *msg)
{
	int err;
	char actual[BUFLEN];
	char buf[BUFLEN];

	sprintf(buf, "%s:%d", msg, line);
	err = ehbi_to_hex_string(val, actual, BUFLEN);
	if (err) {
		LOG_ERROR2("error %d ehbi_to_hex_string (%s)\n", err, buf);
		return 1;
	}

	return check_str_m(actual, expected, buf);
}

unsigned long ehbigint_to_unsigned_long(struct ehbigint *val, int *err)
{
	int base;
	char dec[BUFLEN];
	char *endptr;
	unsigned long result;

	*err = ehbi_to_decimal_string(val, dec, BUFLEN);
	if (*err) {
		LOG_ERROR1("error %d ehbi_to_decimal_string\n", *err);
		return 0;
	}
	base = 10;
	errno = 0;
	result = strtoul(dec, &endptr, base);
	*err = errno;
	if (*err || (endptr && strlen(endptr) > 0)) {
		LOG_ERROR4("strtoul('%s'); (invalid:'%s') (%d: %s)\n", dec,
			   endptr, *err, strerror(*err));
	}
	return result;
}

int test_to_string(int verbose)
{
	int failures;

	unsigned char bytes[4] = { 0x00, 0x01, 0x00, 0x45 };
	struct ehbigint a_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = bytes;
	a_bigint.bytes_len = 4;
	a_bigint.bytes_used = 3;

	failures +=
	    check_ehbigint_hex(&a_bigint, "0x010045", __LINE__, TEST_FUNC);
	failures += check_ehbigint_dec(&a_bigint, "65605", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_to_string\n", failures);
	}

	return failures;
}

int test_decimal_to_hex(int verbose)
{
	int err, failures;
	char buf[20];
	const char *str;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = ehbi_decimal_to_hex("275", 3, buf, 20);

	if (err) {
		LOG_ERROR1("error %d ehbi_decimal_to_hex\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(buf, "0x0113");

	err = ehbi_decimal_to_hex("65543", 10, buf, 20);

	if (err) {
		LOG_ERROR1("error %d ehbi_decimal_to_hex\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(buf, "0x010007");

	str = "5088824049625";
	err = ehbi_decimal_to_hex(str, strlen(str), buf, 20);
	if (err) {
		LOG_ERROR1("error %d ehbi_decimal_to_hex\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_str(buf, "0x04A0D58CBFD9");

	if (failures) {
		LOG_ERROR1("%d failures in test_decimal_to_hex\n", failures);
	}

	return failures;
}

int test_hex_to_decimal(int verbose)
{
	int err, failures;
	char buf[20];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err = ehbi_hex_to_decimal("0x113", 5, buf, 20);

	if (err) {
		LOG_ERROR1("error %d ehbi_hex_to_decimal\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(buf, "275");

	err = ehbi_hex_to_decimal("0x10007", 10, buf, 20);

	if (err) {
		LOG_ERROR1("error %d ehbi_hex_to_decimal\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(buf, "65543");

	if (failures) {
		LOG_ERROR1("%d failures in test_hex_to_decimal\n", failures);
	}

	return failures;
}

int test_decimal_to_hex_to_decimal_loop(int verbose)
{
	int failures;
	int i;
	char buf[100];
	char hex[100];
	char dec[100];
	char *numv[] = { "1", "10", "275", "65543", "17", "1025", "106" };
	int numc = 7;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	for (i = 0; i < numc; ++i) {
		ehbi_decimal_to_hex(numv[i], strlen(numv[i]), hex, 100);
		ehbi_hex_to_decimal(hex, 100, dec, 100);
		failures += check_str(dec, numv[i]);
	}

	/* lets just do a bunch */
	for (i = 1; i < 1000001; i += 25) {
		sprintf(buf, "%d", i);
		ehbi_decimal_to_hex(buf, strlen(buf), hex, 100);
		ehbi_hex_to_decimal(hex, 100, dec, 100);
		failures += check_str(dec, buf);
	}

	if (failures) {
		LOG_ERROR1("%d failures in test_hex_to_decimal\n", failures);
	}

	return failures;
}

int test_from_hex_to_hex_round_trip(int verbose)
{
	int err, failures;
	unsigned char bytes_buf[20];

	/*         char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *expected_str = "0x123456789ABCDEF012";

	struct ehbigint a_bigint;
	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err =
	    ehbi_from_hex_string(&a_bigint, expected_str, strlen(expected_str));

	if (err) {
		LOG_ERROR1("error %d returned from ehbi_from_hex_string\n",
			   err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures +=
	    check_ehbigint_hex(&a_bigint, expected_str, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_string_round_trip\n", failures);
	}

	return failures;
}

int test_from_decimal_to_decimal_round_trip(int verbose)
{
	int err, failures;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];

	/*   const char *u64_max =    "18446744073709551615" */
	const char *expected_str = "12345678901234567890000";

	struct ehbigint a_bigint;
	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	err =
	    ehbi_from_decimal_string(&a_bigint, expected_str,
				     strlen(expected_str));

	if (err) {
		LOG_ERROR1("error %d ehbi_from_decimal_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_to_decimal_string(&a_bigint, as_string, BUFLEN);
	if (err) {
		LOG_ERROR1("error %d ehbi_to_decimal_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_str(as_string, expected_str);

	if (failures) {
		LOG_ERROR1("%d failures in "
			   "check_decimal_to_decimal_round_trip\n", failures);
	}

	return failures;
}

int test_add(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F20000F0000000000A";
	const char *str_2 = "0x32000020000000000A";
	const char *str_3 = "0x01240001100000000014";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_add(&bi3, &bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_add\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi3, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_add\n", failures);
	}

	return failures;
}

int test_inc(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F00000F00000000001";
	const char *str_2 = "0x00100000100000000001";
	const char *str_3 = "0x01000001000000000002";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_inc\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi1, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_inc\n", failures);
	}

	/*
	   $ echo "9415273 + 3154116455" | bc
	   3163531728
	 */
	err = ehbi_set_ul(&bi1, 9415273UL);
	err += ehbi_set_ul(&bi2, 3154116455UL);
	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_inc\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&bi1, "3163531728", __LINE__, TEST_FUNC);

	err = ehbi_set_ul(&bi1, 254);
	err += ehbi_set_ul(&bi2, 1);
	err += ehbi_inc(&bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_inc\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&bi1, "255", __LINE__, TEST_FUNC);

	return failures;
}

int test_dec(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x700002000000000001";
	const char *str_2 = "0x100000100000000001";
	const char *str_3 = "0x600001F00000000000";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_inc\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi1, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_inc\n", failures);
	}

	return failures;
}

int test_inc_ul(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	struct ehbigint bi1;

	const char *str_1 = "0x700000000000000001";
	const char *str_3 = "0x700000000100000000";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_inc_ul(&bi1, (unsigned long)0xFFFFFFFF);
	if (err) {
		LOG_ERROR1("error %d from ehbi_inc_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi1, str_3, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_inc_ul\n", failures);
	}

	return failures;
}

int test_equals(int verbose)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x720000F";
	const char *str_2 = "0x720000F";
	const char *str_3 = "0x0F";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	err += ehbi_from_hex_string(&bi3, str_3, strlen(str_3));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_equals(&bi1, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi3, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_equals\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	if (failures) {
		LOG_ERROR1("%d failures in test_equals\n", failures);
	}

	return failures;
}

int test_compare(int verbose)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x60000000000";
	const char *str_2 = "0x6F000000000";
	const char *str_3 = "0x7F";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	err += ehbi_from_hex_string(&bi3, str_3, strlen(str_3));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_compare(&bi1, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);
	result = ehbi_less_than(&bi1, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi1, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, -1);
	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);
	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);
	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi1, &bi3, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);
	result = ehbi_less_than(&bi1, &bi3, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi1, &bi3, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi3, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, -1);
	result = ehbi_less_than(&bi3, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);
	result = ehbi_greater_than(&bi3, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	if (failures) {
		LOG_ERROR1("%d failures in test_compare\n", failures);
	}

	return failures;
}

int test_compare2(int verbose)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	struct ehbigint bi1, bi2;

	const char *str_1 = "0x00F513";
	const char *str_2 = "0x00023B";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, -1);

	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	if (failures) {
		LOG_ERROR1("%d failures in test_compare\n", failures);
	}

	return failures;
}

int test_subtract(int verbose)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[14];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =    "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x0F20100F00002202040A";
	const char *str_2 = "0x0320000200004404020A";
	const char *str_3 = "0x0C00100CFFFFDDFE0200";

	const char *str_4 = "0xF3FFEFF300002201FE00";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	bi3.bytes = bytes_buf3;
	bi3.bytes_len = 14;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_subtract(&bi3, &bi1, &bi2);
	if (err) {
		LOG_ERROR1("error %d from ehbi_subtract\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi3, str_3, __LINE__, TEST_FUNC);

	err = ehbi_subtract(&bi3, &bi2, &bi1);
	if (err) {
		LOG_ERROR1("error %d from ehbi_subtract\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures += check_ehbigint_hex(&bi3, str_4, __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_subtract\n", failures);
	}

	return failures;
}

int test_div(int verbose)
{
	int err, failures;

	unsigned long ulnumerator = 287713;
	unsigned long uldenominator = 571;
	unsigned long uliquotient = 503;
	unsigned long ulremainder = 500;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	char as_string[BUFLEN];
	char expected[BUFLEN];
	char *str;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	numerator.bytes = bytes_numerator;
	numerator.bytes_len = 10;
	numerator.bytes_used = 0;

	denominator.bytes = bytes_denominator;
	denominator.bytes_len = 10;
	denominator.bytes_used = 0;

	quotient.bytes = bytes_quotient;
	quotient.bytes_len = 10;
	quotient.bytes_used = 0;

	remainder.bytes = bytes_remainder;
	remainder.bytes_len = 10;
	remainder.bytes_used = 0;

	sprintf(as_string, "%lu", ulnumerator);
	err =
	    ehbi_from_decimal_string(&numerator, as_string, strlen(as_string));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&numerator, as_string, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("round trip failed %lu\n", ulnumerator);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	sprintf(as_string, "%lu", uldenominator);
	err =
	    ehbi_from_decimal_string(&denominator, as_string,
				     strlen(as_string));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&denominator, as_string, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("round trip failed %lu\n", uldenominator);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		LOG_ERROR1("error %d from ehbi_div\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	sprintf(expected, "%lu", uliquotient);
	failures +=
	    check_ehbigint_dec(&quotient, expected, __LINE__, TEST_FUNC);

	sprintf(expected, "%lu", ulremainder);
	failures +=
	    check_ehbigint_dec(&remainder, expected, __LINE__, TEST_FUNC);

	/*
	   lldiv_t result = lldiv(5088824049625,33554393);
	   result.quot: 151658, result.remainder: 31916031
	 */
	str = "5088824049625";
	err = ehbi_from_decimal_string(&numerator, str, strlen(str));
	if (err) {
		LOG_ERROR3
		    ("error %d ehbi_from_decimal_string(&numerator, %s, %lu)\n",
		     err, str, (unsigned long)strlen(str));
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	str = "33554393";
	denominator.bytes_used = 0;
	ehbi_from_decimal_string(&denominator, str, strlen(str));
	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		LOG_ERROR1("error %d from ehbi_div\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	failures +=
	    check_ehbigint_dec(&quotient, "151658", __LINE__, TEST_FUNC);
	failures +=
	    check_ehbigint_dec(&remainder, "31916031", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_div\n", failures);
	}

	return failures;
}

int test_set(int verbose)
{
	int err, failures;

	unsigned char a_bytes[10];
	struct ehbigint a_bigint;
	unsigned long three = 3;

	unsigned char b_bytes[10];
	struct ehbigint b_bigint;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 10;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 10;
	b_bigint.bytes_used = 0;

	err = ehbi_set_ul(&a_bigint, three);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_hex(&a_bigint, "0x03", __LINE__, TEST_FUNC);

	err = ehbi_set(&b_bigint, &a_bigint);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_hex(&b_bigint, "0x03", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_set\n", failures);
	}

	return failures;
}

int test_mul(int verbose)
{
	/*
	   $ bc <<< "9415273 * 252533"
	   2377667136509
	 */
	int err, failures;

	unsigned long aul;
	unsigned char a_bytes[16];
	struct ehbigint a_bigint;

	unsigned long bul;
	unsigned char b_bytes[16];
	struct ehbigint b_bigint;

	unsigned char result_bytes[16];
	struct ehbigint result;

	char buf[80];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 16;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 16;
	b_bigint.bytes_used = 0;

	aul = 9415273;
	sprintf(buf, "%lu", aul);
	err = ehbi_set_ul(&a_bigint, aul);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&a_bigint, buf, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("assign failed %s\n", buf);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	bul = 252533;
	sprintf(buf, "%lu", bul);
	err = ehbi_set_ul(&b_bigint, bul);
	if (err) {
		LOG_ERROR1("error %d from ehbi_set_ul\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_ehbigint_dec(&b_bigint, buf, __LINE__, TEST_FUNC);
	if (failures) {
		LOG_ERROR1("assign failed %s\n", buf);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result.bytes = result_bytes;
	result.bytes_len = 16;
	err = ehbi_mul(&result, &a_bigint, &b_bigint);
	if (err) {
		LOG_ERROR1("error %d from ehbi_mul\n", err);
	}

	failures +=
	    check_ehbigint_dec(&result, "2377667136509", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_mul\n", failures);
	}

	return failures;
}

int test_scenario_mul_mod(int verbose)
{
	int err, failures;
	struct ehbigint bx, by, bz, bresult, bquot, brem;
	unsigned long x, y, z, result;
	unsigned char xb[16], yb[16], zb[16], resb[16], quotb[16], remb[16];
	char *expect_mul;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bx.bytes = xb;
	bx.bytes_len = 16;
	bx.bytes_used = 0;

	by.bytes = yb;
	by.bytes_len = 16;
	by.bytes_used = 0;

	bz.bytes = zb;
	bz.bytes_len = 16;
	bz.bytes_used = 0;

	bresult.bytes = resb;
	bresult.bytes_len = 16;
	bresult.bytes_used = 0;

	bquot.bytes = quotb;
	bquot.bytes_len = 16;
	bquot.bytes_used = 0;

	brem.bytes = remb;
	brem.bytes_len = 16;
	brem.bytes_used = 0;

	x = 20151125;
	err = ehbi_set_ul(&bx, x);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", x, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	y = 252533;
	err = ehbi_set_ul(&by, y);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", y, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	z = 33554393;
	err = ehbi_set_ul(&bz, z);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", z, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	/* 20151125 * 252533 == 5088824049625 */
	expect_mul = "5088824049625";
	err = ehbi_mul(&bresult, &bx, &by);
	if (err) {
		LOG_ERROR3("ehbi_mul (%lu * %lu), error: %d\n", x, y, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&bresult, expect_mul, __LINE__, TEST_FUNC);

	/*
	   r = lldiv(5088824049625, 33554393);
	   r.quot: 151658, r.rem: 31916031
	 */
	err = ehbi_div(&bquot, &brem, &bresult, &bz);
	if (err) {
		LOG_ERROR3("ehbi_div: (%s/%lu) error: %d\n", expect_mul, z,
			   err);
	}
	if (brem.bytes_used > (1 + sizeof(unsigned long))) {
		LOG_ERROR2
		    ("brem.bytes_used > sizeof(unsigned long)(%lu > %lu)\n",
		     (unsigned long)brem.bytes_used,
		     (unsigned long)(1 + sizeof(unsigned long)));
		failures += 1;
	}

	result = ehbigint_to_unsigned_long(&bquot, &err);
	if (err) {
		LOG_ERROR1("ehbigint_to_unsigned_long(quot) error: %d\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_unsigned_long(result, 151658);

	result = ehbigint_to_unsigned_long(&brem, &err);
	if (err) {
		LOG_ERROR1("ehbigint_to_unsigned_long(rem) error: %d\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_unsigned_long(result, 31916031);

	if (failures) {
		LOG_ERROR1("%d failures in test_scenario_mul_mod\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;

	VERBOSE_ANNOUNCE(v);
	failures = 0;

	failures += test_decimal_to_hex(v);
	failures += test_hex_to_decimal(v);
	failures += test_decimal_to_hex_to_decimal_loop(v);
	failures += test_to_string(v);
	failures += test_from_hex_to_hex_round_trip(v);
	failures += test_from_decimal_to_decimal_round_trip(v);
	failures += test_add(v);
	failures += test_inc(v);
	failures += test_inc_ul(v);
	failures += test_equals(v);
	failures += test_compare(v);
	failures += test_compare2(v);
	failures += test_subtract(v);
	failures += test_dec(v);
	failures += test_set(v);
	failures += test_mul(v);
	failures += test_div(v);
	failures += test_scenario_mul_mod(v);

	if (failures) {
		LOG_ERROR1("%d failures in total\n", failures);
	}

	return failures;
}
