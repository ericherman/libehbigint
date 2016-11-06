#Makefile.am
#Copyright (C) 2016 Eric Herman <eric@freesa.org>
#
#This work is free software: you can redistribute it and/or modify it
#under the terms of the GNU Lesser General Public License as published by
#the Free Software Foundation, either version 3 of the License, or (at
#your option) any later version.
#
#This work is distributed in the hope that it will be useful, but WITHOUT
#ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#License for more details.

lib_LTLIBRARIES=libehbigint.la
include_HEADERS=src/ehbigint.h

libehbigint_la_SOURCES=\
 src/ehbigint.h \
 src/ehbigint-log.h \
 src/ehbigint-util.h \
 src/ehbigint-log.c \
 src/ehbigint-util.c \
 src/ehbigint.c
libehbigint_la_LIBADD=-lehstr
AM_LDFLAGS=-rdynamic


TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-add \
 test-compare \
 test-compare-with-gmp \
 test-compare2 \
 test-dec \
 test-dec-corner-case \
 test-div \
 test-equals \
 test-from-decimal-to-decimal-round-trip \
 test-from-hex-to-hex-round-trip \
 test-inc \
 test-inc-ul \
 test-mul \
 test-scenario-mul-mod \
 test-set \
 test-set-ul \
 test-subtract \
 test-shift-left \
 test-to-string

COMMON_TEST_SOURCES=src/ehbigint.h \
 tests/test-ehbigint-private-utils.h \
 tests/test-ehbigint-private-utils.c

TEST_LDADDS=-lehstr -lecheck -lehbigint

test_add_SOURCES=tests/test-add.c $(COMMON_TEST_SOURCES)
test_add_LDADD=$(TEST_LDADDS)

test_compare_SOURCES=tests/test-compare.c $(COMMON_TEST_SOURCES)
test_compare_LDADD=$(TEST_LDADDS)

test_compare_with_gmp_SOURCES=tests/test-compare-with-gmp.c \
 $(COMMON_TEST_SOURCES)
test_compare_with_gmp_LDADD=$(TEST_LDADDS) -lgmp

test_compare2_SOURCES=tests/test-compare2.c $(COMMON_TEST_SOURCES)
test_compare2_LDADD=$(TEST_LDADDS)

test_dec_SOURCES=tests/test-dec.c $(COMMON_TEST_SOURCES)
test_dec_LDADD=$(TEST_LDADDS)

test_dec_corner_case_SOURCES=tests/test-dec-corner-case.c \
 $(COMMON_TEST_SOURCES)
test_dec_corner_case_LDADD=$(TEST_LDADDS)

test_div_SOURCES=tests/test-div.c $(COMMON_TEST_SOURCES)
test_div_LDADD=$(TEST_LDADDS)

test_equals_SOURCES=tests/test-equals.c $(COMMON_TEST_SOURCES)
test_equals_LDADD=$(TEST_LDADDS)

test_from_decimal_to_decimal_round_trip_SOURCES=\
 tests/test-from-decimal-to-decimal-round-trip.c $(COMMON_TEST_SOURCES)
test_from_decimal_to_decimal_round_trip_LDADD=$(TEST_LDADDS)

test_from_hex_to_hex_round_trip_SOURCES=\
 tests/test-from-hex-to-hex-round-trip.c $(COMMON_TEST_SOURCES)
test_from_hex_to_hex_round_trip_LDADD=$(TEST_LDADDS)

test_inc_SOURCES=tests/test-inc.c $(COMMON_TEST_SOURCES)
test_inc_LDADD=$(TEST_LDADDS)

test_inc_ul_SOURCES=tests/test-inc-ul.c $(COMMON_TEST_SOURCES)
test_inc_ul_LDADD=$(TEST_LDADDS)

test_mul_SOURCES=tests/test-mul.c $(COMMON_TEST_SOURCES)
test_mul_LDADD=$(TEST_LDADDS)

test_scenario_mul_mod_SOURCES=tests/test-scenario-mul-mod.c \
 $(COMMON_TEST_SOURCES)
test_scenario_mul_mod_LDADD=$(TEST_LDADDS)

test_set_SOURCES=tests/test-set.c $(COMMON_TEST_SOURCES)
test_set_LDADD=$(TEST_LDADDS)

test_set_ul_SOURCES=tests/test-set-ul.c $(COMMON_TEST_SOURCES)
test_set_ul_LDADD=$(TEST_LDADDS)

test_subtract_SOURCES=tests/test-subtract.c $(COMMON_TEST_SOURCES)
test_subtract_LDADD=$(TEST_LDADDS)

test_shift_left_SOURCES=tests/test-shift-left.c $(COMMON_TEST_SOURCES)
test_shift_left_LDADD=$(TEST_LDADDS)

test_to_string_SOURCES=tests/test-to-string.c $(COMMON_TEST_SOURCES)
test_to_string_LDADD=$(TEST_LDADDS)

bin_PROGRAMS=bi-calc
bi_calc_SOURCES=demos/bi-calc.c src/ehbigint.h
bi_calc_LDADD=-lehbigint -lehstr

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.GPL3

CSTD_CFLAGS=-std=c89
#CSTD_CFLAGS=-std=gnu11

DEBUG_CFLAGS=-ggdb -O2
#DEBUG_CFLAGS=-ggdb -O0

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

ALLOCA_CFLAGS=
#ALLOCA_CFLAGS=-DEHBI_NO_ALLOCA=1

#TODO get configure/autoconf to work right here
#if HAVE_BACKTRACE
#if HAVE_BACKTRACE_SYMBOLS_FD
BACKTRACE_CFLAGS=-D_POSIX_SOURCE -DEBHI_CAN_BACKTRACE -rdynamic
#endif
#else
#BACKTRACE_CFLAGS=
#endif

VERBOSE_ANNOUNCE=0
#VERBOSE_ANNOUNCE=1

AM_CFLAGS=$(CSTD_CFLAGS) \
 $(DEBUG_CFLAGS) \
 $(NOISY_CFLAGS) \
 $(ALLOCA_CFLAGS) \
 $(BACKTRACE_CFLAGS)

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	$(LINDENT) \
		-T size_t \
		-T FILE \
		-T ehbigint \
		`find src tests demos -name '*.h' -o -name '*.c'`

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd demos && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd

demo: bi-calc
	libtool --mode=execute ./bi-calc \
		132904811234120000312412 + 123412413132500

valgrind: $(check_PROGRAMS)
	libtool --mode=execute valgrind -q ./test-add
	libtool --mode=execute valgrind -q ./test-compare
	libtool --mode=execute valgrind -q ./test-compare-with-gmp
	libtool --mode=execute valgrind -q ./test-compare2
	libtool --mode=execute valgrind -q ./test-dec
	libtool --mode=execute valgrind -q ./test-dec-corner-case
	libtool --mode=execute valgrind -q ./test-div
	libtool --mode=execute valgrind -q ./test-equals
	libtool --mode=execute valgrind -q \
		./test-from-decimal-to-decimal-round-trip
	libtool --mode=execute valgrind -q ./test-from-hex-to-hex-round-trip
	libtool --mode=execute valgrind -q ./test-inc
	libtool --mode=execute valgrind -q ./test-inc-ul
	libtool --mode=execute valgrind -q ./test-mul
	libtool --mode=execute valgrind -q ./test-scenario-mul-mod
	libtool --mode=execute valgrind -q ./test-set
	libtool --mode=execute valgrind -q ./test-set-ul
	libtool --mode=execute valgrind -q ./test-subtract
	libtool --mode=execute valgrind -q ./test-shift-left
	libtool --mode=execute valgrind -q ./test-to-string
