# C-compiler
CC=g++

CFLAGS=-Wall

all: stableMatch.out

stableMatch.out: stableMatch.cpp
	$(CC) $(CFLAGS) -o $@ $< 

clean:
	rm *.out