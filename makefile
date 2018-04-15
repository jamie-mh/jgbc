# Directories
BIN := ./bin
SRC := ./src
OBJ := ./obj
INC := ./inc

# Compiler
CC := gcc
CFLAGS := -I$(INC)/ -lSDL2 -lm -Ofast -fomit-frame-pointer -g -Wall -Wextra

# Files
OUTNAME := lxgbc
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Link all objects to create the executable
all: $(OBJECTS)
	$(CC) $^ -o $(BIN)/$(OUTNAME) $(CFLAGS)

# Compile all source files to objects
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ)/* $(BIN)/$(OUTNAME)
