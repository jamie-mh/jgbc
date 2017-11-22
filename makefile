# Directories and files
BIN=./bin
SRC=./src
OBJ=./obj

# Command and flags
CC=gcc
CFLAGS=-g -lSDL2 -lm -L$(SRC)/

# Other
OUTNAME=lxgbc

# Main rule
default: lxgbc
lxgbc: lxgbc.o
	$(CC) $(CFLAGS) $(OBJ)/lxgbc.o -o $(BIN)/$(OUTNAME)

lxgbc.o: $(SRC)/lxgbc.c $(SRC)/lxgbc.h
	$(CC) $(CFLAGS) -c $(SRC)/lxgbc.c -o $(OBJ)/lxgbc.o

clean:
	rm -R *.o *.gch *.exe