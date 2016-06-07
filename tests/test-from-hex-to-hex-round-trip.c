#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"

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
		LOG_ERROR1("%d failures in test_hex_round_trip\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_from_hex_to_hex_round_trip(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
