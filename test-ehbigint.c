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
		fprintf(stderr, "error %d returned from ehbi_from_hex_string",
			err);
		return 1;
	}

	err = ehbi_to_hex_string(&a_bigint, as_string, 80);
	if (err) {
		fprintf(stderr, "error %d returned from ehbi_as_string", err);
		return 1;
	}

	failures += check_str(as_string, expected_str);

	if (failures) {
		fprintf(stderr, "%d failures in check_string_round_trip\n",
			failures);
	}

	return failures;
}

/* int main(int argc, char *argv[]) */
int main(void)
{
	int failures = 0;

	failures += check_hex_round_trip();

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
