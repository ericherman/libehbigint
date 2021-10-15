/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "eembed-arduino.h"
#include "ehbigint.h"

// having ALL of these tests in a single .ino file bloats the firmware
// a lot, consider breaking into separate .ino files.
unsigned test_is_probably_prime(int verbose);
unsigned test_n_choose_k(int verbose);
unsigned test_scenario_mul_mod(int verbose);
unsigned test_sqrt(int verbose);

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
	int verbose = (loop_count % 2);

	Serial.println("=================================================");
	++loop_count;
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	unsigned failures = 0;

	Serial.println();
	failures += Test_func(test_scenario_mul_mod, verbose);

	Serial.println();
	failures += Test_func(test_n_choose_k, verbose);

	Serial.println();
	Serial.println("test_sqrt is a big test,");
	Serial.println("it may take 1 minute to run:");
	failures += Test_func(test_sqrt, verbose);

	Serial.println();
	Serial.println("test_is_probably_prime is a VERY big test,");
	Serial.println("it may take 10 minutes to run:");
	failures += Test_func(test_is_probably_prime, verbose);

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
