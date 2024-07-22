# jgbc: GameBoy (Color) Emulator

A fast and semi-accurate GameBoy (Color) emulator for Windows and Linux.
Written in C/C++ with SDL2.

## Screenshots

![Zelda](./doc/zelda.png)
![Tetris](./doc/tetris.png)
![Dr Mario](./doc/drmario.png)
![Pokemon Blue](./doc/pokemonblue.png)
![Super Mario Land 1](./doc/mario1.png)
![Super Mario Land 2](./doc/mario2.png)
![Super Mario Land 3](./doc/mario3.png)
![Kirby's Dream Land](./doc/kirby.png)
![Metroid II](./doc/metroid.png)
![Donkey Kong Land](./doc/donkeykong.png)
![Tennis](./doc/tennis.png)
![Pinball Deluxe](./doc/pinballdeluxe.png)

## Project Status

### Working

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

### Not Working

- Some GameBoy Color specific features (in progress)
- Other MBCs

## Tests

![CPU instructions](./doc/cpu-instrs.png)
![Instruction timing](./doc/instr-timing.png)
![Memory timing](./doc/mem-timing.png)
![DMG ACID](./doc/dmg-acid.png)
![CGB ACID](./doc/cgb-acid.png)

### CPU instructions

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

