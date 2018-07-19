# jgbc: GameBoy (Color) Emulator

**License: MIT**

A fast and semi-accurate GameBoy (Color) emulator for Windows, macOS And Linux.
Written in C/C++ with SDL2.

- Compiled and tested on Fedora 28 with gcc 8.1.1.
- Compiled and tested on Ubuntu 16.04 LTS with gcc 5.4.0.
- Compiled and tested on Windows 10 with Visual Studio 2017.

![Debugger](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/debugger.png)

### Screenshots

![Zelda](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/zelda.png)
![Tetris](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/tetris.png)
![Dr Mario](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/drmario.png)
![Pokemon Blue](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/pokemonblue.png)
![Super Mario Land 1](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/mario1.png)
![Super Mario Land 2](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/mario2.png)
![Super Mario Land 3](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/mario3.png)
![Kirby's Dream Land](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/kirby.png)
![Metroid II](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/metroid.png)
![Donkey Kong Land](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/donkeykong.png)
![Tennis](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/tennis.png)
![Pinball Deluxe](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/pinballdeluxe.png)
![CPU test](https://raw.githubusercontent.com/jamie-mh/jgbc/master/doc/cpuinstr.png)

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
git clone https://github.com/jamie-mh/jgbc
cd jgbc
cmake CMakeLists.txt
make
```

To run it, just specify some arguments.

```
./bin/jgbc (path to rom)
```

Roms are not included in the repository, steal them if you want, not like I advocate that or anything (pls nintendo no sue).

Don't try and run anything other than GB roms, it will crash (duh).


##### Example: Playing Tetris

```
./bin/jgbc roms/Tetris.gb
```

### Using the debugger

You can launch the debugger by running the ``` jgbc_debugger ``` executable in the ``` bin ``` directory.

#### WIP
