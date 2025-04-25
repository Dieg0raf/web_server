# Compiler and flags
CC = g++
CFLAGS_BASE = -g -Werror -Wall -std=c++17 -I ./include
LDFLAGS = -pthread

# If DEBUGGER is set, don't use ASAN
ifdef DEBUGGER
    CFLAGS = $(CFLAGS_BASE)
else
    CFLAGS = $(CFLAGS_BASE) -fsanitize=address
endif

# Directories
SRC_DIR = src
HTTP_DIR = src/http
SOCKET_DIR = src/socket
RESOURCE_DIR = src/resource
LOG_DIR = src/logger
INCLUDE_DIR = include

# Source and object files
SRC = $(wildcard $(HTTP_DIR)/*.cpp) $(wildcard $(SOCKET_DIR)/*cpp) $(wildcard $(LOG_DIR)/*cpp) $(wildcard $(RESOURCE_DIR)/*cpp) $(SRC_DIR)/server.cpp
OBJ = $(SRC:.cpp=.o)

# Target executable
EXE = server

# Default target (all)
all: $(EXE)

# Linking the final executable for the web server
$(EXE): $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)

# Rule to create object files from .cpp files
src/http/%.o: src/http/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

src/socket/%.o: src/socket/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

src/logger/%.o: src/logger/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

src/resource/%.o: src/resource/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(EXE) $(OBJ) *~ core.*

# Phony targets
.PHONY: all clean

