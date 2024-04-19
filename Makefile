# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -Wall -Wextra -Werror

# Define the target executable
TARGET = pc

# Define sources
SRCS = pc.c eventbuf.c

# Rule to link the object files into the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# target to clean the workspace
clean:
	rm -f $(TARGET)