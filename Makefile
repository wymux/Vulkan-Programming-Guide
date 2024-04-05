CC = gcc
LDFLAGS = -lvulkan -lxcb -lglfw -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SRC = 01-Introduction/main.c 02-Memory-and-Resources/main.c 03-Queues-and-Commands/main.c 04-Moving-Data/main.c 05-Presentation/main.c triangle/triangle.c
OBJ = $(SRC:.c=.o)
BIN = $(SRC:.c=)

all: format $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

%: %.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

format: $(SRC)
	clang-format -i $^
	touch format 
