#include <stdio.h>
#include <string.h>
#include <echeck.h>

#include "ehbigint.h"

int check_hex_round_trip(void)
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
		fprintf(stderr, "error %d returned from ehbi_as_string\n", err);
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
	bi3.bytes_used = 0;

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
		fprintf(stderr, "error %d from ehbi_as_string\n", err);
		return 1;
	}

	failures += check_str(as_string, str_3);

	if (failures) {
		fprintf(stderr, "%d failures in check_add\n", failures);
	}

	return failures;
}

/* int main(int argc, char *argv[]) */
int main(void)
{
	int failures = 0;

	failures += check_hex_round_trip();
	failures += check_add();

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
