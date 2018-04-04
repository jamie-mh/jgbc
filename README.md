# LXGBC: GameBoy (Color) Emulator

**License: MIT**

A GameBoy and GameBoy Color emulator for Linux. Written in C with SDL2.

- Compiled and tested on Fedora 27 with gcc 7.3.1.
- Compiled and tested on Ubuntu 16.04 LTS with gcc 5.4.0.

### Project Status

As it stands, the emulator is not complete enough to play games.

#### Working

- Loading roms (parsing header, etc...)
- Most CPU instructions implemented
- Disassembly and debugger
- Background graphics
- Interrupts

#### Not Working

- Sprites / Window
- Sound
- Joypad
- GameBoy Color specific features

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
git clone --recurse-submodules https://github.com/jamie-mh/lxgbc
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
### Screenshots

![Dr Mario](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/drmario.png)

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
Available Commands:
(1-9): Run X instructions
b: Create a breakpoint
d: Remove a breakpoint
l: List all breakpoints
r: Run
p: Print debug information
s: Read byte in memory
f: Dump RAM to a file
q: Quit
```

#### Disassembly preview

This is what it looks like when you step through instructions. Pretty neat.

The instructions are on the left, the current instruction is indicated by the arrow. On the right you will find the various CPU registers and their values.

Underneath, the various flags of the F register are shown, and just next to that, there is the current opcode and the current scanline.
```
--------------------------------------------
| 0x0150: -> JP 01e8           || A : 01   |
| 0x0153:    LD BC, d00e       || B : 00   |
| 0x0156:    LD A, (BC)        || C : 13   |
| 0x0157:    AND A             || D : 00   |
| 0x0158:    JR NZ, f3         || E : D8   |
| 0x015A:    LDH A, (cf)       || H : 01   |
| 0x015C:    CP fe             || L : 4D   |
| 0x015E:    JR NZ, fc         || AF: 01B0 |
| 0x0160:    LD A, 01          || BC: 0013 |
| 0x0162:    JR ff             || DE: 00D8 |
| 0x0164:    XOR A             || HL: 014D |
| 0x0165:    LD (BC), A        || SP: FFFE |
| 0x0166:    RET               || PC: 0150 |
| 0x0167:    LD A, (d046)      || IME: 1   |
--------------------------------------------
-------------------
| Z: 1 || OPCODE: |
| N: 0 || -> C3   |
| H: 1 || LY:     |
| C: 1 || -> 00   |
-------------------
```
