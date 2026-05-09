CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c11
TARGET := bin/watcher
BUILD_DIR := build

SRCS := src/main.c \
	src/core/command_runner.c \
	src/core/process_result.c \
	src/ui/console_view.c

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
