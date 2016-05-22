#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "ehbigint.h"
#include "test-ehbigint-private-utils.h"

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

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_inc_ul(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
