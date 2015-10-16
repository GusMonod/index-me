ZIPF_SRC=$(wildcard zipf/*.c)
CONS_SRC=$(wildcard construct/*.c)

ZIPF_OBJECTS=$(ZIPF_SRC:.c=.o)
CONS_OBJECTS=$(CONS_SRC:.c=.o)

OBJECTS=$ZIPF_OBJECTS $CONS_OBJECTS

CC=gcc
CC_FLAGS=-std=c11 -pedantic -Wall -W -Wextra
TARGET ?= debug

ifeq ($(TARGET), release)
		CC_FLAGS +=-DDEBUG
else
		CC_FLAGS +=-g -DDEBUG
endif

LD=gcc

LIBS=
LIB_PATH=-L/usr/local/lib

INC=
INC_PATH=-I$(shell pwd)

.PHONY: clean

bin/construct: $(CONS_OBJECTS)
	mkdir -p bin
	$(LD) $(LD_FLAGS) $(INC_PATH) $(LIB_PATH) $< $(LIBS) -o $@

bin/zipf: $(ZIPF_OBJECTS)
	mkdir -p bin
	$(LD) $(LD_FLAGS) $(INC_PATH) $(LIB_PATH) $< $(LIBS) -o $@

#$(PROG): $(OBJECTS)
#	mkdir -p bin
#	$(LD) $(LD_FLAGS) $(INC_PATH) $(LIB_PATH) $(OBJECTS) $(LIBS) -o $(PROG)

%.o: %.c
	@echo Compiling $<...
	$(CC) $(CC_FLAGS) $(INC_PATH) -c $< -o $@
	@echo Compiled $< successfully

clean:
	rm -f $(OBJECTS) bin/* core
