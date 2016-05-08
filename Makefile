LIB_NAME=ehbigint

EHSTR_INCLUDES=-I ../libehstr
EHSTR_A_FILES=../libehstr/libehstr.a
EHSTR_LDFLAGS=-L../libehstr
EHSTR_LDADD=-lehstr
EHSTR_LD_LIBRARY_PATHS=:../libehstr

ECHECK_INCLUDES=-I ../libecheck
ECHECK_A_FILES=../libecheck/libecheck.a
ECHECK_LDFLAGS=-L../libecheck
ECHECK_LDADD=-lecheck
ECHECK_LD_LIBRARY_PATHS=:../libecheck

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

LIB_SRC=$(LIB_NAME).c
LIB_HDR=$(LIB_NAME).h
LIB_OBJ=$(LIB_SRC:.c=.o)
SO_OBJS=$(LIB_NAME).o
SO_NAME=lib$(LIB_NAME).$(SHAREDEXT)
ifneq ($(UNAME), Darwin)
    SHAREDFLAGS += -Wl,-soname,$(SO_NAME)
endif

A_NAME=lib$(LIB_NAME).a

INCLUDES=-I. $(EHSTR_INCLUDES)
A_FILES=$(EHSTR_A_FILES)

TEST_OUT=test-$(LIB_NAME)
TEST_SRC=$(TEST_OUT).c
TEST_OBJ=$(TEST_OUT).o

OUT=bi-calc
SRC=$(OUT).c
OBJ=$(OUT).o

CSTD_CFLAGS=-std=c89 -DEHBI_ENSURE_POSIX=1
#CSTD_CFLAGS=-std=c11

ifeq ("$(NOALLOCA)", "")
ALLOCA_FLAGS=
else
ALLOCA_FLAGS=-DEHBI_NO_ALLOCA=1
endif

ifeq ("$(DEBUG)", "")
VERBOSE_ANNOUNCE=0
DEBUG_CFLAGS=-ggdb -O3
else
VERBOSE_ANNOUNCE=1
DEBUG_CFLAGS=-ggdb -O0
endif

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

CFLAGS += $(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS) $(ALLOCA_FLAGS)
LDFLAGS += -L. $(EHSTR_LDFLAGS) $(ECHECK_LDFLAGS) -rdynamic
LDADD += -l$(LIB_NAME) $(EHSTR_LDADD) $(ECHECK_LDADD) -lgmp

CC=gcc

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

ifeq ("$(PREFIX)", "")
PREFIX=/usr/local
endif

ifeq ("$(LIBDIR)", "")
LIBDIR=$(PREFIX)/lib
endif


ifeq ("$(INCDIR)", "")
INCDIR=$(PREFIX)/include
endif

ifneq ($(strip $(srcdir)),)
   VPATH::=$(srcdir)
endif

LD_LIBRARY_PATH=.$(EHSTR_LD_LIBRARY_PATHS)$(ECHECK_LD_LIBRARY_PATHS)

#default: $(LIB_NAME)
default: $(OUT)

.c.o:
	$(CC) -c $(INCLUDES) -fPIC $(CFLAGS) $< -o $@

all: $(OUT) $(TEST_OUT) demo

$(SO_NAME): $(LIB_OBJ)
	$(CC) $(SHAREDFLAGS) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(A_NAME): $(LIB_OBJ)
	ar -r $(A_NAME) $(SO_OBJS)

$(LIB_NAME): $(SO_NAME) $(A_NAME)
	@echo $(UNAME) $(LIB_NAME) library files:
	@ls -1 $(SO_NAME)* *.a

$(TEST_OUT): $(LIB_NAME)
	$(CC) -c $(INCLUDES) $(ECHECK_INCLUDES) $(CFLAGS) \
		$(TEST_SRC) -o $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(A_NAME) $(A_FILES) $(ECHECK_A_FILES) \
		-o $(TEST_OUT)-static -lgmp
	$(CC) $(TEST_OBJ) $(LDFLAGS) -o $(TEST_OUT)-dynamic $(LDADD)

$(OUT): $(LIB_NAME)
	$(CC) -c $(INCLUDES) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CC) $(OBJ) $(A_FILES) -o $(OUT)

demo: $(OUT)
	./$(OUT) 132904811234120000312412 + 123412413132500

check: $(TEST_OUT)
	./$(TEST_OUT)-static $(VERBOSE_ANNOUNCE) $(SLOW_TESTS)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./$(TEST_OUT)-dynamic
	@echo "Success."

tidy:
	$(LINDENT) \
		-T size_t \
		-T FILE \
		*.h *.c

clean:
	rm -f *~ *.o *.a *.$(SHAREDEXT) \
		$(SO_NAME).* \
		$(TEST_OUT)-static $(TEST_OUT)-dynamic \
		bi-calc


install: $(LIB_NAME)
	@echo "Installing $(LIB_NAME) $(UNAME) libraries in $(LIBDIR)"
	mkdir -pv $(LIBDIR)
	cp -pv $(A_NAME) $(LIBDIR)/
	cp -Rv $(SO_NAME)* $(LIBDIR)/
	@echo "Installing $(LIB_NAME) headers in $(INCDIR)"
	mkdir -pv $(INCDIR)
	cp -pv $(LIB_HDR) $(INCDIR)/
