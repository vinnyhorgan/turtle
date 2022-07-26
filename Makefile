CC := gcc
CFLAGS := -Ideps/include -std=c99 #-O2
LDFLAGS := -Ldeps/lib -lchipmunk -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

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

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)