# Compiler
CC := g++

# Compiler flags
CFLAGS := -Wall -Wextra -std=c++11 -lm

# Source files
SRCS := $(wildcard *.cpp)

# Object files
OBJS := $(SRCS:.cpp=.o)

# Executable name
TARGET := correlation check keyrec

# Default target
all: $(TARGET)

keyrec: keyrec.o skinny-64.o
	$(CC) $(CFLAGS) $^ -o $@

correlation: correlation.o skinny-64.o
	$(CC) $(CFLAGS) $^ -o $@

check: check.o skinny-64.o
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJS) $(TARGET)