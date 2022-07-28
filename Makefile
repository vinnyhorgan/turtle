CC := gcc
CFLAGS := -Ideps/include -Wall -Wextra -Wno-unused-value -Wno-unused-parameter -O0 -std=c99 -g
LDFLAGS := -Ldeps/lib -lchipmunk -lenet -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET := turtle

BUILD_DIR := build
SRC_DIRS := src deps/src

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(TARGET) run

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	$(BUILD_DIR)/$(TARGET) demo

debug:
	gdb -tui --args $(BUILD_DIR)/$(TARGET) demo

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)