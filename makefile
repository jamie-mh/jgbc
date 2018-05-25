BIN := ./bin
SRC := ./src
OBJ := ./obj
INC := ./inc

CC := gcc
CFLAGS := -I$(INC)/ -lSDL2 -lm -Ofast -fomit-frame-pointer -g -Wall -Wextra

OUTNAME := lxgbc
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: $(OBJECTS)
	$(CC) $^ -o $(BIN)/$(OUTNAME) $(CFLAGS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ)/* $(BIN)/$(OUTNAME)
