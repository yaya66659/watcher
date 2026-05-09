CC := gcc
CPPFLAGS := -Iheaders
CFLAGS := -Wall -Wextra -Werror -std=c11
TARGET := bin/watcher
BUILD_DIR := build

SRCS := src/main.c \
	src/core/command_runner.c \
	src/core/process_result.c \
	src/ui/console_view.c

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean test test-fr retest

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

test: all
	sh ./tests/test_cli.sh

test-fr: all
	sh ./tests/test_cli_fr.sh

retest: clean test

-include $(DEPS)
