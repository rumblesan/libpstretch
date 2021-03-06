#include <stdlib.h>
#include <sndfile.h>
#include "audiofile.h"
#include "pstretch.h"


AudioFile *read_audio_file(char *filename) {

    AudioFile *af = malloc(sizeof(AudioFile));

    af->filename = filename;
    af->finished = 0;

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    return af;
}

AudioFile *write_audio_file(char *filename,
                            int samplerate,
                            int channels,
                            int format) {

    AudioFile *af = malloc(sizeof(AudioFile));

    af->filename = filename;

    af->info.samplerate = samplerate;
    af->info.channels   = channels;
    af->info.format     = format;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);

    return af;
}

AudioBuffer *get_audio_data(AudioFile *af, int size) {

    if (af->finished) return NULL;

    int channels = af->info.channels;
    int i,j;

    int buffer_size = size * channels;
    int read_amount;
    int pos;

    float iobuffer[buffer_size];
    read_amount = (int) sf_readf_float(af->sf, iobuffer, size);
    if (read_amount < size) {
        af->finished = 1;
        for (i = read_amount*channels; i < buffer_size; i++) {
            iobuffer[i] = 0.0;
        }
    }

    AudioBuffer *audio = audio_buffer_create(channels, size);
    for (i = 0; i < channels; i++) {
        for (j = 0; j < size; j++) {
            pos = (j * channels) + i;
            audio->buffers[i][j] = iobuffer[pos];
        }
    }

    return audio;

}

void write_audio_data(AudioFile *af, AudioBuffer *audio) {

    int i,j;
    int pos;
    int buffer_size = audio->channels * audio->size;
    float iobuffer[buffer_size];
    for (i = 0; i < audio->channels; i++) {
        for (j = 0; j < audio->size; j++) {
            pos = (j * audio->channels) + i;
            iobuffer[pos] = audio->buffers[i][j];
        }
    }

    sf_writef_float(af->sf, iobuffer, audio->size);

    audio_buffer_destroy(audio);
}

void cleanup_audio_file(AudioFile *af) {
    sf_close(af->sf);
    free(af);
}


