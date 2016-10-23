#include <stdio.h>
#include <string.h>
#include <echeck.h>
#include <stdlib.h>

#include "test-ehbigint-private-utils.h"

int test_scenario_mul_mod(int verbose)
{
	int err, failures;
	struct ehbigint bx, by, bz, bresult, bquot, brem;
	unsigned long x, y, z, result;
	unsigned char xb[16], yb[16], zb[16], resb[16], quotb[16], remb[16];
	char *expect_mul;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	bx.bytes = xb;
	bx.bytes_len = 16;
	bx.bytes_used = 0;

	by.bytes = yb;
	by.bytes_len = 16;
	by.bytes_used = 0;

	bz.bytes = zb;
	bz.bytes_len = 16;
	bz.bytes_used = 0;

	bresult.bytes = resb;
	bresult.bytes_len = 16;
	bresult.bytes_used = 0;

	bquot.bytes = quotb;
	bquot.bytes_len = 16;
	bquot.bytes_used = 0;

	brem.bytes = remb;
	brem.bytes_len = 16;
	brem.bytes_used = 0;

	x = 20151125;
	err = ehbi_set_ul(&bx, x);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", x, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	y = 252533;
	err = ehbi_set_ul(&by, y);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", y, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	z = 33554393;
	err = ehbi_set_ul(&bz, z);
	if (err) {
		LOG_ERROR2("ehbi_set_ul (%lu) error: %d\n", z, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}

	/* 20151125 * 252533 == 5088824049625 */
	expect_mul = "5088824049625";
	err = ehbi_mul(&bresult, &bx, &by);
	if (err) {
		LOG_ERROR3("ehbi_mul (%lu * %lu), error: %d\n", x, y, err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures +=
	    check_ehbigint_dec(&bresult, expect_mul, __LINE__, TEST_FUNC);

	/*
	   r = lldiv(5088824049625, 33554393);
	   r.quot: 151658, r.rem: 31916031
	 */
	err = ehbi_div(&bquot, &brem, &bresult, &bz);
	if (err) {
		LOG_ERROR3("ehbi_div: (%s/%lu) error: %d\n", expect_mul, z,
			   err);
	}
	if (brem.bytes_used > (1 + sizeof(unsigned long))) {
		LOG_ERROR2
		    ("brem.bytes_used > sizeof(unsigned long)(%lu > %lu)\n",
		     (unsigned long)brem.bytes_used,
		     (unsigned long)(1 + sizeof(unsigned long)));
		failures += 1;
	}

	result = ehbigint_to_unsigned_long(&bquot, &err);
	if (err) {
		LOG_ERROR1("ehbigint_to_unsigned_long(quot) error: %d\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_unsigned_long(result, 151658);

	result = ehbigint_to_unsigned_long(&brem, &err);
	if (err) {
		LOG_ERROR1("ehbigint_to_unsigned_long(rem) error: %d\n", err);
		LOG_ERROR("Aborting test\n");
		return (1 + failures);
	}
	failures += check_unsigned_long(result, 31916031);

	if (failures) {
		LOG_ERROR1("%d failures in test_scenario_mul_mod\n", failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	failures += test_scenario_mul_mod(v);

	if (failures) {
		LOG_ERROR2("%d failures in %s\n", failures, __FILE__);
	}

	return cap_failures(failures);
}