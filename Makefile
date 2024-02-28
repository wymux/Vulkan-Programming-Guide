CC = gcc
LDFLAGS = -lvulkan

SRC = 01-Introduction/main.c 02-Memory-and-Resources/main.c

all: format 01-Introduction/main

01-Introduction/main: $(SRC)
	$(CC) $(CFLAGS) -o $@ 01-Introduction/main.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o 02-Memory-and-Resources/main 02-Memory-and-Resources/main.c $(LDFLAGS)

format: $(SRC)
	clang-format 01-Introduction/main.c -i
	clang-format 02-Memory-and-Resources/main.c -i
