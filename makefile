# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -I./include
LDFLAGS = -lsqlite3

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files and output binary
SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/database.cpp 
OUTPUT = $(BUILD_DIR)/inventory_manager.out

# OS detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    # Linux specific settings
    LDFLAGS = -lsqlite3
else
    # Windows specific settings
    OUTPUT = $(BUILD_DIR)/inventory_manager.exe
    LDFLAGS = -lsqlite3
endif

# Default target
all: $(OUTPUT)

# Build target
$(OUTPUT): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Clean up
clean:
	rm -f $(OUTPUT)

.PHONY: all clean
