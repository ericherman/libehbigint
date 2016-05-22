#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "ehbigint.h"
#include "test-ehbigint-private-utils.h"

int test_compare2(int verbose)
{
	int err, failures, result;
	unsigned char bytes_buf1[20];
	unsigned char bytes_buf2[10];
	struct ehbigint bi1, bi2;

	const char *str_1 = "0x00F513";
	const char *str_2 = "0x00023B";

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bi1.bytes = bytes_buf1;
	bi1.bytes_len = 20;

	bi2.bytes = bytes_buf2;
	bi2.bytes_len = 10;

	err = ehbi_from_hex_string(&bi1, str_1, strlen(str_1));
	err += ehbi_from_hex_string(&bi2, str_2, strlen(str_2));
	if (err) {
		LOG_ERROR1("error %d from ehbi_from_hex_string\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	result = ehbi_compare(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_less_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	result = ehbi_greater_than(&bi1, &bi2, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_compare(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_compare\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, -1);

	result = ehbi_less_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_less_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 1);

	result = ehbi_greater_than(&bi2, &bi1, &err);
	if (err) {
		LOG_ERROR1("error %d from ehbi_greater_than\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_int(result, 0);

	if (failures) {
		LOG_ERROR1("%d failures in test_compare\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_compare2(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}
