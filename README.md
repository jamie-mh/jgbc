# jgbc: GameBoy (Color) Emulator

A fast and semi-accurate GameBoy (Color) emulator for Windows and Linux.
Written in C/C++ with SDL2.

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
- Background / Window graphics
- Sprites
- Interrupts
- Timers
- Joypad
- MBC 1, 5
- Sound
- Save games

#### Not Working

- GameBoy Color specific features (in progress)
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
