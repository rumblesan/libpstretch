#ifndef LIBPSTRETCH_RAWAUDIO_H
#define LIBPSTRETCH_RAWAUDIO_H

typedef struct RawAudio {

    int size;
    int channels;

    float **buffers;

} RawAudio;

RawAudio *raw_audio_create(int channels, int size);
void raw_audio_destroy(RawAudio *audio);

#endif
