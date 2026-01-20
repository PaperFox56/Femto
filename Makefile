CC=gcc
PROJECT=femto

SOURCES=src/main.c

$(PROJECT): $(SOURCES)
	gcc $(SOURCES) -o $(PROJECT) -Wall -Wextra -pedantic --std=c99

run:
	./$(PROJECT)