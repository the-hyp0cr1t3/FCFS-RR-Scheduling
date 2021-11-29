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

INPUT_FILES=c2.txt c3.txt

build: $(INPUT_FILES) clean $(DIRS) all

all: $(OBJECTS)
	$(CC) -o $(BINDIR)/main $+ $(CFLAGS) $(LIBS)

$(LIBDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(LIBDIR) $(BINDIR) *.shm stats.csv exec.log gen_txt

$(DIRS):
	mkdir $@

$(INPUT_FILES):
	gcc utils/gen_txt.c -o gen_txt
	./gen_txt