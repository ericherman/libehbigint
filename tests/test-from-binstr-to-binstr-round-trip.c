/*
test-from-binstr-to-binstr-round-trip.c
Copyright (C) 2017 Eric Herman <eric@freesa.org>

This work is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This work is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.
*/
#include "test-ehbigint-private-utils.h"

int test_from_binstr_to_binstr_round_trip(int verbose, const char *bstr)
{
	int err, failures;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];
	struct ehbigint a_bigint;
	size_t i, prefix, skip, ones;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	prefix = (bstr && (strlen(bstr) > 2)
		  && (bstr[1] == 'b' || bstr[1] == 'B')) ? 2 : 0;

	ehbi_init(&a_bigint, bytes_buf, 20);

	err = ehbi_set_binary_string(&a_bigint, bstr, strlen(bstr));
	if (err) {
		Test_log_error1("error %d ehbi_set_binary_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}
	ehbi_to_binary_string(&a_bigint, as_string, BUFLEN, &err);
	if (err) {
		Test_log_error1("error %d ehbi_to_binary_string\n", err);
		Test_log_error("Aborting test\n");
		return (1 + failures);
	}

	/* skip the 0b */
	skip = 2;
	/* skip extra byte of zero padding if present */
	if ((strlen(as_string + skip) - 8) == (strlen(bstr + prefix))) {
		ones = 0;
		for (i = 0; i < 8; ++i) {
			if (*(as_string + skip + i) != '0') {
				++ones;
			}
		}
		if (ones == 0) {
			skip += 8;
		}
	}
	failures += check_str(as_string + skip, bstr + prefix);
	if (failures) {
		Test_log_error1("%d failures in "
				"check_binstr_to_binstr_round_trip\n",
				failures);
	}

	return failures;
}

int test_hex_vs_binary_string(int verbose, const char *hex, const char *bstr)
{
	int err, failures;
	struct ehbigint a_bigint;
	unsigned char a_bytes[20];
	struct ehbigint b_bigint;
	unsigned char b_bytes[20];
	char hexhexs[BUFLEN];
	char hexbins[BUFLEN];
	char binhexs[BUFLEN];
	char binbins[BUFLEN];

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	ehbi_init(&a_bigint, a_bytes, 20);
	ehbi_init(&b_bigint, b_bytes, 20);

	ehbi_set_hex_string(&a_bigint, hex, strlen(hex));
	ehbi_to_hex_string(&a_bigint, hexhexs, BUFLEN, &err);
	ehbi_to_binary_string(&a_bigint, hexbins, BUFLEN, &err);

	ehbi_set_binary_string(&b_bigint, bstr, strlen(bstr));
	ehbi_to_hex_string(&b_bigint, binhexs, BUFLEN, &err);
	ehbi_to_binary_string(&b_bigint, binbins, BUFLEN, &err);

	failures += check_ehbigint_hex(&a_bigint, hex, __LINE__, "hex != hex?");
	failures +=
	    check_ehbigint_hex(&b_bigint, hex, __LINE__, "binstr != hex!");
	failures += check_str(hex, hexhexs);
	failures += check_str(hexhexs, binhexs);

	failures += check_str(bstr, binbins);
	failures += check_str(binbins, hexbins);

	if (failures) {
		Test_log_error1("%d failures in "
				"check_binstr_to_binstr_round_trip\n",
				failures);
	}

	return failures;
}

int main(int argc, char **argv)
{
	int v, failures;
	const char *bin_str, *hex_str;

	v = (argc > 1) ? atoi(argv[1]) : 0;
	failures = 0;

	bin_str = "0b00001111010101010101010101010000";
	failures += test_from_binstr_to_binstr_round_trip(v, bin_str);

	bin_str = "0b1111111100001111010101010101010101010000";
	failures += test_from_binstr_to_binstr_round_trip(v, bin_str);

	bin_str = "1011111100001111010101010101010101010001";
	failures += test_from_binstr_to_binstr_round_trip(v, bin_str);

	hex_str = "0x0A0F";
	bin_str = "0b0000101000001111";
	failures += test_hex_vs_binary_string(v, hex_str, bin_str);

	if (failures) {
		Test_log_error2("%d failures in %s\n", failures, __FILE__);
	}

	return check_status(failures);
}
