#pragma once

#define AUDIO_SAMPLES 2048 
#define AUDIO_CHANNELS 2
#define SAMPLE_RATE 44100 

// Clock Dividers
#define DOWNSAMPLE_DIVIDER CLOCK_SPEED / SAMPLE_RATE
#define FRAME_SEQUENCER_DIVIDER CLOCK_SPEED / 512

// Audio Registers
// Square Wave 1
#define NR10 0xFF10
#define SQUARE_SWEEP 0x70
#define SQUARE_MODE 0x8
#define SQUARE_SHIFT 0x7

#define NR11 0xFF11
#define SQUARE_DUTY_MODE 0xC0
#define SQUARE_LENGTH 0x3F

#define NR12 0xFF12
#define SQUARE_INITIAL_VOLUME 0xF0
#define SQUARE_ENVELOPE_MODE 0x8
#define SQUARE_ENVELOPE_PERIOD 0x7
#define SQUARE_DAC_ENABLED 0xF8

#define NR13 0xFF13 // Frequency LSB

#define NR14 0xFF14
#define SQUARE_TRIGGER 0x80
#define SQUARE_LENGTH_ENABLE 0x40
#define SQUARE_FREQUENCY_MSB 0x7

// Square Wave 2
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19

// Control
#define NR50 0xFF24
#define NR50_VOL_LEFT 0x70
#define NR50_VOL_RIGHT 0x7

#define NR51 0xFF25
#define NR51_SND_4_TO_SO2 0x80
#define NR51_SND_3_TO_SO2 0x40
#define NR51_SND_2_TO_SO2 0x20
#define NR51_SND_1_TO_SO2 0x10
#define NR51_SND_4_TO_SO1 0x8
#define NR51_SND_3_TO_SO1 0x4
#define NR51_SND_2_TO_SO1 0x2
#define NR51_SND_1_TO_SO1 0x1

#define NR52 0xFF26
#define NR52_SND_ENABLED 0x80
#define NR52_SND_4_ON 0x8
#define NR52_SND_3_ON 0x4
#define NR52_SND_2_ON 0x2
#define NR52_SND_1_ON 0x1


void init_apu(GameBoy *gb);
void update_apu(GameBoy *gb);
void audio_register_write(GameBoy *gb, const uint16_t address, const uint8_t value);
