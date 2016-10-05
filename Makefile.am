lib_LTLIBRARIES=libehbigint.la
libehbigint_la_SOURCES=src/ehbigint.c src/ehbigint.h
libehbigint_la_LIBADD=-lehstr

include_HEADERS=src/ehbigint.h

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

test_to_string_SOURCES=tests/test-to-string.c $(COMMON_TEST_SOURCES)
test_to_string_LDADD=$(TEST_LDADDS)

bin_PROGRAMS=bi-calc
bi_calc_SOURCES=demos/bi-calc.c src/ehbigint.h
bi_calc_LDADD=-lehbigint -lehstr

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.GPL3

CSTD_CFLAGS=-std=c89 -DEHBI_ENSURE_POSIX=1
#CSTD_CFLAGS=-std=c11

ALLOCA_FLAGS=
#ALLOCA_FLAGS=-DEHBI_NO_ALLOCA=1

VERBOSE_ANNOUNCE=0
#VERBOSE_ANNOUNCE=1
DEBUG_CFLAGS=-ggdb -O3
#DEBUG_CFLAGS=-ggdb -O0

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror
AM_CFLAGS=$(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS) $(ALLOCA_FLAGS)

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
	libtool --mode=execute valgrind ./test-add
	libtool --mode=execute valgrind ./test-compare
	libtool --mode=execute valgrind ./test-compare-with-gmp
	libtool --mode=execute valgrind ./test-compare2
	libtool --mode=execute valgrind ./test-dec
	libtool --mode=execute valgrind ./test-dec-corner-case
	libtool --mode=execute valgrind ./test-div
	libtool --mode=execute valgrind ./test-equals
	libtool --mode=execute valgrind \
		./test-from-decimal-to-decimal-round-trip
	libtool --mode=execute valgrind ./test-from-hex-to-hex-round-trip
	libtool --mode=execute valgrind ./test-inc
	libtool --mode=execute valgrind ./test-inc-ul
	libtool --mode=execute valgrind ./test-mul
	libtool --mode=execute valgrind ./test-scenario-mul-mod
	libtool --mode=execute valgrind ./test-set
	libtool --mode=execute valgrind ./test-set-ul
	libtool --mode=execute valgrind ./test-subtract
	libtool --mode=execute valgrind ./test-to-string
