CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -lncurses

SRC_DIR = src
BUILD_DIR = build
TARGET = system_monitor

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean docs

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

docs:
	doxygen Doxyfile

clean:
	rm -rf $(BUILD_DIR) docs/html docs/latex 