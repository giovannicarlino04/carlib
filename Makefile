# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Executable and source files
EXEC = main.exe
SRC = main.c network.c debug.c time.c
HEADERS = network.h debug.h time.h
LIBS = -static -lws2_32


# Build target
all: $(EXEC)

$(EXEC): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LIBS)

# Clean target
clean:
	rm $(EXEC)
