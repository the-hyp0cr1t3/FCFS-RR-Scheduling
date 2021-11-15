CC=gcc

CFLAGS=-O3 -w
LIBS=-pthread

BIN=main

SRCDIR=src
BINDIR=bin
LIBDIR=lib

DIRS=$(BINDIR) $(LIBDIR)
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.c)))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))

build: $(DIRS) clean all

all: $(OBJECTS)
	$(CC) -o $(BINDIR)/main $+ $(CFLAGS) $(LIBS)

$(LIBDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)


clean: 
	rm -rf $(LIBDIR)/* $(BINDIR)/*

$(DIRS):
	mkdir $@
	