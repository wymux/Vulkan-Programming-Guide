CC = gcc
LDFLAGS = -lvulkan

SRC = 01-Introduction/main.c

all: format 01-Introduction/main

01-Introduction/main: $(SRC)
	$(CC) $(CFLAGS) -o $@ 01-Introduction/main.c $(LDFLAGS)

format: $(SRC)
	clang-format 01-Introduction/main.c -i
