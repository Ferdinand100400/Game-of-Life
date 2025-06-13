CC=gcc
CFLAGS=-Werror -Wextra -Wall -std=c11
SOURCES=src/GameOfLife.c
LIBRARY=ncurses
EXUTABLE=build/

all:
	$(CC) $(CFLAGS) $(SOURCES) -l$(LIBRARY) -o $(EXUTABLE)/GameOfLife.exe

clean:
	rm -rf $(EXUTABLE)/*

rebuild: clean all