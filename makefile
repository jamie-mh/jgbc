# Directories and files
BIN=./bin
SRC=./src
OBJ=./obj

# Command and flags
CC=gcc
CFLAGS=-g -lSDL2 -lm -L$(SRC)/ `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

# Other
OUTNAME=lxgbc

# Main rule
default: $(BIN)/$(OUTNAME)
$(BIN)/$(OUTNAME): lxgbc.o cpu.o ram.o rom.o debugger.o
	$(CC) $(CFLAGS) $(OBJ)/lxgbc.o $(OBJ)/cpu.o $(OBJ)/ram.o $(OBJ)/rom.o $(OBJ)/debugger.o -o $(BIN)/$(OUTNAME)

lxgbc.o: $(SRC)/lxgbc.c $(SRC)/lxgbc.h $(SRC)/cpu.h $(SRC)/ram.h $(SRC)/rom.h
	$(CC) $(CFLAGS) -c $(SRC)/lxgbc.c -o $(OBJ)/lxgbc.o

cpu.o: $(SRC)/cpu.c $(SRC)/cpu.h $(SRC)/lxgbc.h
	$(CC) $(CFLAGS) -c $(SRC)/cpu.c -o $(OBJ)/cpu.o

ram.o: $(SRC)/ram.c $(SRC)/ram.h $(SRC)/lxgbc.h
	$(CC) $(CFLAGS) -c $(SRC)/ram.c -o $(OBJ)/ram.o

rom.o: $(SRC)/rom.c $(SRC)/rom.h $(SRC)/lxgbc.h
	$(CC) $(CFLAGS) -c $(SRC)/rom.c -o $(OBJ)/rom.o

debugger.o: $(SRC)/debugger.c $(SRC)/debugger.h $(SRC)/lxgbc.h
	$(CC) $(CFLAGS) -c $(SRC)/debugger.c -o $(OBJ)/debugger.o

clean:
	rm -R *.o *.gch *.exe