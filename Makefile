# Makefile for compiling user.c

CC = gcc  # The compiler to be used
CFLAGS = -Wall -Wextra  # Compilation flags

TARGET = user  # Name of the final executable

# Default target, invoked when you run 'make' or 'mk'
all: $(TARGET)

# Rule for building the executable
$(TARGET): user.c
	$(CC) $(CFLAGS) -o $(TARGET) user.c

# Rule to clean up the generated files
clean:
	rm -f $(TARGET)

# Define an alias 'mk' for 'make'
mk: all

.PHONY: all clean mk
