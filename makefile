#Generic GNUMakefile
# snippet to fail if not GNU
ifneq (,)
This makefile requires GNU Make.
endif

PROGRAM = femto
C_FILES := $(wildcard src/*.c)
C_FILES += $(wildcard src/*/*.c)
C_FILES += $(wildcard src/*/*/*.c)
INCLUDES := includes/
OBJS := $(patsubst src/%.c, bin/%.o, $(C_FILES))
CC = gcc
CFLAGS = -Wall -pedantic -Wextra --std=c99
#LDFLAGS =
#LDLIBS = -lm

all: build
	
build: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o bin/$(PROGRAM) $(LDLIBS)
	
run:
	bin/$(PROGRAM)
	
build_an_run: build run
	
# These are the pattern matching rules. In addition to the automatic<br># vari>
# can be useful in special cases.
bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
src/%: bin/%.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: ./bin/%.o

clean:

	rm -f .depend $(OBJS)
.PHONY: clean depend


