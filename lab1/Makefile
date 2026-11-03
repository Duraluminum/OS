CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

BUILD_DIR = build
SRC_DIR = src


all: $(BUILD_DIR)/parent $(BUILD_DIR)/child1 $(BUILD_DIR)/child2

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/parent: $(SRC_DIR)/parent.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/child1: $(SRC_DIR)/child1.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/child2: $(SRC_DIR)/child2.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean