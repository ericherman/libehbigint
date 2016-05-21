#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "ehbigint.h"
#include "test-ehbigint-private-utils.h"

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

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_set(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return failures;
}
