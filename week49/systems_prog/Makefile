CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2 -g
LDFLAGS := 
INCLUDES := -Isrc -Iinclude
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
BIN := build/sysprog

all: $(BIN)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

build:
	mkdir -p build

test:
	@echo "No tests yet"

clean:
	rm -rf build

.PHONY: all clean test
