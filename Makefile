CC=gcc
CFLAGS= -Wall -Wextra -pedantic --std=c99
TARGET=femto
SOURCES=src/*.c src/*/*.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc $(SOURCES) -o $@ $(CFLAGS)

run:
	./$(TARGET)

clean: $(TARGET)
	rm $(TARGET)