# Makefile for KOF

# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = .

# Source and object files
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))

# Executable
EXECUTABLE = $(BINDIR)/game

# Default target
all: clean $(EXECUTABLE)

# Linking executable from object files
$(EXECUTABLE): $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compiling object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove object files and executable
clean:
	rm -f $(OBJFILES) $(EXECUTABLE)

# Run target to execute the game
run: 
	./game
