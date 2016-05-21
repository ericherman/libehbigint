#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "ehbigint.h"
#include "test-ehbigint-private-utils.h"

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

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_add(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return failures;
}
