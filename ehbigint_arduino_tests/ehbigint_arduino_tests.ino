/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "eembed-arduino.h"
#include "ehbigint.h"

// having ALL of these tests in a single .ino file bloats the firmware
// a lot, thus split into a couple of .ino files.
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
unsigned test_mul(int verbose);
unsigned test_set(int verbose);
unsigned test_set_l(int verbose);
unsigned test_shift_right(int verbose);
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
	// print the name delay so that it flushes before running test
	// the delay helps us see which test crashed and caused a restart
	Serial.print(name);
	Serial.print(" ...");
	uint32_t delay_ms = 25;
	delay(delay_ms);

	unsigned long usec_begin = micros();

	unsigned fail = pfunc(verbose);

	unsigned long usec_end = micros();
	unsigned long usec_elapsed = usec_end - usec_begin;
	double seconds_elapsed = (usec_elapsed * 1.0) / (1000.0 * 1000.0);
	Serial.print(" (");
	Serial.print(seconds_elapsed, 6);
	Serial.print(" seconds)");

	Serial.println(fail ? " FAIL!" : " ok.");
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
	failures += Test_func(test_mul, verbose);
	failures += Test_func(test_set_l, verbose);
	failures += Test_func(test_set, verbose);
	failures += Test_func(test_shift_right, verbose);
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
