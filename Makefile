# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Executable and source files
EXEC = main.exe
SRC = main.c network.c
HEADERS = network.h

# Build target
all: $(EXEC)

$(EXEC): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SRC) -lws2_32

# Clean target
clean:
	del /q $(EXEC)
