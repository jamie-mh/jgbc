#include "apu.h"
#include "cpu.h"
#include "macro.h"
#include "mmu.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void update_envelope(ChannelEnvelope *);
static void update_length(ChannelLength *, bool *);

static void reset_square_wave(GameBoy *gb, uint8_t idx);
static void read_square(GameBoy *, uint16_t, uint8_t, uint8_t);
static void update_square(GameBoy *, uint8_t);
static void update_square_sweep(GameBoy *);
static void trigger_square(GameBoy *, uint8_t);

static void reset_wave(GameBoy *gb);
static void read_wave(GameBoy *, uint16_t, uint8_t);
static void update_wave(GameBoy *);
static void trigger_wave(GameBoy *);

static void reset_noise(GameBoy *gb);
static void read_noise(GameBoy *, uint16_t, uint8_t);
static void update_noise(GameBoy *);
static void trigger_noise(GameBoy *);


void init_apu(GameBoy *gb) {

    SDL_AudioSpec desired_spec;
    SDL_zero(desired_spec);
    
    desired_spec.freq = SAMPLE_RATE;
    desired_spec.format = AUDIO_F32SYS;
    desired_spec.channels = AUDIO_CHANNELS;
    desired_spec.samples = AUDIO_SAMPLES;

    gb->apu.device_id = SDL_OpenAudioDevice(
        NULL, 
        0, 
        &desired_spec, 
        &gb->apu.audio_spec, 
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
    );

    gb->apu.buffer = malloc(gb->apu.audio_spec.size);
}

void reset_apu(GameBoy *gb) {

    reset_square_wave(gb, 0);
    reset_square_wave(gb, 1);
    reset_wave(gb);
    reset_noise(gb);

    for(int i = 0; i < 4; ++i) {
        gb->apu.left_enabled[i] = true;
        gb->apu.right_enabled[i] = true;
    }

    gb->apu.enabled = true;
    gb->apu.frame_sequencer.clock = 0;
    gb->apu.frame_sequencer.step = 0;
    gb->apu.downsample_clock = 0;

    gb->apu.left_volume = 0;
    gb->apu.right_volume = 0;

    gb->apu.buffer_position = 0;
    memset(gb->apu.buffer, 0, gb->apu.audio_spec.size);
}

static void reset_square_wave(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    square->enabled = false;
    square->dac_enabled = false;
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

    square->sweep.period = 0;
    square->sweep.shift = 0;
    square->sweep.mode = Addition;
}

static void reset_wave(GameBoy *gb) {

    Wave *wave = &gb->apu.wave;
    wave->enabled = false;
    wave->position = 0;
    wave->clock = 0;
    wave->frequency = 0;
    wave->length.clock = 0;
    wave->length.enabled = false;
    wave->volume_code = 0;
}

static void reset_noise(GameBoy *gb) {

    Noise *noise = &gb->apu.noise;
    noise->enabled = false;
    noise->clock = 0;
    noise->clock_shift = 0;
    noise->divisor_code = 0;
    noise->last_result = 0;
    noise->lfsr = 0;

    noise->envelope.period = 0;
    noise->envelope.initial_volume = 0;
    noise->envelope.current_volume = 0;
    noise->envelope.mode = Decrease;

    noise->length.clock = 0;
    noise->length.enabled = false;
    noise->width_mode = 0;
}

void update_apu(GameBoy *gb) {

    APU *apu = &gb->apu;

    if(!apu->enabled)
        return;

    for(int i = 0; i < gb->cpu.ticks; ++i) { 

        if(apu->frame_sequencer.clock >= FRAME_SEQUENCER_DIVIDER) {

            apu->frame_sequencer.clock = 0;
            apu->frame_sequencer.step++;
            apu->frame_sequencer.step %= 9;

            switch(gb->apu.frame_sequencer.step) {
                case 2:
                case 6:
                    update_square_sweep(gb);
                    // fallthrough
                case 0:
                case 4:
                    update_length(&apu->square_waves[0].length, &apu->square_waves[0].enabled);
                    update_length(&apu->square_waves[1].length, &apu->square_waves[1].enabled);
                    update_length(&apu->wave.length, &apu->wave.enabled);
                    update_length(&apu->noise.length, &apu->noise.enabled);
                    break;

                case 7: // every 8 clocks
                    update_envelope(&apu->square_waves[0].envelope);
                    update_envelope(&apu->square_waves[1].envelope);
                    update_envelope(&apu->noise.envelope);
                    break;
            }
        }
        else
            apu->frame_sequencer.clock++;

        update_square(gb, 0);
        update_square(gb, 1);
        update_wave(gb);
        update_noise(gb);

        if(apu->downsample_clock++ >= DOWNSAMPLE_DIVIDER) {
            apu->downsample_clock = 0;

            float left = 0.0f;
            float right = 0.0f;

            for(int j = 0; j < 4; ++j) {

                if(apu->left_enabled[j])
                    left += apu->channels[j];

                if(apu->right_enabled[j])
                    right += apu->channels[j];
            }

            left *= (float) gb->apu.left_volume / 7.f;
            right *= (float) gb->apu.right_volume / 7.f;

            apu->buffer[apu->buffer_position] = left / 60.0f;
            apu->buffer[apu->buffer_position + 1] = right / 60.0f;

            apu->buffer_position += AUDIO_CHANNELS;
        }
    }

    const size_t max_samples = apu->audio_spec.size / sizeof(float);

    if(apu->buffer_position >= max_samples) {
        SDL_QueueAudio(apu->device_id, apu->buffer, apu->audio_spec.size);
        apu->buffer_position = 0;
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
        case NR20: break;
        case NR21:
        case NR22:
        case NR23:
        case NR24:
            read_square(gb, address, value, 1);
            break;

        // Wave
        case NR30:
        case NR31:
        case NR32:
        case NR33:
        case NR34:
            read_wave(gb, address, value);
            break;

        // Noise
        case NR40: break;
        case NR41:
        case NR42:
        case NR43:
        case NR44:
            read_noise(gb, address, value);
            break;

        // Channel control
        case NR50:
            gb->apu.left_volume = (value & VOL_LEFT) >> 4;
            gb->apu.right_volume = value & VOL_RIGHT;
            break;

        // Channel enable
        case NR51:
            gb->apu.right_enabled[CHANNEL_NOISE] = (value & SND_4_TO_SO2) >> 7;
            gb->apu.right_enabled[CHANNEL_WAVE] = (value & SND_3_TO_SO2) >> 6;
            gb->apu.right_enabled[CHANNEL_SQUARE_2] = (value & SND_2_TO_SO2) >> 5;
            gb->apu.right_enabled[CHANNEL_SQUARE_1] = (value & SND_1_TO_SO2) >> 4;

            gb->apu.left_enabled[CHANNEL_NOISE] = (value & SND_4_TO_SO1) >> 3;
            gb->apu.left_enabled[CHANNEL_WAVE] = (value & SND_3_TO_SO1) >> 2;
            gb->apu.left_enabled[CHANNEL_SQUARE_2] = (value & SND_2_TO_SO1) >> 1;
            gb->apu.left_enabled[CHANNEL_SQUARE_1] = value & SND_1_TO_SO1;
            break;

        case NR52:
            gb->apu.enabled = (value & SND_ENABLED);
            break;

        default:
            ASSERT_NOT_REACHED();
    }
}

static void update_envelope(ChannelEnvelope *envelope) {

    if(envelope->period == 0)
        return;

    if(envelope->mode == Increase && envelope->current_volume < 15)
        envelope->current_volume++;
    else if(envelope->mode == Decrease && envelope->current_volume > 0)
        envelope->current_volume--;
}

static void update_length(ChannelLength *length, bool *channel_enabled) {

    if(!length->enabled || length->clock == 0)
        return;

    if(length->clock-- == 0)
        *channel_enabled = false;
}

static void read_square(GameBoy *gb, const uint16_t address, const uint8_t value, const uint8_t idx) {

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
            square->length.clock = value & CHANNEL_LENGTH;
            break;

        case NR12:
        case NR22:
            square->envelope.initial_volume = (value & CHANNEL_ENVELOPE_INITIAL_VOLUME) >> 4;
            square->envelope.mode = (EnvelopeMode) (value & CHANNEL_ENVELOPE_MODE) >> 3;
            square->envelope.period = value & CHANNEL_ENVELOPE_PERIOD;
            square->dac_enabled = value & SQUARE_DAC_ENABLED; // check upper 5 bits for 0
            break;

        case NR13:
        case NR23:
            square->frequency &= 0xFF00;
            square->frequency |= value;
            break;

        case NR14:
        case NR24:
            square->length.enabled = (value & CHANNEL_LENGTH_ENABLE) >> 6;
            square->frequency = ((value & CHANNEL_FREQUENCY_MSB) << 8) | (square->frequency & 0xFF);
            if(value & CHANNEL_TRIGGER) trigger_square(gb, idx);
            break;

        default:
            ASSERT_NOT_REACHED();
    }
}

static void update_square(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];

    static const bool duty_table[4][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 1 }, // 12.5%
        { 1, 0, 0, 0, 0, 0, 0, 1 }, // 25%
        { 1, 0, 0, 0, 0, 1, 1, 1 }, // 50%
        { 0, 1, 1, 1, 1, 1, 1, 0 }  // 75%
    };

    if(square->clock-- <= 0)
    {
        square->clock = (2048 - square->frequency) * 4;
        square->duty.step++;
        square->duty.step &= 0x7;
    }

    if(square->enabled && square->dac_enabled && 
       duty_table[square->duty.mode][square->duty.step]) {

        gb->apu.channels[idx] = square->envelope.current_volume;
    }
    else
        gb->apu.channels[idx] = 0;
}

static void update_square_sweep(GameBoy *gb) {

    SquareWave *square = &gb->apu.square_waves[0];

    if(square->sweep.period == 0 || square->sweep.shift == 0)
        return;

    uint16_t frequency = square->frequency;
    frequency = frequency + (frequency >> square->sweep.shift);

    if(frequency >= 2048) {
        square->enabled = false;
        return;
    }

    square->frequency = frequency;
    SWRITE8(NR13, (frequency & 0xFF00) >> 8);
    SWRITE8(NR14, (SREAD8(NR14) & ~CHANNEL_FREQUENCY_MSB) | ((frequency >> 8) & CHANNEL_FREQUENCY_MSB));

    frequency = frequency + (frequency >> square->sweep.shift);

    if(frequency >= 2048)
        square->enabled = false;
}

static void trigger_square(GameBoy *gb, const uint8_t idx) {

    assert(idx <= 1);
    SquareWave *square = &gb->apu.square_waves[idx];
    square->enabled = true;

    if(square->length.clock == 0)
        square->length.clock = 64;

    square->clock = (2048 - square->frequency) * 4;
    square->envelope.current_volume = square->envelope.initial_volume;
}

static void read_wave(GameBoy *gb, const uint16_t address, const uint8_t value) {

    Wave *wave = &gb->apu.wave;

    switch(address) {
        
        case NR30:
            wave->enabled = (value & WAVE_ENABLED) >> 7;
            break;

        case NR31:
            wave->length.clock = value;
            break;

        case NR32:
            wave->volume_code = (value & WAVE_VOLUME) >> 5;
            break;

        case NR33:
            wave->frequency &= 0xFF00;
            wave->frequency |= value;
            break;

        case NR34:
            wave->length.enabled = (value & CHANNEL_LENGTH_ENABLE) >> 6;
            wave->frequency = ((value & CHANNEL_FREQUENCY_MSB) << 8) | (wave->frequency & 0xFF);
            if(value & CHANNEL_TRIGGER) trigger_wave(gb);
            break;

        default:
            break;
    }
}

static void update_wave(GameBoy *gb) {

    Wave *wave = &gb->apu.wave;

    if(wave->clock-- <= 0)
    {
        wave->clock = (2048 - wave->frequency) * 2;

        if(wave->position++ == 31)
            wave->position = 0;
    }

    if(wave->enabled && wave->volume_code > 0) {

        uint8_t sample;

        // Top 4 bits
        if(wave->position % 2 == 0)
            sample = (SREAD8(WAVE_TABLE_START + wave->position) & 0xF0) >> 4;

        // Lower 4 bits
        else
            sample = SREAD8(WAVE_TABLE_START + wave->position - 1) & 0xF;

        sample = sample >> (wave->volume_code - 1);
        gb->apu.channels[CHANNEL_WAVE] = sample;
    }
    else
        gb->apu.channels[CHANNEL_WAVE] = 0;
}

static void trigger_wave(GameBoy *gb) {

    Wave *wave = &gb->apu.wave;
    wave->enabled = true;

    if(wave->length.clock == 0)
        wave->length.clock = 256;

    wave->position = 0; 
}

static void read_noise(GameBoy *gb, const uint16_t address, const uint8_t value) {

    Noise *noise = &gb->apu.noise;

    switch(address) {
        
        case NR41:
            noise->length.clock = (value & CHANNEL_LENGTH);
            break;

        case NR42:
            noise->envelope.initial_volume = (value & CHANNEL_ENVELOPE_INITIAL_VOLUME) >> 4;
            noise->envelope.mode = (EnvelopeMode) (value & CHANNEL_ENVELOPE_MODE) >> 3;
            noise->envelope.period = value & CHANNEL_ENVELOPE_PERIOD;
            break;

        case NR43:
            noise->clock_shift = (value & NOISE_CLOCK_SHIFT) >> 4;
            noise->width_mode = (value & NOISE_WIDTH_MODE) >> 3;
            noise->divisor_code = (value & NOISE_DIVISOR_CODE);
            break;

        case NR44:
            noise->length.enabled = (value & CHANNEL_LENGTH_ENABLE) >> 6;
            if(value & CHANNEL_TRIGGER) trigger_noise(gb);
            break;

        default:
            ASSERT_NOT_REACHED();
    }
}

static void update_noise(GameBoy *gb) {

    Noise *noise = &gb->apu.noise;

    // TODO: figure out why this channel is so loud

    if(!noise->enabled)
        return;

    if(noise->clock-- <= 0)
    {
        uint8_t divisor = 0;

        switch(noise->divisor_code) {
            case 0: divisor = 8; break;
            case 1: divisor = 16; break;
            case 2: divisor = 32; break;
            case 3: divisor = 48; break;
            case 4: divisor = 64; break;
            case 5: divisor = 80; break;
            case 6: divisor = 96; break;
            case 7: divisor = 112; break;
        }

        noise->clock = (divisor << noise->clock_shift);

        const uint8_t new_bit = (GET_BIT(noise->lfsr, 1) ^ GET_BIT(noise->lfsr, 0));

        noise->lfsr >>= 1;
        noise->lfsr |= (new_bit << 14);

        if(noise->width_mode == 1) {
            noise->lfsr &= ~(1 << 5);
            noise->lfsr |= (new_bit << 5);
        }

        noise->last_result = !GET_BIT(noise->lfsr, 0);
    }

    gb->apu.channels[CHANNEL_NOISE] = noise->last_result * noise->envelope.current_volume;
}

static void trigger_noise(GameBoy *gb) {

    Noise *noise = &gb->apu.noise;
    noise->enabled = true;

    if(noise->length.clock == 0)
        noise->length.clock = 64;

    noise->envelope.current_volume = noise->envelope.initial_volume;
    noise->lfsr = 0x7FFF;
}
