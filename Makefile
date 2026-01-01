# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -g

# Source files
SRCS = \
	include/literals.c \
	src/main.c \
	src/timer.c \
	src/config.c \
	src/data/history.c \
	src/data/progress.c \
	src/tui/ui.c \
	src/tui/screens/timer.c \
	src/tui/terminal/command.c \
	src/tui/terminal/setup.c

# Output executable
TARGET = timer

# Default target
all: $(TARGET)

# Build executable (single command, no .o files)
$(TARGET):
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean build
clean:
	rm -f $(TARGET)

# Rebuild everything
rebuild: clean all