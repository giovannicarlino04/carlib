##########################
# Compiler Settings Section
##########################
CC = gcc                # The compiler to use
CFLAGS = -Wall -g -D_WIN32      # Compiler flags for debugging and warnings
LDFLAGS = -lws2_32      # Linker flags (Winsock library)

##########################
# Network Test Program Section
##########################
# Source and object files for network_test
SRC_NETWORK = main_network.c gc_common.c gc_debug.c gc_memory.c gc_network.c gc_time.c gc_xml.c 
OBJ_NETWORK = $(SRC_NETWORK:.c=.o)
NETWORK_TARGET = network_test   # Executable name

# Rule to build network_test
$(NETWORK_TARGET): $(OBJ_NETWORK)
	$(CC) $(OBJ_NETWORK) -o $(NETWORK_TARGET) $(LDFLAGS)

##########################
# General Compilation Section
##########################
# Rule to compile .c files to .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

##########################
# Utility Targets Section
##########################
# Clean up the build directory (use 'del' on Windows instead of 'rm')
clean:
	rm -r $(OBJ_NETWORK) $(OBJ_ZIP) $(NETWORK_TARGET) $(ZIP_TARGET)

# Default target to build everything
all: $(NETWORK_TARGET) $(ZIP_TARGET)

##########################
# Phony Targets Section
##########################
# Phony targets are not associated with files and are always executed
.PHONY: all clean install
