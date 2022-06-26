all: test demo

CC=gcc

LIBS=
CFLAGS=-Os -pipe -ansi
DEBUGCFLAGS=-Og -pipe -g -Wall -ansi -pedantic

INPUT=test.c
OUTPUT=test

RM=/bin/rm

test: test.c vt100utils.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

demo: demos/overflow.c demos/hover.c demos/words.c vt100utils.h
	$(CC) demos/overflow.c -o demos/overflow $(LIBS) $(CFLAGS)
	$(CC) demos/hover.c -o demos/hover $(LIBS) $(CFLAGS)
	$(CC) demos/words.c -o demos/words $(LIBS) $(CFLAGS)
	$(CC) demos/truecolor_stresstest.c -o demos/truecolor_stresstest $(LIBS) -lm $(CFLAGS)

debug: test.c demos/overflow.c demos/hover.c demos/words.c vt100utils.h
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/overflow.c -o demos/overflow $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/hover.c -o demos/hover $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/words.c -o demos/words $(LIBS) $(DEBUGCFLAGS)
	$(CC) demos/truecolor_stresstest.c -o demos/truecolor_stresstest $(LIBS) -lm $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
	if [ -e demos/overflow ]; then $(RM) demos/overflow; fi
	if [ -e demos/hover ]; then $(RM) demos/hover; fi
	if [ -e demos/words ]; then $(RM) demos/words; fi
	if [ -e demos/truecolor_stresstest ]; then $(RM) demos/truecolor_stresstest; fi
