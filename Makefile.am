lib_LTLIBRARIES=libehbigint.la
libehbigint_la_SOURCES=src/ehbigint.c src/ehbigint.h
libehbigint_la_LIBADD=-lehstr

include_HEADERS=src/ehbigint.h

TESTS=$(check_PROGRAMS)
check_PROGRAMS=test-ehbigint
test_ehbigint_SOURCES=tests/test-ehbigint.c src/ehbigint.h
test_ehbigint_LDADD=-lehbigint -lehstr -lecheck -lgmp

#bin_PROGRAMS=bi-calc
#bi_calc_SROUCES=demos/bi-calc.c src/ehbigint.h
#bi_calc_LDADD=-lehbigint -lehstr

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
	rm -rfv `cat .gitignore`
	( cd src; rm -rfv `cat ../.gitignore`; cd .. )
	( cd tests; rm -rfv `cat ../.gitignore`; cd .. )
	( cd demos; rm -rfv `cat ../.gitignore`; cd .. )

demo: bi-calc
	./bi-calc 132904811234120000312412 + 123412413132500
