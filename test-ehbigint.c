#include <stdio.h>
#include <string.h>
#include <echeck.h>

#include "ehbigint.h"

int check_to_string()
{
	int err, failures;

	unsigned char bytes[4] = { 0x00, 0x01, 0x00, 0x45 };
	struct ehbigint a_bigint;
	char as_string[80];

	failures = 0;

	a_bigint.bytes = bytes;
	a_bigint.bytes_len = 4;
	a_bigint.bytes_used = 3;

	err = ehbi_to_hex_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_hex_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "0x010045");

	err = ehbi_to_decimal_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_decimal_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "65605");

	return failures;
}

int check_decimal_to_hex(void)
{
	int err, failures;
	char buf[20];
	const char *str;

	failures = 0;

	err = ehbi_decimal_to_hex("275", 3, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_decimal_to_hex\n", err);
		return 1;
	}

	failures += check_str(buf, "0x0113");

	err = ehbi_decimal_to_hex("65543", 10, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_decimal_to_hex\n", err);
		return 1;
	}

	failures += check_str(buf, "0x010007");

	str = "5088824049625";
	err = ehbi_decimal_to_hex(str, strlen(str), buf, 20);
	if (err) {
		fprintf(stderr, "error %d ehbi_decimal_to_hex\n", err);
		return 1;
	}
	failures += check_str(buf, "0x04A0D58CBFD9");

	if (failures) {
		fprintf(stderr, "%d failures in check_decimal_to_hex\n",
			failures);
	}

	return failures;
}

int check_hex_to_decimal(void)
{
	int err, failures;
	char buf[20];

	failures = 0;

	err = ehbi_hex_to_decimal("0x113", 5, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_hex_to_decimal\n", err);
		return 1;
	}

	failures += check_str(buf, "275");

	err = ehbi_hex_to_decimal("0x10007", 10, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_hex_to_decimal\n", err);
		return 1;
	}

	failures += check_str(buf, "65543");

	if (failures) {
		fprintf(stderr, "%d failures in check_hex_to_decimal\n",
			failures);
	}

	return failures;
}

int check_decimal_to_hex_to_decimal_loop(void)
{
	int failures;
	int i;
	char buf[100];
	char hex[100];
	char dec[100];
	char *numv[] = { "1", "10", "275", "65543", "17", "1025", "106" };
	int numc = 7;

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
		fprintf(stderr, "%d failures in check_hex_to_decimal\n",
			failures);
	}

	return failures;
}

int check_from_hex_to_hex_round_trip(void)
{
	int err, failures;
	unsigned char bytes_buf[20];
	char as_string[80];

	/*         char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *expected_str = "0x123456789ABCDEF012";

	struct ehbigint a_bigint;
	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;

	failures = 0;

	err =
	    ehbi_from_hex_string(&a_bigint, expected_str, strlen(expected_str));

	if (err) {
		fprintf(stderr, "error %d returned from ehbi_from_hex_string\n",
			err);
		return 1;
	}

	err = ehbi_to_hex_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d returned from ehbi_to_hex_string\n",
			err);
		return 1;
	}

	failures += check_str(as_string, expected_str);

	if (failures) {
		fprintf(stderr, "%d failures in check_string_round_trip\n",
			failures);
	}

	return failures;
}

int check_from_decimal_to_decimal_round_trip(void)
{
	int err, failures;
	unsigned char bytes_buf[20];
	char as_string[80];

	/*   const char *u64_max =    "18446744073709551615" */
	const char *expected_str = "12345678901234567890000";

	struct ehbigint a_bigint;
	a_bigint.bytes = bytes_buf;
	a_bigint.bytes_len = 20;

	failures = 0;

	err =
	    ehbi_from_decimal_string(&a_bigint, expected_str,
				     strlen(expected_str));

	if (err) {
		fprintf(stderr, "error %d ehbi_from_decimal_string\n", err);
		return 1;
	}

	err = ehbi_to_decimal_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_decimal_string\n", err);
		return 1;
	}

	failures += check_str(as_string, expected_str);

	if (failures) {
		fprintf(stderr, "%d failures in "
			"check_decimal_to_decimal_round_trip\n", failures);
	}

	return failures;
}

int check_add(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	char as_string[80];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F20000F0000000000A";
	const char *str_2 = "0x32000020000000000A";
	const char *str_3 = "0x01240001100000000014";

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
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_add(&bi3, &bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_add\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi3, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_3);

	if (failures) {
		fprintf(stderr, "%d failures in check_add\n", failures);
	}

	return failures;
}

int check_inc(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	char hex[80], dec[80];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x00F00000F00000000001";
	const char *str_2 = "0x00100000100000000001";
	const char *str_3 = "0x01000001000000000002";

	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_inc\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi1, hex, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(hex, str_3);

	if (failures) {
		fprintf(stderr, "%d failures in check_inc\n", failures);
	}

	/*
	   $ echo "9415273 + 3154116455" | bc
	   3163531728
	 */
	ehbi_set_ul(&bi1, 9415273UL);
	ehbi_set_ul(&bi2, 3154116455UL);
	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_inc\n", err);
		return 1;
	}
	err = ehbi_to_hex_string(&bi1, hex, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	err = ehbi_hex_to_decimal(hex, 80, dec, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_hex_to_decimal\n", err);
		return 1;
	}
	failures += check_str(dec, "3163531728");

	ehbi_set_ul(&bi1, 254);
	ehbi_set_ul(&bi2, 1);
	err = ehbi_inc(&bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_inc\n", err);
		return 1;
	}
	err = ehbi_to_decimal_string(&bi1, dec, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_decimal_string\n", err);
		return 1;
	}
	failures += check_str(dec, "255");

	return failures;
}

int check_dec(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	char as_string[80];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x700002000000000001";
	const char *str_2 = "0x100000100000000001";
	const char *str_3 = "0x600001F00000000000";

	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_dec(&bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_inc\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi1, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_3);

	if (failures) {
		fprintf(stderr, "%d failures in check_inc\n", failures);
	}

	return failures;
}

int check_inc_ul(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	char as_string[80];
	struct ehbigint bi1;

	const char *str_1 = "0x700000000000000001";
	const char *str_3 = "0x700000000100000000";

	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_inc_ul(&bi1, (unsigned long)0xFFFFFFFF);
	if (err) {
		fprintf(stderr, "error %d from ehbi_inc_ul\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi1, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_3);

	if (failures) {
		fprintf(stderr, "%d failures in check_inc_ul\n", failures);
	}

	return failures;
}

int check_equals(void)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x720000F";
	const char *str_2 = "0x720000F";
	const char *str_3 = "0x0F";

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
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	result = ehbi_equals(&bi1, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_equals\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_equals\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_equals(&bi1, &bi3, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_equals\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	if (failures) {
		fprintf(stderr, "%d failures in check_equals\n", failures);
	}

	return failures;
}

int check_compare(void)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	unsigned char bytes_buf3[20];
	struct ehbigint bi1, bi2, bi3;

	const char *str_1 = "0x60000000000";
	const char *str_2 = "0x6F000000000";
	const char *str_3 = "0x7F";

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
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	result = ehbi_compare(&bi1, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, 0);
	result = ehbi_less_than(&bi1, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi1, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, -1);
	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);
	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, 1);
	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi1, &bi3, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, 1);
	result = ehbi_less_than(&bi1, &bi3, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);
	result = ehbi_greater_than(&bi1, &bi3, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi3, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, -1);
	result = ehbi_less_than(&bi3, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);
	result = ehbi_greater_than(&bi3, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	if (failures) {
		fprintf(stderr, "%d failures in check_compare\n", failures);
	}

	return failures;
}

int check_compare2(void)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	struct ehbigint bi1, bi2;

	const char *str_1 = "0x00F513";
	const char *str_2 = "0x00023B";

	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_compare\n", err);
		return 1;
	}
	failures += check_int(result, -1);

	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_less_than\n", err);
		return 1;
	}
	failures += check_int(result, 1);

	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		fprintf(stderr, "error %d from ehbi_greater_than\n", err);
		return 1;
	}
	failures += check_int(result, 0);

	if (failures) {
		fprintf(stderr, "%d failures in check_compare\n", failures);
	}

	return failures;
}

int check_subtract(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[14];
	char as_string[80];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =    "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0x0F20100F00002202040A";
	const char *str_2 = "0x0320000200004404020A";
	const char *str_3 = "0x0C00100CFFFFDDFE0200";

	const char *str_4 = "0xF3FFEFF300002201FE00";

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
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_subtract(&bi3, &bi1, &bi2);
	if (err) {
		fprintf(stderr, "error %d from ehbi_subtract\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi3, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_3);

	err = ehbi_subtract(&bi3, &bi2, &bi1);
	if (err) {
		fprintf(stderr, "error %d from ehbi_subtract\n", err);
		return 1;
	}

	err = ehbi_to_hex_string(&bi3, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_4);

	if (failures) {
		fprintf(stderr, "%d failures in check_subtract\n", failures);
	}

	return failures;
}

int check_div(void)
{
	int err, failures;

	int inumerator = 287713;
	int idenominator = 571;
	int iquotient = 503;
	int iremainder = 500;

	unsigned char bytes_numerator[10];
	unsigned char bytes_denominator[10];
	unsigned char bytes_quotient[10];
	unsigned char bytes_remainder[10];

	struct ehbigint numerator;
	struct ehbigint denominator;
	struct ehbigint quotient;
	struct ehbigint remainder;

	char as_string[80];
	char expected[80];
	char *str;

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

	sprintf(as_string, "0x%04X", inumerator);
	err = ehbi_from_hex_string(&numerator, as_string, strlen(as_string));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	sprintf(as_string, "0x%04X", idenominator);
	err = ehbi_from_hex_string(&denominator, as_string, strlen(as_string));
	if (err) {
		fprintf(stderr, "error %d from ehbi_from_hex_string\n", err);
		return 1;
	}

	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		fprintf(stderr, "error %d from ehbi_div\n", err);
		return 1;
	}

	failures = 0;

	err = ehbi_to_hex_string(&quotient, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	sprintf(expected, "0x%04X", iquotient);
	failures += check_str(as_string, expected);

	err = ehbi_to_hex_string(&remainder, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	sprintf(expected, "0x%04X", iremainder);
	failures += check_str(as_string, expected);

	/*
	   lldiv_t result = lldiv(5088824049625,33554393);
	   result.quot: 151658, result.remainder: 31916031
	 */
	str = "5088824049625";
	err = ehbi_from_decimal_string(&numerator, str, strlen(str));
	if (err) {
		fprintf(stderr,
			"error %d ehbi_from_decimal_string(&numerator, %s, %lu)\n",
			err, str, (unsigned long)strlen(str));
		return 1;
	}
	str = "33554393";
	denominator.bytes_used = 0;
	ehbi_from_decimal_string(&denominator, str, strlen(str));
	err = ehbi_div(&quotient, &remainder, &numerator, &denominator);
	if (err) {
		fprintf(stderr, "error %d from ehbi_div\n", err);
		return 1;
	}
	err = ehbi_to_decimal_string(&quotient, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_decimal_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "151658");
	err = ehbi_to_decimal_string(&remainder, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d ehbi_to_decimal_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "31916031");

	if (failures) {
		fprintf(stderr, "%d failures in check_div\n", failures);
	}

	return failures;
}

int check_set()
{
	int err, failures;

	unsigned char a_bytes[10];
	struct ehbigint a_bigint;
	char as_string[80];
	unsigned long three = 3;

	unsigned char b_bytes[10];
	struct ehbigint b_bigint;

	failures = 0;

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 10;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 10;
	b_bigint.bytes_used = 0;

	err = ehbi_set_ul(&a_bigint, three);
	if (err) {
		fprintf(stderr, "error %d from ehbi_set_ul\n", err);
		return 1;
	}
	err = ehbi_to_hex_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "0x03");

	err = ehbi_set(&b_bigint, &a_bigint);
	if (err) {
		fprintf(stderr, "error %d from ehbi_set_ul\n", err);
		return 1;
	}
	err = ehbi_to_hex_string(&b_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	failures += check_str(as_string, "0x03");
	return failures;
}

int check_mul()
{
	/*
	   $ bc <<< "9415273 * 252533"
	   2377667136509
	 */
	int err, failures;

	unsigned char a_bytes[16];
	struct ehbigint a_bigint;
	char hex[80], dec[80];

	unsigned char b_bytes[16];
	struct ehbigint b_bigint;

	unsigned char result_bytes[16];
	struct ehbigint result;

	failures = 0;

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 16;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 16;
	b_bigint.bytes_used = 0;

	err = ehbi_set_ul(&a_bigint, 9415273);
	if (err) {
		fprintf(stderr, "error %d from ehbi_set_ul\n", err);
		return 1;
	}

	err = ehbi_set_ul(&b_bigint, 252533);
	if (err) {
		fprintf(stderr, "error %d from ehbi_set_ul\n", err);
		return 1;
	}

	result.bytes = result_bytes;
	result.bytes_len = 16;
	err = ehbi_mul(&result, &a_bigint, &b_bigint);

	err = ehbi_to_hex_string(&result, hex, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_to_hex_string\n", err);
		return 1;
	}
	err = ehbi_hex_to_decimal(hex, 80, dec, 80);
	if (err) {
		fprintf(stderr, "error %d from ehbi_hex_to_decimal\n", err);
		return 1;
	}
	failures += check_str(dec, "2377667136509");

	return failures;
}

int main(void)
{
	int failures;

	failures = 0;

	failures += check_to_string();
	failures += check_decimal_to_hex();
	failures += check_hex_to_decimal();
	failures += check_decimal_to_hex_to_decimal_loop();
	failures += check_from_hex_to_hex_round_trip();
	failures += check_from_decimal_to_decimal_round_trip();
	failures += check_add();
	failures += check_inc();
	failures += check_inc_ul();
	failures += check_equals();
	failures += check_compare();
	failures += check_compare2();
	failures += check_subtract();
	failures += check_dec();
	failures += check_div();
	failures += check_set();
	failures += check_mul();

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
