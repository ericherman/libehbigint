#Makefile.am
#Copyright (C) 2016, 2018 Eric Herman <eric@freesa.org>
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

# for environments that do not have alloca, malloc is used
# malloc instead of alloca can be forced: #define EHBI_NO_ALLOCA 1
# TODO: make this plugable for non-alloca, non-C11 environments
#ALLOCA_CFLAGS=-DEHBI_NO_ALLOCA=1
ALLOCA_CFLAGS=

# typical builds:
RELEASE_HOSTED_CFLAGS=\
 -std=c89 -Wpedantic $(ALLOCA_CFLAGS) \
 -pipe -ggdb -O2 -fomit-frame-pointer \
 -Wall -Wextra -Werror -DNDEBUG=1 \
 -DEHBI_SKIP_STRUCT_NULL_CHECK=1

# if debugging, these flags may be better:
#TODO get configure/autoconf to work right with BACKTRACE
#if HAVE_BACKTRACE
#if HAVE_BACKTRACE_SYMBOLS_FD
BACKTRACE_CFLAGS=-D_POSIX_SOURCE -DEBHI_CAN_BACKTRACE -rdynamic
#endif
#else
#BACKTRACE_CFLAGS=
#endif
DEBUG_HOSTED_GNU_CFLAGS=\
 -std=gnu11 -D_GNU_SOURCE $(BACKTRACE_CFLAGS) $(ALLOCA_CFLAGS) \
 -pipe -ggdb -O0 \
 -Wall -Wextra -Werror

#BUILD_TYPE_CFLAGS=$(DEBUG_HOSTED_GNU_CFLAGS)
BUILD_TYPE_CFLAGS=$(RELEASE_HOSTED_CFLAGS)


EBA_CLFAGS=\
 -DEBA_SKIP_EBA_NEW=1 \
 -DEBA_SKIP_STRUCT_NULL_CHECK=1 \
 -DEBA_SKIP_STRUCT_BITS_NULL_CHECK=1 \
 -DEBA_SKIP_ARRAY_INDEX_OVERRUN_SAFETY=1 \
 -include src/ehbigint-eba.h

AM_CFLAGS=$(BUILD_TYPE_CFLAGS) $(EBA_CLFAGS)

libehbigint_la_LIBADD=
AM_LDFLAGS=-rdynamic

lib_LTLIBRARIES=libehbigint.la
include_HEADERS=src/ehbigint.h

libehbigint_la_SOURCES=\
 src/ehbigint.h \
 src/ehbigint-log.h \
 src/ehbigint-log.c \
 src/ehbigint-priv.h \
 src/ehbigint-priv.c \
 src/ehbigint-str.h \
 src/ehbigint-str.c \
 src/ehstr.h \
 src/ehstr.c \
 src/ehbigint-util.h \
 src/ehbigint-util.c \
 src/ehbigint-eba.h \
 src/ehbigint-eba.c \
 src/eba.h \
 src/eba.c \
 src/ehbigint.c


TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-add \
 test-compare \
 test-comp-exp-mod-with-gmp \
 test-comp-mul-with-gmp \
 test-compare2 \
 test-compare3 \
 test-dec \
 test-dec-corner-case \
 test-div \
 test-sqrt \
 test-exp \
 test-exp-mod \
 test-equals \
 test-from-binstr-to-binstr-round-trip \
 test-from-decimal-to-decimal-round-trip \
 test-from-hex-to-hex-round-trip \
 test-inc \
 test-inc-l \
 test-is-probably-prime \
 test-mul \
 test-n-choose-k \
 test-scenario-mul-mod \
 test-set \
 test-set-l \
 test-shift-right \
 test-subtract \
 test-bytes-shift-left \
 test-bytes-shift-right \
 test-to-string

#XFAIL_TESTS=test-is-probably-prime

COMMON_TEST_SOURCES=src/ehbigint.h \
 tests/echeck.h \
 tests/echeck.c \
 tests/test-ehbigint-private-utils.h \
 tests/test-ehbigint-private-utils.c

TEST_LDADDS=-lehbigint

test_add_SOURCES=tests/test-add.c $(COMMON_TEST_SOURCES)
test_add_LDADD=$(TEST_LDADDS)

test_compare_SOURCES=tests/test-compare.c $(COMMON_TEST_SOURCES)
test_compare_LDADD=$(TEST_LDADDS)

test_comp_exp_mod_with_gmp_SOURCES=tests/test-comp-exp-mod-with-gmp.c \
 $(COMMON_TEST_SOURCES)
test_comp_exp_mod_with_gmp_LDADD=$(TEST_LDADDS) -lgmp

test_comp_mul_with_gmp_SOURCES=tests/test-comp-mul-with-gmp.c \
 $(COMMON_TEST_SOURCES)
test_comp_mul_with_gmp_LDADD=$(TEST_LDADDS) -lgmp

test_compare2_SOURCES=tests/test-compare2.c $(COMMON_TEST_SOURCES)
test_compare2_LDADD=$(TEST_LDADDS)

test_compare3_SOURCES=tests/test-compare3.c $(COMMON_TEST_SOURCES)
test_compare3_LDADD=$(TEST_LDADDS)

test_dec_SOURCES=tests/test-dec.c $(COMMON_TEST_SOURCES)
test_dec_LDADD=$(TEST_LDADDS)

test_dec_corner_case_SOURCES=tests/test-dec-corner-case.c \
 $(COMMON_TEST_SOURCES)
test_dec_corner_case_LDADD=$(TEST_LDADDS)

test_div_SOURCES=tests/test-div.c $(COMMON_TEST_SOURCES)
test_div_LDADD=$(TEST_LDADDS)

test_sqrt_SOURCES=tests/test-sqrt.c $(COMMON_TEST_SOURCES)
test_sqrt_LDADD=$(TEST_LDADDS)

test_exp_SOURCES=tests/test-exp.c $(COMMON_TEST_SOURCES)
test_exp_LDADD=$(TEST_LDADDS)

test_exp_mod_SOURCES=tests/test-exp-mod.c $(COMMON_TEST_SOURCES)
test_exp_mod_LDADD=$(TEST_LDADDS)

test_equals_SOURCES=tests/test-equals.c $(COMMON_TEST_SOURCES)
test_equals_LDADD=$(TEST_LDADDS)

test_from_binstr_to_binstr_round_trip_SOURCES=\
 tests/test-from-binstr-to-binstr-round-trip.c $(COMMON_TEST_SOURCES)
test_from_binstr_to_binstr_round_trip_LDADD=$(TEST_LDADDS)

test_from_decimal_to_decimal_round_trip_SOURCES=\
 tests/test-from-decimal-to-decimal-round-trip.c $(COMMON_TEST_SOURCES)
test_from_decimal_to_decimal_round_trip_LDADD=$(TEST_LDADDS)

test_from_hex_to_hex_round_trip_SOURCES=\
 tests/test-from-hex-to-hex-round-trip.c $(COMMON_TEST_SOURCES)
test_from_hex_to_hex_round_trip_LDADD=$(TEST_LDADDS)

test_inc_SOURCES=tests/test-inc.c $(COMMON_TEST_SOURCES)
test_inc_LDADD=$(TEST_LDADDS)

test_inc_l_SOURCES=tests/test-inc-l.c $(COMMON_TEST_SOURCES)
test_inc_l_LDADD=$(TEST_LDADDS)

test_is_probably_prime_SOURCES=tests/test-is-probably-prime.c \
 $(COMMON_TEST_SOURCES)
test_is_probably_prime_LDADD=$(TEST_LDADDS)

test_mul_SOURCES=tests/test-mul.c $(COMMON_TEST_SOURCES)
test_mul_LDADD=$(TEST_LDADDS)

test_n_choose_k_SOURCES=tests/test-n-choose-k.c $(COMMON_TEST_SOURCES)
test_n_choose_k_LDADD=$(TEST_LDADDS)

test_scenario_mul_mod_SOURCES=tests/test-scenario-mul-mod.c \
 $(COMMON_TEST_SOURCES)
test_scenario_mul_mod_LDADD=$(TEST_LDADDS)

test_set_SOURCES=tests/test-set.c $(COMMON_TEST_SOURCES)
test_set_LDADD=$(TEST_LDADDS)

test_set_l_SOURCES=tests/test-set-l.c $(COMMON_TEST_SOURCES)
test_set_l_LDADD=$(TEST_LDADDS)

test_shift_right_SOURCES=tests/test-shift-right.c \
 $(COMMON_TEST_SOURCES)
test_shift_right_LDADD=$(TEST_LDADDS)

test_subtract_SOURCES=tests/test-subtract.c $(COMMON_TEST_SOURCES)
test_subtract_LDADD=$(TEST_LDADDS)

test_bytes_shift_left_SOURCES=tests/test-bytes-shift-left.c \
 $(COMMON_TEST_SOURCES)
test_bytes_shift_left_LDADD=$(TEST_LDADDS)

test_bytes_shift_right_SOURCES=tests/test-bytes-shift-right.c \
 $(COMMON_TEST_SOURCES)
test_bytes_shift_right_LDADD=$(TEST_LDADDS)

test_to_string_SOURCES=tests/test-to-string.c $(COMMON_TEST_SOURCES)
test_to_string_LDADD=$(TEST_LDADDS)

#bin_PROGRAMS=bi-calc
#bi_calc_SOURCES=demos/bi-calc.c src/ehbigint.h
#bi_calc_LDADD=-lehbigint

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.LESSER misc



# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	patch -Np1 -i misc/pre-tidy.patch
	$(LINDENT) \
		-T size_t \
		-T FILE \
		-T ehbigint \
		`find src tests demos -name '*.h' -o -name '*.c'`
	patch -Rp1 -i misc/pre-tidy.patch

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd
	pushd demos && rm -rf `cat ../.gitignore | sed -e 's/#.*//'`; popd

bi-calc:
	$(CC) $(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS) \
		-o bi-calc $(libehbigint_la_SOURCES) \
		demos/bi-calc.c src/ehbigint.h

demo: bi-calc
	./bi-calc 132904811234120000312412 + 123412413132500

valgrind: $(check_PROGRAMS) $(lib_LTLIBRARIES)
	./libtool --mode=execute valgrind -q ./test-add
	./libtool --mode=execute valgrind -q ./test-compare
	./libtool --mode=execute valgrind -q ./test-comp-exp-mod-with-gmp
	./libtool --mode=execute valgrind -q ./test-comp-mul-with-gmp
	./libtool --mode=execute valgrind -q ./test-compare2
	./libtool --mode=execute valgrind -q ./test-compare3
	./libtool --mode=execute valgrind -q ./test-dec
	./libtool --mode=execute valgrind -q ./test-dec-corner-case
	./libtool --mode=execute valgrind -q ./test-div
	./libtool --mode=execute valgrind -q ./test-sqrt
	./libtool --mode=execute valgrind -q ./test-exp
	./libtool --mode=execute valgrind -q ./test-exp-mod
	./libtool --mode=execute valgrind -q ./test-equals
	./libtool --mode=execute valgrind -q \
		./test-from-binstr-to-binstr-round-trip
	./libtool --mode=execute valgrind -q \
		./test-from-decimal-to-decimal-round-trip
	./libtool --mode=execute valgrind -q ./test-from-hex-to-hex-round-trip
	./libtool --mode=execute valgrind -q ./test-inc
	./libtool --mode=execute valgrind -q ./test-inc-l
	./libtool --mode=execute valgrind -q ./test-is-probably-prime
	./libtool --mode=execute valgrind -q ./test-n-choose-k
	./libtool --mode=execute valgrind -q ./test-mul
	./libtool --mode=execute valgrind -q ./test-scenario-mul-mod
	./libtool --mode=execute valgrind -q ./test-set
	./libtool --mode=execute valgrind -q ./test-set-l
	./libtool --mode=execute valgrind -q ./test-shift-right
	./libtool --mode=execute valgrind -q ./test-subtract
	./libtool --mode=execute valgrind -q ./test-bytes-shift-left
	./libtool --mode=execute valgrind -q ./test-bytes-shift-right
	./libtool --mode=execute valgrind -q ./test-to-string
