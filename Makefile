CC = gcc
LDFLAGS = -lvulkan

all: 01-Introduction/main

01-Introduction/main: 01-Introduction/main.c
	$(CC) $(CFLAGS) -o $@ 01-Introduction/main.c $(LDFLAGS)
