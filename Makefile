CC      := cc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -g -Iinclude
LDFLAGS := -lncurses
BUILD   := build
TARGET  := $(BUILD)/terminal_tetris
SRC     := $(wildcard src/*.c)
OBJ     := $(patsubst src/%.c,$(BUILD)/%.o,$(SRC))

$(TARGET): $(BUILD) $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

.PHONY: clean run

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf $(BUILD)
