LIB_NAME=ehbigint

AUX_INCLUDES=-I ../libecheck
AUX_A_FILES=../libecheck/libecheck.a
AUX_LDFLAGS=-L../libecheck -lecheck
AUX_LD_LIBRARY_PATHS=:../libecheck


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

INCLUDES=-I.
TEST_OUT=test-$(LIB_NAME)
TEST_SRC=$(TEST_OUT).c
TEST_OBJ=$(TEST_OUT).o

OUT=bi-calc
SRC=$(OUT).c
OBJ=$(OUT).o

CSTD_CFLAGS=-std=c89 -DEHBI_ENSURE_POSIX=1
#CSTD_CFLAGS=-std=c11

ifeq ("$(DEBUG)", "")
VERBOSE_ANNOUNCE=0
DEBUG_CFLAGS=-ggdb -O3
else
VERBOSE_ANNOUNCE=1
DEBUG_CFLAGS=-ggdb -O0
endif

NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

CFLAGS += $(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS)
LDFLAGS += -L. -l$(LIB_NAME) -rdynamic
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

LD_LIBRARY_PATH=.$(AUX_LD_LIBRARY_PATHS)

#default: $(LIB_NAME)
default: $(OUT)

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

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
	$(CC) -c $(INCLUDES) $(AUX_INCLUDES) $(CFLAGS) \
		$(TEST_SRC) -o $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(A_NAME) $(AUX_A_FILES) -o $(TEST_OUT)-static
	$(CC) $(TEST_OBJ) $(LDFLAGS) $(AUX_LDFLAGS) -o $(TEST_OUT)-dynamic

$(OUT): $(LIB_NAME)
	$(CC) -c $(INCLUDES) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CC) $(OBJ) -o $(OUT)

demo: $(OUT)
	./$(OUT) 132904811234120000312412 + 123412413132500

check: $(TEST_OUT)
	./$(TEST_OUT)-static $(VERBOSE_ANNOUNCE)
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
