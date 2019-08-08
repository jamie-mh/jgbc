#pragma once

#define AUDIO_SAMPLES 1024 
#define AUDIO_CHANNELS 2
#define SAMPLE_RATE 44100 

#define CHANNEL_SQUARE_1 0
#define CHANNEL_SQUARE_2 1
#define CHANNEL_WAVE 2
#define CHANNEL_NOISE 3

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
#define CHANNEL_LENGTH 0x3F

#define NR12 0xFF12
#define CHANNEL_ENVELOPE_INITIAL_VOLUME 0xF0
#define CHANNEL_ENVELOPE_MODE 0x8
#define CHANNEL_ENVELOPE_PERIOD 0x7
#define SQUARE_DAC_ENABLED 0xF8

#define NR13 0xFF13 // Frequency LSB

#define NR14 0xFF14
#define CHANNEL_TRIGGER 0x80
#define CHANNEL_LENGTH_ENABLE 0x40
#define CHANNEL_FREQUENCY_MSB 0x7

// Square Wave 2
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19

// Wave
#define NR30 0xFF1A
#define WAVE_ENABLED 0x80

#define NR31 0xFF1B // Length load

#define NR32 0xFF1C
#define WAVE_VOLUME 0x60

#define NR33 0xFF1D // Frequency LSB
#define NR34 0xFF1E

#define WAVE_TABLE_START 0xFF30

// Noise
#define NR41 0xFF20
#define NR42 0xFF21

#define NR43 0xFF22
#define NOISE_CLOCK_SHIFT 0xF0
#define NOISE_WIDTH_MODE 0X8
#define NOISE_DIVISOR_CODE 0x7

#define NR44 0xFF23

// Control
#define NR50 0xFF24
#define VOL_LEFT 0x70
#define VOL_RIGHT 0x7

#define NR51 0xFF25
#define SND_4_TO_SO2 0x80
#define SND_3_TO_SO2 0x40
#define SND_2_TO_SO2 0x20
#define SND_1_TO_SO2 0x10
#define SND_4_TO_SO1 0x8
#define SND_3_TO_SO1 0x4
#define SND_2_TO_SO1 0x2
#define SND_1_TO_SO1 0x1

#define NR52 0xFF26
#define SND_ENABLED 0x80
#define SND_4_ON 0x8
#define SND_3_ON 0x4
#define SND_2_ON 0x2
#define SND_1_ON 0x1


void init_apu(GameBoy *gb);
void update_apu(GameBoy *gb);
void audio_register_write(GameBoy *gb, const uint16_t address, const uint8_t value);
