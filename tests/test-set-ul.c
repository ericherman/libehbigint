#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"

int test_set_ul(int verbose)
{
	int failures;

	unsigned char bytes[4];
	struct ehbigint bi;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi.bytes = bytes;
	bi.bytes_len = 4;

	ehbi_set_ul(&bi, 0);
	failures += check_ehbigint_hex(&bi, "0x00", __LINE__, TEST_FUNC);
	failures += check_ehbigint_dec(&bi, "0", __LINE__, TEST_FUNC);

	ehbi_set_ul(&bi, 1234567890);
	failures += check_ehbigint_hex(&bi, "0x499602D2", __LINE__, TEST_FUNC);
	failures += check_ehbigint_dec(&bi, "1234567890", __LINE__, TEST_FUNC);

	if (failures) {
		LOG_ERROR1("%d failures in test_set_ul\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_set_ul(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
