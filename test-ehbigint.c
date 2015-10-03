#include <stdio.h>
#include <string.h>
#include <echeck.h>

#include "ehbigint.h"

int check_decimal_to_hex(void)
{
	int err, failures;
	char buf[20];

	failures = 0;

	err = ehbi_decimal_to_hex("275", 3, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_decimal_to_hex\n", err);
		return 1;
	}

	failures += check_str(buf, "0x113");

	err = ehbi_decimal_to_hex("65543", 10, buf, 20);

	if (err) {
		fprintf(stderr, "error %d ehbi_decimal_to_hex\n", err);
		return 1;
	}

	failures += check_str(buf, "0x10007");

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

int check_add(void)
{
	int err, failures;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[20];
	unsigned char bytes_buf3[20];
	char as_string[80];
	struct ehbigint bi1, bi2, bi3;

	/*  char *u64_max =   "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0xF20000F0000000000A";
	const char *str_2 = "0x32000020000000000A";
	const char *str_3 = "0x1240001100000000014";

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
	char as_string[80];
	struct ehbigint bi1, bi2;

	/*  char *u64_max = "0xFFFFFFFFFFFFFFFF" */
	const char *str_1 = "0xF00000F00000000001";
	const char *str_2 = "0x100000100000000001";
	const char *str_3 = "0x1000001000000000002";

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

	const char *str_1 = "0xF00000000000000001";
	const char *str_3 = "0xF00000000100000000";

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

/* int main(int argc, char *argv[]) */
int main(void)
{
	int failures = 0;

	failures += check_decimal_to_hex();
	failures += check_hex_to_decimal();
	failures += check_decimal_to_hex_to_decimal_loop();
	failures += check_from_hex_to_hex_round_trip();
	failures += check_add();
	failures += check_inc();
	failures += check_inc_ul();

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
