all: synthiumc

CC=clang
CFLAGS = -Wall -Wextra -pedantic

OBJS = $(patsubst %.c, %.o, $(wildcard src/*.c))

.PHONY: synthiumc

synthiumc: $(OBJS)
    $(CC) $(CFLAGS) -o $@ $^

HEADERS = $(wildcard include/*.h)
$(OBJS): $(HEADERS)