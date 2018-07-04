BIN := ./bin
SRC := ./src
OBJ := ./obj
INC := ./inc

CC := gcc
CFLAGS := -I$(INC)/ -lSDL2 -lm -g -Wall -Wextra

OUTNAME := lxgbc
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $(BIN)/$(OUTNAME)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)/* $(BIN)/$(OUTNAME)
