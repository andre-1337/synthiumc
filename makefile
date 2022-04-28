all: synthiumc

CFLAGS=-Wall -Wextra -pedantic -std=c++17

.PHONY: synthiumc

synthiumc: $(OBJS)
	clang++ $(CFLAGS) src/main.cpp -o synthiumc.exe

clean:
	rm -rf src/*.o
	rm -rf synthiumc.exe