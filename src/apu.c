#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "apu.h"

static inline void init_square_wave(GameBoy *gb, const uint8_t idx);
static inline void read_square(GameBoy *gb, const uint16_t address, const uint8_t value, const uint8_t idx);
static inline void update_square(GameBoy *gb, const uint8_t idx);
static inline void update_square_envelope(GameBoy *gb, const uint8_t idx);
static inline void update_square_sweep(GameBoy *gb);
static inline void update_square_length(GameBoy *gb, const uint8_t idx);
static inline void trigger_square(GameBoy *gb, const uint8_t idx);


void init_apu(GameBoy *gb) {

    SDL_AudioSpec *desired = &gb->apu.desired_spec;
    SDL_zero(*desired);
    desired->freq = SAMPLE_RATE;
    desired->format = AUDIO_F32SYS;
    desired->channels = AUDIO_CHANNELS;
    desired->samples = AUDIO_SAMPLES;

    gb->apu.device_id = SDL_OpenAudioDevice(
        NULL, 
        0, 
        desired, 
        &gb->apu.actual_spec, 
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
    );

    gb->apu.enabled = true;
    gb->apu.frame_sequencer_clock = 0;
    gb->apu.frame_sequencer_step = 0;
    gb->apu.downsample_clock = 0;

    gb->apu.buffer = malloc(gb->apu.actual_spec.size);
    gb->apu.buffer_position = 0;

    init_square_wave(gb, 0);
    init_square_wave(gb, 1);
}

static void init_square_wave(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    square->enabled = true;
    square->dac_enabled = true;
    square->frequency = 0;
    square->clock = 0;

    square->duty.mode = 0;
    square->duty.step = 0;

    square->length.enabled = false;
    square->length.clock = 0;

    square->envelope.period = 0;
    square->envelope.initial_volume = 0;
    square->envelope.current_volume = 0;
    square->envelope.mode = Decrease;

    square->sweep.enabled = false;
    square->sweep.period = 0;
    square->sweep.shift = 0;
    square->sweep.current_frequency = 0;
    square->sweep.mode = Addition;
}

void update_apu(GameBoy *gb) {

    if(!gb->apu.enabled)
        return;

    for(int i = 0; i < gb->cpu.ticks; ++i) { 

        if(gb->apu.frame_sequencer_clock >= FRAME_SEQUENCER_DIVIDER) {

            gb->apu.frame_sequencer_clock = 0;
            gb->apu.frame_sequencer_step++;
            gb->apu.frame_sequencer_step %= 9;

            switch(gb->apu.frame_sequencer_step) {
                case 2:
                case 6:
                    /*update_square_sweep(gb);*/
                case 0:
                case 4:
                    update_square_length(gb, 0);
                    update_square_length(gb, 1);
                    break;

                case 7: // every 8 clocks
                    update_square_envelope(gb, 0);
                    update_square_envelope(gb, 1);
                    break;
            }
        }
        else
            gb->apu.frame_sequencer_clock++;

        update_square(gb, 0);
        update_square(gb, 1);

        if(gb->apu.downsample_clock++ >= DOWNSAMPLE_DIVIDER) {
            gb->apu.downsample_clock = 0;

            float volume_left = (SREAD8(NR50) & NR50_VOL_LEFT) >> 4;
            volume_left /= 7;

            float volume_right = SREAD8(NR50) & NR50_VOL_RIGHT;
            volume_right /= 7;

            const uint8_t left = (gb->apu.channels[0] + gb->apu.channels[1]) * volume_left;
            const uint8_t right = (gb->apu.channels[0] + gb->apu.channels[1]) * volume_right;

            gb->apu.buffer[gb->apu.buffer_position] = left / 255.0f;
            gb->apu.buffer[gb->apu.buffer_position + 1] = right / 255.0f;

            gb->apu.buffer_position += AUDIO_CHANNELS;
        }
    }

    if(gb->apu.buffer_position >= gb->apu.actual_spec.size / sizeof(float)) {
        SDL_QueueAudio(gb->apu.device_id, gb->apu.buffer, gb->apu.actual_spec.size);
        gb->apu.buffer_position = 0;
    }
}

void audio_register_write(GameBoy *gb, const uint16_t address, const uint8_t value) {

    switch(address) {

        // Square Wave 1
        case NR10:
        case NR11:
        case NR12:
        case NR13:
        case NR14:
            read_square(gb, address, value, 0);
            break;

        // Square Wave 2
        case NR21:
        case NR22:
        case NR23:
        case NR24:
            read_square(gb, address, value, 1);
            break;

        case NR52:
            gb->apu.enabled = (value & NR52_SND_ENABLED);
    }
}

static inline void read_square(GameBoy *gb, const uint16_t address, const uint8_t value, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    switch(address) {
        
        case NR10:
            square->sweep.period = (SweepMode) ((value & SQUARE_SWEEP) >> 4);
            square->sweep.mode = (value & SQUARE_MODE) >> 3;
            square->sweep.shift = value & SQUARE_SHIFT;
            break;

        case NR11:
        case NR21:
            square->duty.mode = (value & SQUARE_DUTY_MODE) >> 6;
            square->length.clock = value & SQUARE_LENGTH;
            break;

        case NR12:
        case NR22:
            square->envelope.initial_volume = (value & SQUARE_INITIAL_VOLUME) >> 4;
            square->envelope.mode = (EnvelopeMode) (value & SQUARE_ENVELOPE_MODE) >> 3;
            square->envelope.period = value & SQUARE_ENVELOPE_PERIOD;
            square->dac_enabled = value & SQUARE_DAC_ENABLED; // check upper 5 bits for 0
            break;

        case NR13:
        case NR23:
            square->frequency &= 0xFF00;
            square->frequency |= value;
            break;

        case NR14:
        case NR24:
            square->length.enabled = (value & SQUARE_LENGTH_ENABLE) >> 6;
            square->frequency = ((value & SQUARE_FREQUENCY_MSB) << 8) | (square->frequency & 0xFF);
            if(value & SQUARE_TRIGGER) trigger_square(gb, idx);
            break;
    }
}

static inline void update_square(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    static const bool duty_table[4][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 1 }, // 12.5%
        { 1, 0, 0, 0, 0, 0, 0, 1 }, // 25%
        { 1, 0, 0, 0, 0, 1, 1, 1 }, // 50%
        { 0, 1, 1, 1, 1, 1, 1, 0 }  // 75%
    };

    square->clock--;
    if(square->clock <= 0)
    {
        square->clock = (2048 - square->frequency) * 4;
        square->duty.step++;
        square->duty.step &= 0x7;
    }

    if(square->enabled && square->dac_enabled && 
       duty_table[square->duty.mode][square->duty.step]) {

        // Maximum envelope volume is 15 so fill the byte 255 / 15 = 17
        gb->apu.channels[idx] = square->envelope.current_volume * 17;
    }
    else {
        gb->apu.channels[idx] = 0;
    }
}

static inline void update_square_envelope(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    if(square->envelope.period == 0)
        return;

    switch(square->envelope.mode) {
        case Increase: 
            if(square->envelope.current_volume < 15)
                square->envelope.current_volume++;

            break;

        case Decrease: 
            if(square->envelope.current_volume > 0)
                square->envelope.current_volume--;

            break;
    }
}

static inline void update_square_sweep(GameBoy *gb) {

    SquareWave *square = &gb->apu.square_waves[0];

    if(!square->sweep.enabled || square->sweep.period == 0)
        return;

    // TODO: Implement
}

static inline void update_square_length(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    if(!square->length.enabled || square->length.clock == 0)
        return;

    if(square->length.clock-- == 0)
        square->enabled = false;
}

static inline void trigger_square(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];
    square->enabled = true;

    if(square->length.clock == 0)
        square->length.clock = 64;

    square->clock = (2048 - square->frequency) * 4;
    square->envelope.current_volume = square->envelope.initial_volume;

    square->sweep.current_frequency = square->frequency;
    // sweep timer reset
    square->sweep.enabled = (square->sweep.period > 0 || square->sweep.shift > 0);

    /*if(square->sweep.shift > 0)*/
        // frequency calculation and overflow check
}
