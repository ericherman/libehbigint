lib_LTLIBRARIES=libehbigint.la
libehbigint_la_SOURCES=src/ehbigint.c src/ehbigint.h
libehbigint_la_LIBADD=-lehstr

include_HEADERS=src/ehbigint.h

TESTS=$(check_PROGRAMS)
check_PROGRAMS=test-ehbigint
test_ehbigint_SOURCES=tests/test-ehbigint.c src/ehbigint.h
test_ehbigint_LDADD=-lehbigint -lehstr -lecheck -lgmp

bin_PROGRAMS=bi-calc
bi_calc_SOURCES=demos/bi-calc.c src/ehbigint.h
bi_calc_LDADD=-lehbigint -lehstr

ACLOCAL_AMFLAGS=-I m4 --install

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

valgrind: test-ehbigint
	libtool --mode=execute valgrind ./test-ehbigint
