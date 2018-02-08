# LXGBC: GameBoy (Color) Emulator

**License: MIT**

A GameBoy and GameBoy Color emulator for Linux. Written in C with SDL2.

- Compiled and tested on Fedora 27 with gcc 7.2.1.

### Project Status

As it stands, the emulator is not complete enough to play games.

#### Working

- Loading roms (parsing header, etc...)
- Some CPU instructions implemented
- Disassembly and debugger

#### Not Working

- Graphics
- Sound
- Controls
- GameBoy specific features

### Prerequisites

Install the development tools (gcc) and SDL2.

#### Ubuntu

```
sudo apt-get install build-essential libsdl2-dev
```

#### Fedora

```
sudo dnf group install "Development Tools"
sudo dnf install SDL2-devel
```


### Compiling and running

Compile the program with these commands.

```
git clone https://github.com/jamie-mh/lxgbc
cd lxgbc
make
```

To run it, just specify some arguments.

```
./bin/lxgbc -f (path to rom) [-g debugger] [-s scale]
```

Roms are not included in the repository, steal them if you want, not like I advocate that or anything (pls nintendo no sue).

Don't try and run anything other than GB roms, it will crash (duh).


##### Example: Playing Tetris

```
./bin/lxgbc -f roms/Tetris.gb
```

### Using the debugger

When running LXGBC, adding the ``` -g ``` flag will start the debugger and allow you to step through the roms. You will be greeted with this:

``` 
    __   _  ____________  ______
   / /  | |/ / ____/ __ )/ ____/
  / /   |   / / __/ __  / /    
 / /___/   / /_/ / /_/ / /___  
/_____/_/|_\____/_____/\____/   
                                
Title: TETRIS
CGB Flag: 00
Cartridge Type: 00
ROM Size: 2 x 16384 KB
RAM Size: 0 x 8192 KB
Destination Code: 00
Version Number: 00

LXGBC DEBUGGER RUNNING
Type 'h' for information on the available commands.
(dbg) 
```

#### Commands

You can type a command after the ```(dbg)``` prompt, just hit enter afterwards.

```
(1-9): Run X instructions
b: Create a breakpoint
d: Remove a breakpoint
l: List all breakpoints
r: Run
p: Print debug information
f: Dump RAM to a file
q: Quit
```

#### Disassembly preview

This is what it looks like when you step through instructions. Pretty neat.

The instructions are on the left, the current instruction is indicated by the arrow. On the right you will find the various CPU registers and their values.

Underneath, the various flags of the F register are shown, and just next to that, there is the current opcode and whether or not interrupts are enabled.

```
--------------------------------------------
| 0x028C: -> LD HL, dfff       || A : 00   |
| 0x028F:    LD C, 10          || B : 00   |
| 0x0291:    LD B, 00          || C : 13   |
| 0x0293:    LD (HL-), A       || D : 00   |
| 0x0294:    DEC B             || E : D8   |
| 0x0295:    JR NZ, 04         || H : 01   |
| 0x0297:    DEC C             || L : 4D   |
| 0x0298:    JR NZ, 07         || AF: 0080 |
| 0x029A:    LD A, 01          || BC: 0013 |
| 0x029C:    DI                || DE: 00D8 |
| 0x029D:    LDH (0f), A       || HL: 014D |
| 0x029F:    LDH (ff), A       || PC: 028C |
| 0x02A1:    XOR A             || SP: FFFE |
--------------------------------------------
-------------------
| Z: 1 || OPCODE: |
| N: 0 || -> 21   |
| H: 0 || INTERR: |
| C: 0 || -> 1    |
-------------------
```