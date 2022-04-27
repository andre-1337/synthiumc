all: synthiumc

CC=gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu11

OBJS = $(patsubst %.c, %.o, $(wildcard src/*.c))

.PHONY: synthiumc

synthiumc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

HEADERS = $(wildcard include/*.h)
$(OBJS): $(HEADERS)

clean:
	rm -rf src/*.o
	rm -rf synthiumc