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
    desired->callback = &audio_callback;
    desired->userdata = gb;

   gb->apu.device_id = SDL_OpenAudioDevice(
        NULL, 
        0, 
        desired, 
        &gb->apu.actual_spec, 
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
    );

    gb->apu.frame_sequencer_clock = 0;
    gb->apu.downsample_clock = 0;

    gb->apu.buffer.size = gb->apu.actual_spec.size;
    gb->apu.buffer.data = malloc(gb->apu.buffer.size);
    gb->apu.buffer.read_cursor = 0;
    gb->apu.buffer.write_cursor = sizeof(float);

    gb->apu.square_wave_1.enabled = true;
    gb->apu.square_wave_1.dac_enabled = true;
    gb->apu.square_wave_1.frequency = 0;
}

void audio_callback(void *user_data, uint8_t *buffer, int length) {

    GameBoy *gb = (GameBoy *) user_data;
    int region1_size = length;
    int region2_size = 0;

    if(gb->apu.buffer.read_cursor + length > gb->apu.buffer.size)
    {
        // Handle looping back from the beginning.
        region1_size = gb->apu.buffer.size - gb->apu.buffer.read_cursor;
        region2_size = length - region1_size;
    }

    SDL_memcpy(
        buffer,
        (gb->apu.buffer.data + gb->apu.buffer.read_cursor),
        region1_size
    );

    SDL_memcpy(
        &buffer[region1_size],
        gb->apu.buffer.data,
        region2_size
    );

    gb->apu.buffer.read_cursor =
        (gb->apu.buffer.read_cursor + length) % gb->apu.buffer.size;
}

void update_apu(GameBoy *gb) {

    if(!gb->apu.enabled)
        return;

    static unsigned int start;

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

        //uint8_t volume_left = (SREAD8(NR50) & NR50_VOL_LEFT) >> 4;
        //uint8_t volume_right = SREAD8(NR50) & NR50_VOL_RIGHT;

        //const uint8_t amplification = 0x30; // tbd

        // Convert signed sample to unsigned sample
        //gb->apu.buffer[position] = (0xF - gb->apu.channels[0] * 2) * volume_left * amplification;
        //gb->apu.buffer[position + 1] = (0xF - gb->apu.channels[0] * 2) * volume_right * amplification;

        float volume_left = (SREAD8(NR50) & NR50_VOL_LEFT) >> 4;
        //float volume_right = SREAD8(NR50) & NR50_VOL_RIGHT;

        volume_left /= 7;
        //volume_right /= 7;

        const uint8_t left = gb->apu.channels[0] * volume_left;
        //const uint8_t right = gb->apu.channels[0] * volume_right;

        // Normalise
        SDL_LockAudioDevice(gb->apu.device_id);
        gb->apu.buffer.data[gb->apu.buffer.write_cursor / sizeof(float)] = left / 255.0f;
        SDL_UnlockAudioDevice(gb->apu.device_id);

        gb->apu.buffer.write_cursor += sizeof(float);
        gb->apu.buffer.write_cursor %= gb->apu.buffer.size;

        //if(gb->apu.buffer.write_cursor >= gb->apu.buffer.size)
        //    gb->apu.buffer.write_cursor -= gb->apu.buffer.size;

        //gb->apu.buffer.data[cursor + 1] = right / 255.0f;

        //gb->apu.buffer.write_cursor += sizeof(float);
        //gb->apu.buffer.write_cursor += AUDIO_CHANNELS * sizeof(float);
    }

    //if(gb->apu.buffer_position >= AUDIO_CHANNELS * AUDIO_SAMPLES) {

    //    //while(SDL_GetQueuedAudioSize(gb->apu.device_id) > AUDIO_CHANNELS * AUDIO_SAMPLES * sizeof(int16_t)) {
    //    //    SDL_Delay(1);
    //    //}

    //    //for(int i = 0; i < AUDIO_SAMPLES; ++i) {
    //    //    printf("%d ", gb->apu.buffer[i]);
    //    //}

    //    //getchar();

    //    SDL_QueueAudio(gb->apu.device_id, gb->apu.buffer, AUDIO_CHANNELS * AUDIO_SAMPLES * sizeof(float));
    //    gb->apu.buffer_position = 0;
    //    //memset(gb->apu.buffer, 0, AUDIO_CHANNELS * AUDIO_SAMPLES * sizeof(float));
    //}
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


    //  Channel is enabled(see length counter).
    //    If length counter is zero, it is set to 64 (256 for wave channel).
    //    Frequency timer is reloaded with period.
    //    Volume envelope timer is reloaded with period.
    //    Channel volume is reloaded from NRx2.
    //    Noise channel's LFSR bits are all set to 1.
    //    Wave channel's position is set to 0 but sample buffer is NOT refilled.
    //    Square 1's sweep does several things (see frequency sweep).
}
