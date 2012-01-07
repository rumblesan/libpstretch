#include <stdlib.h>
#include <sndfile.h>
#include "audiodata.h"


AudioFile read_audio_file(char *filename) {

    AudioFile af = (AudioFile) malloc(sizeof(AudioFile_Data));

    af->filename = filename;
    af->finished = 0;

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    return af;
}

Samples get_audio_data(AudioFile af, int size) {

    int channels = af->info.channels;
    int i,j;

    int buffer_size = size * channels;
    int read_amount;

    float iobuffer[buffer_size];
    read_amount = (int) sf_readf_float(af->sf, iobuffer, size);
    if (read_amount < size) {
        af->finished = 1;
        for (i = read_amount*channels; i < buffer_size; i++) {
            iobuffer[i] = 0.0;
        }
    }

    Samples smps = create_sample_buffer(channels, size);
    for (i = 0; i < channels; i++) {
        for (j = 0; j < size; j++) {
            smps->buffers[i][j] = iobuffer[i+j];
        }
    }

    return smps;

}

void write_wav(AudioFile af) {

    /*
    sf_count_t frame_num = af->info.frames;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);
    if (af->sf == NULL) {
        printf("ERROR OPENING\n");
    } else {
        sf_writef_float(af->sf, af->sound_data, frame_num);
        sf_close(af->sf);
    }
    */
}

void cleanup_audio_file(AudioFile af) {
    sf_close(af->sf);
    free(af);
}

Samples create_sample_buffer(int channels, int size) {
    Samples smps   = (Samples) malloc(sizeof(Samples_Data));
    smps->size     = size;
    smps->channels = channels;

    smps->buffers = (float**) malloc(sizeof(float*) * channels);
    int i;
    for (i = 0; i < channels; i++) {
        smps->buffers[i] = (float*) malloc(sizeof(float*) * size);
    }

    return smps;
}

void cleanup_sample_buffer(Samples smps) {
    int i;
    for (i = 0; i < smps->channels; i++) {
        free(smps->buffers[i]);
    }
    free(smps->buffers);
    free(smps);
}
