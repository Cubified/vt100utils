all: test demo

CC=gcc

LIBS=
CFLAGS=-Os -pipe -s
DEBUGCFLAGS=-Og -pipe -g

INPUT=test.c
OUTPUT=test

RM=/bin/rm

test: test.c vt100utils.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

demo: demos/overflow.c demos/hover.c vt100utils.h
	$(CC) demos/overflow.c -o demos/overflow $(LIBS) $(CFLAGS)
	$(CC) demos/hover.c -o demos/hover $(LIBS) $(CFLAGS)

debug: test.c demos/overflow.c demos/hover.c vt100utils.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/overflow.c -o demos/overflow $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/hover.c -o demos/hover $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
	if [ -e demos/overflow ]; then $(RM) demos/overflow; fi
	if [ -e demos/hover ]; then $(RM) demos/hover; fi
