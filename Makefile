# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -g

# Source files
SRCS = \
	include/literals.c \
	src/main.c \
	src/core/session.c \
	src/core/timer.c \
	src/core/config.c \
	src/data/history.c \
	src/data/progress.c \
	src/data/management.c \
	src/data/stats.c \
	src/tui/ui.c \
	src/tui/primitives.c \
	src/tui/screens/timer.c \
	src/tui/views/timer.c \
	src/tui/terminal/key_command.c \
	src/tui/terminal/cli_command.c \
	src/tui/terminal/setup.c

# Output executable
TARGET = cpomo

.PHONY: all clean rebuild

# Default target
all: $(TARGET)

# Build executable (single command)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean build
clean:
	rm -f $(TARGET)

# Rebuild everything
rebuild: clean all
