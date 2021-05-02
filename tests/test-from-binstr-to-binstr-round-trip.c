/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-from-binstr-to-binstr-round-trip.c */
/* Copyright (C) 2017, 2019 Eric Herman <eric@freesa.org> */

#include "test-ehbigint-private-utils.h"

unsigned test_from_binstr_to_binstr_round_trip_s(int verbose, const char *bstr)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
	unsigned char bytes_buf[20];
	char as_string[BUFLEN];
	struct ehbigint a_bigint;
	size_t i, prefix, skip, ones;

	VERBOSE_ANNOUNCE(verbose);
	failures = 0;

	prefix = (bstr && (eembed_strlen(bstr) > 2)
		  && (bstr[1] == 'b' || bstr[1] == 'B')) ? 2 : 0;

	err = 0;
	ehbi_init(&a_bigint, bytes_buf, 20);

	ehbi_set_binary_string(&a_bigint, bstr, eembed_strlen(bstr), &err);
	if (err) {
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_set_binary_string(");
		log->append_s(log, bstr);
		log->append_s(log, "). Aborting test.");
		log->append_eol(log);
		return 1;
	}

	err = 0;
	ehbi_to_binary_string(&a_bigint, as_string, BUFLEN, &err);
	if (err) {
		++failures;
		STDERR_FILE_LINE_FUNC(log);
		log->append_s(log, "error ");
		log->append_l(log, err);
		log->append_s(log, " from ehbi_to_binary_string");
		log->append_eol(log);
	}

	/* skip the 0b */
	skip = 2;
	/* skip extra byte of zero padding if present */
	if ((eembed_strlen(as_string + skip) - 8) ==
	    (eembed_strlen(bstr + prefix))) {
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
		log->append_l(log, failures);
		log->append_s(log,
			      " faiures in "
			      "test_from_binstr_to_binstr_round_trip(");
		log->append_s(log, bstr);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_hex_vs_binary_string(int verbose, const char *hex,
				   const char *bstr)
{
	struct eembed_log *log = eembed_err_log;
	int err;
	unsigned failures;
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

	err = 0;
	ehbi_init(&a_bigint, a_bytes, 20);
	ehbi_init(&b_bigint, b_bytes, 20);

	ehbi_set_hex_string(&a_bigint, hex, eembed_strlen(hex), &err);
	ehbi_to_hex_string(&a_bigint, hexhexs, BUFLEN, &err);
	ehbi_to_binary_string(&a_bigint, hexbins, BUFLEN, &err);

	ehbi_set_binary_string(&b_bigint, bstr, eembed_strlen(bstr), &err);
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
		log->append_l(log, failures);
		log->append_s(log, " faiures in test_hex_vs_binary_string(");
		log->append_s(log, hex);
		log->append_s(log, ", ");
		log->append_s(log, bstr);
		log->append_s(log, ")");
		log->append_eol(log);
	}

	return failures;
}

unsigned test_from_binstr_to_binstr_round_trip(int v)
{
	unsigned failures = 0;
	const char *bin_str, *hex_str;

	bin_str = "0b00001111010101010101010101010000";
	failures += test_from_binstr_to_binstr_round_trip_s(v, bin_str);

	bin_str = "0b1111111100001111010101010101010101010000";
	failures += test_from_binstr_to_binstr_round_trip_s(v, bin_str);

	bin_str = "1011111100001111010101010101010101010001";
	failures += test_from_binstr_to_binstr_round_trip_s(v, bin_str);

	hex_str = "0x0A0F";
	bin_str = "0b0000101000001111";
	failures += test_hex_vs_binary_string(v, hex_str, bin_str);

	return failures;
}

ECHECK_TEST_MAIN_V(test_from_binstr_to_binstr_round_trip)
