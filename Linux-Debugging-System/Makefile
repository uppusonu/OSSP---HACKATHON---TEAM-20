CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
TARGET = build/linux_debugging_system
SOURCES = src/main.c src/leak_tracker.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES) src/leak_tracker.h
	mkdir -p build reports
	$(CC) $(CFLAGS) -Isrc $(SOURCES) -o $(TARGET)

clean:
	rm -rf build

test: $(TARGET)
	bash tests/run_tests.sh
