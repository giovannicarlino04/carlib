CC = gcc                # The compiler to use
CFLAGS = -Wall -g -D_WIN32      # Compiler flags for debugging and warnings
LDFLAGS = -lws2_32      # Linker flags (Winsock library)

SRC = main.c gc_common.c gc_iggy.c gc_debug.c gc_memory.c gc_network.c gc_time.c gc_xml.c 
OBJ = $(SRC:.c=.o)
TARGET = main   # Executable name

# Rule to build main
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(OBJ) $(OBJ_ZIP) $(TARGET) $(ZIP_TARGET)

# Default target to build everything
all: $(TARGET) $(ZIP_TARGET)

.PHONY: all clean install
