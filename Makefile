CC=gcc

CFLAGS=-Wall -O2
LIBS=-pthread

BIN=main

SRCDIR=src
BINDIR=bin
LIBDIR=lib

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.c)))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))

all: $(OBJECTS)
	$(CC) -o $(BINDIR)/main $+ $(CFLAGS) $(LIBS)


$(LIBDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)


clean:
	rm -rf $(LIBDIR)/* $(BINDIR)/*

rebuild: clean all