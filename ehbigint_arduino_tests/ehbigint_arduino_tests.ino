/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "eembed-arduino.h"
#include "ehbigint.h"

unsigned test_add(int verbose);
unsigned test_bytes_shift_left(int verbose);
unsigned test_bytes_shift_right(int verbose);
unsigned test_compare2(int verbose);
unsigned test_compare3(int verbose);
unsigned test_compare(int verbose);
unsigned test_dec_corner_case(int verbose);
unsigned test_dec(int verbose);
unsigned test_div(int verbose);
unsigned test_equals(int verbose);
unsigned test_exp(int verbose);
unsigned test_exp_mod(int verbose);
unsigned test_from_binstr_to_binstr_round_trip(int verbose);
unsigned test_from_decimal_to_decimal_round_trip(int verbose);
unsigned test_from_hex_to_hex_round_trip(int verbose);
unsigned test_inc(int verbose);
unsigned test_inc_l(int verbose);
unsigned test_is_probably_prime(int verbose);
unsigned test_mul(int verbose);
unsigned test_n_choose_k(int verbose);
unsigned test_scenario_mul_mod(int verbose);
unsigned test_set(int verbose);
unsigned test_set_l(int verbose);
unsigned test_shift_right(int verbose);
unsigned test_sqrt(int verbose);
unsigned test_subtract(int verbose);
unsigned test_to_string(int verbose);

/* globals */
uint32_t loop_count;

void setup(void)
{
	Serial.begin(9600);

	eembed_arduino_serial_log_init();

	delay(50);

	Serial.println("Begin");

	loop_count = 0;
}

unsigned test_func(const char *name, unsigned (*pfunc)(int v), int verbose)
{
	Serial.print(name);
	Serial.print(" ...");
	uint32_t delay_ms = 125;
	delay(delay_ms);
	unsigned fail = pfunc(verbose);
	Serial.println(fail ? " FAIL!" : " ok.");
	delay(delay_ms);
	return fail;
}

#define Test_func(pfunc, verbose) test_func(#pfunc, pfunc, verbose)

void loop(void)
{
	int verbose = 0;
	Serial.println("=================================================");
	++loop_count;
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	unsigned failures = 0;

	failures += Test_func(test_add, verbose);
	failures += Test_func(test_bytes_shift_left, verbose);
	failures += Test_func(test_bytes_shift_right, verbose);
	failures += Test_func(test_compare2, verbose);
	failures += Test_func(test_compare3, verbose);
	failures += Test_func(test_compare, verbose);
	failures += Test_func(test_dec_corner_case, verbose);
	failures += Test_func(test_dec, verbose);
	failures += Test_func(test_div, verbose);
	failures += Test_func(test_equals, verbose);
	failures += Test_func(test_exp_mod, verbose);
	failures += Test_func(test_exp, verbose);
	failures += Test_func(test_from_binstr_to_binstr_round_trip, verbose);
	failures += Test_func(test_from_decimal_to_decimal_round_trip, verbose);
	failures += Test_func(test_from_hex_to_hex_round_trip, verbose);
	failures += Test_func(test_inc_l, verbose);
	failures += Test_func(test_inc, verbose);
	// failures += Test_func(test_is_probably_prime, verbose);
	failures += Test_func(test_mul, verbose);
	failures += Test_func(test_n_choose_k, verbose);
	failures += Test_func(test_scenario_mul_mod, verbose);
	failures += Test_func(test_set_l, verbose);
	failures += Test_func(test_set, verbose);
	failures += Test_func(test_shift_right, verbose);
	Serial.println("test_sqrt takes a long time");
	failures += Test_func(test_sqrt, verbose);
	failures += Test_func(test_subtract, verbose);
	failures += Test_func(test_to_string, verbose);

	Serial.println("=================================================");
	if (failures) {
		Serial.print("failures = ");
		Serial.println(failures);
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	uint32_t delay_ms = 30 * 1000;
	delay(delay_ms);
}
