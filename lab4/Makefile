CC := gcc
CFLAGS := -std=c11 -O2 -Wall -Wextra -pthread
BUILD_DIR := build
SRC_DIR := src
TARGET := $(BUILD_DIR)/lab4_demo

.DEFAULT_GOAL := all

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(TARGET)

$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean