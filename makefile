# Directories
BIN := ./bin
SRC := ./src
OBJ := ./obj

# Compiler
CC := gcc
CFLAGS := -I$(SRC)/ -lSDL2 -lm -g

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
