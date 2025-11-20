CC      := cc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -g -Iinclude
LDFLAGS := -lncurses
BUILD   := build
TARGET  := $(BUILD)/terminal_tetris
SRC     := $(wildcard src/*.c)
OBJ     := $(patsubst src/%.c,$(BUILD)/%.o,$(SRC))
CORE_OBJ := $(BUILD)/board.o $(BUILD)/piece.o $(BUILD)/score.o $(BUILD)/bag.o
TEST_SRC := $(wildcard tests/*.c)
TEST_BIN := $(patsubst tests/%.c,$(BUILD)/tests/%,$(TEST_SRC))

$(TARGET): $(BUILD) $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/tests:
	@mkdir -p $(BUILD)/tests

$(BUILD)/tests/%: tests/%.c $(CORE_OBJ) | $(BUILD)/tests
	$(CC) $(CFLAGS) $< $(CORE_OBJ) -o $@

.PHONY: clean run test

run: $(TARGET)
	$(TARGET)

test: $(TEST_BIN)
	@set -e; \
	for t in $(TEST_BIN); do \
		printf "Running %s\\n" "$$t"; \
		"$$t"; \
	done

clean:
	rm -rf $(BUILD)
