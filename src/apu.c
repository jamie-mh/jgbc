#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "apu.h"

static inline void read_nr1(GameBoy *gb, const uint16_t address, const uint8_t value);
static inline void update_nr1(GameBoy *gb);
static inline void update_nr1_envelope(GameBoy *gb);
static inline void trigger_nr1(GameBoy *gb);


void init_apu(GameBoy *gb) {

    SDL_AudioSpec *desired = &gb->apu.desired_spec;
    SDL_zero(*desired);
    desired->freq = SAMPLE_RATE;
    desired->format = AUDIO_F32SYS;
    desired->channels = AUDIO_CHANNELS;
    desired->samples = AUDIO_SAMPLES;
    desired->userdata = gb;

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

    gb->apu.square_wave_1.enabled = true;
    gb->apu.square_wave_1.dac_enabled = true;
    gb->apu.square_wave_1.frequency = 0;
    gb->apu.square_wave_1.clock = 0;

    gb->apu.square_wave_1.duty.mode = 0;
    gb->apu.square_wave_1.duty.step = 0;

    gb->apu.square_wave_1.envelope.period = 0;
    gb->apu.square_wave_1.envelope.initial_volume = 0;
    gb->apu.square_wave_1.envelope.current_volume = 0;
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
                case 7: // every 8 clocks
                    update_nr1_envelope(gb);
                    break;
            }
        }
        else
            gb->apu.frame_sequencer_clock++;

        update_nr1(gb);

        if(gb->apu.downsample_clock++ >= DOWNSAMPLE_DIVIDER) {
            gb->apu.downsample_clock = 0;

            float volume_left = (SREAD8(NR50) & NR50_VOL_LEFT) >> 4;
            volume_left /= 7;

            float volume_right = SREAD8(NR50) & NR50_VOL_RIGHT;
            volume_right /= 7;

            const uint8_t left = gb->apu.channels[0] * volume_left;
            const uint8_t right = gb->apu.channels[0] * volume_right;

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
            read_nr1(gb, address, value);
            break;

        case NR52:
            gb->apu.enabled = (value & NR52_SND_ENABLED);
    }
}

static inline void read_nr1(GameBoy *gb, const uint16_t address, const uint8_t value) {

    SquareWave1 *nr1 = &gb->apu.square_wave_1;

    switch(address) {
        
        case NR10:
            nr1->sweep.period = (value & NR10_SWEEP) >> 4;
            nr1->sweep.negate = (value & NR10_NEGATE) >> 3;
            nr1->sweep.shift = value & NR10_SHIFT;
            break;

        case NR11:
            nr1->duty.mode = (value & NR11_DUTY_MODE) >> 6;
            nr1->length.initial = value & NR11_LENGTH;
            break;

        case NR12:
            nr1->envelope.initial_volume = (value & NR12_INITIAL_VOLUME) >> 4;
            nr1->envelope.mode = (EnvelopeMode) (value & NR12_ENVELOPE_MODE) >> 3;
            nr1->envelope.period = value & NR12_ENVELOPE_PERIOD;
            nr1->dac_enabled = value & NR12_DAC_ENABLED; // check upper 5 bits for 0
            break;

        case NR13:
            nr1->frequency &= 0xFF00;
            nr1->frequency |= value;
            break;

        case NR14:
            nr1->length.enabled = (value & NR14_LENGTH_ENABLE) >> 6;
            nr1->frequency = ((value & NR14_FREQUENCY_MSB) << 8) | (nr1->frequency & 0xFF);
            if(value & NR14_TRIGGER) trigger_nr1(gb);
            break;
    }
}

static inline void update_nr1(GameBoy *gb) {

    static const bool duty_table[4][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 1 }, // 12.5%
        { 1, 0, 0, 0, 0, 0, 0, 1 }, // 25%
        { 1, 0, 0, 0, 0, 1, 1, 1 }, // 50%
        { 0, 1, 1, 1, 1, 1, 1, 0 }  // 75%
    };

    SquareWave1 *square = &gb->apu.square_wave_1;

    square->clock--;
    if(square->clock <= 0)
    {
        square->clock = (2048 - square->frequency) * 4;
        square->duty.step++;
        square->duty.step &= 0x7;
    }

    if(square->enabled && square->dac_enabled && 
       duty_table[square->duty.mode][square->duty.step]) {

        gb->apu.channels[0] = square->envelope.current_volume;
    }
    else {
        gb->apu.channels[0] = 0;
    }
}

static inline void update_nr1_envelope(GameBoy *gb) {

    SquareWave1 *nr1 = &gb->apu.square_wave_1;

    if(nr1->envelope.period == 0)
        return;

    if(nr1->envelope.current_volume > 15)
        return;

    switch(nr1->envelope.mode) {
        case Increase: nr1->envelope.current_volume++; break;
        case Decrease: nr1->envelope.current_volume--; break;
    }
}

static inline void trigger_nr1(GameBoy *gb) {

    SquareWave1 *nr1 = &gb->apu.square_wave_1;
    nr1->enabled = true;

    if(nr1->length.counter == 0)
        nr1->length.counter = 64;

    nr1->clock = (2048 - nr1->frequency) * 4;
    nr1->envelope.current_volume = nr1->envelope.initial_volume;
}
