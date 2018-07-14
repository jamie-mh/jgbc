# LXGBC: GameBoy (Color) Emulator

**License: MIT**

A GameBoy and GameBoy Color emulator. Written in C/C++ with SDL2.

- Compiled and tested on Fedora 28 with gcc 8.1.1.
- Compiled and tested on Ubuntu 16.04 LTS with gcc 5.4.0.

![Debugger](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/debugger.png)

### Project Status

#### Working

- Loading roms (parsing header, etc...)
- All CPU instructions implemented correctly
- Disassembly and debugger
- Background / Window graphics
- Sprites
- Interrupts
- Timers
- Joypad
- MBC 1

#### Not Working

- Sound
- GameBoy Color specific features
- Other MBCs

#### Blargg's CPU Tests

| Name          | Status | Output |
| ------------- |:------:|:------:|
| 01-special | :heavy_check_mark: | Passed |
| 02-interrupts | :heavy_check_mark: | Passed |
| 03-op sp,hl | :heavy_check_mark: | Passed |
| 04-op r,imm | :heavy_check_mark: | Passed |
| 05-op rp | :heavy_check_mark: | Passed |
| 06-ld r,r | :heavy_check_mark: | Passed |
| 07-jr,jp,call,ret,rst | :heavy_check_mark: | Passed |
| 08-misc instrs | :heavy_check_mark: | Passed |
| 09-op r,r | :heavy_check_mark: | Passed |
| 10-bit ops | :heavy_check_mark: | Passed |
| 11-op a,(hl) | :heavy_check_mark: | Passed |

### Prerequisites (Linux)

Install the development tools (gcc) and SDL2.

#### Ubuntu

```
sudo apt-get install build-essential libsdl2-dev cmake
```

#### Fedora

```
sudo dnf group install "Development Tools"
sudo dnf install SDL2-devel cmake
```


### Compiling and running (Linux)

Compile the program with these commands.

```
git clone https://github.com/jamie-mh/lxgbc
cd lxgbc
cmake CMakeLists.txt
make
```

To run it, just specify some arguments.

```
./bin/lxgbc (path to rom)
```

Roms are not included in the repository, steal them if you want, not like I advocate that or anything (pls nintendo no sue).

Don't try and run anything other than GB roms, it will crash (duh).


##### Example: Playing Tetris

```
./bin/lxgbc roms/Tetris.gb
```
### Screenshots

![Tetris](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/tetris.png)
![Dr Mario](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/drmario.png)
![Super Mario Land 1](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/mario1.png)
![Super Mario Land 2](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/mario2.png)
![Wario Land](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/wario.png)
![CPU test](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/cpuinstr.png)
![Tetris DX](https://raw.githubusercontent.com/jamie-mh/lxgbc/master/doc/tetrisdx.png)

### Using the debugger

You can launch the debugger by running the ``` lxgbc_debugger ``` executable in the ``` bin ``` directory.

#### WIP
