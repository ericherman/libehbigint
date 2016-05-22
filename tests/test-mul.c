#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "ehbigint.h"
#include "test-ehbigint-private-utils.h"

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

	char buf[BUFLEN];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	a_bigint.bytes = a_bytes;
	a_bigint.bytes_len = 16;
	a_bigint.bytes_used = 0;

	b_bigint.bytes = b_bytes;
	b_bigint.bytes_len = 16;
	b_bigint.bytes_used = 0;

	result.bytes = result_bytes;
	result.bytes_len = 16;

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

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_mul(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
