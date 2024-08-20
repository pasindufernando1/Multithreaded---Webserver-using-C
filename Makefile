# Makefile for the web server project

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Source file
SRC = main.c

# Output executable
TARGET = main

# Default target to build the executable
all: $(TARGET)

# Rule to compile the source file and link it to create the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Rule to clean up the build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
