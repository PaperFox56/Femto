CC=gcc
CFLAGS= -Wall -Wextra -pedantic --std=c99
TARGET=bin/femto
SOURCES=src/*.c src/*/*.c src/*/*/*.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc $(SOURCES) -o $@ $(CFLAGS) -g

run:
	./$(TARGET)

clean: $(TARGET)
	rm $(TARGET)