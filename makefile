# Directories
BIN := ./bin
SRC := ./src
OBJ := ./obj
LIB := ./lib

# Compiler
CC := gcc
CFLAGS := -I$(SRC)/ -lSDL2 -L$(LIB)/ -g

# Files
OUTNAME := lxgbc
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Link all objects to create the executable
all: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $(BIN)/$(OUTNAME)

# Compile all source files to objects
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)/* $(BIN)/*