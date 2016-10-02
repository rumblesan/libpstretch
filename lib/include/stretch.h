#ifndef LIBPSTRETCH_STRETCH_H
#define LIBPSTRETCH_STRETCH_H

#include "rawaudio.h"

typedef struct Stretch {

    int window_size;
    int channels;
    float speed;

    int need_more_audio;
    float input_offset;

    RawAudio *input;

    RawAudio *old_output;

} Stretch;


//typedef RawAudio *(*stream_reader)(void *stream);

Stretch *stretch_create(int channels,
                        int window_size,
                        float ratio);

void stretch_add_samples(Stretch *s, RawAudio *audio);
RawAudio *stretch_window(Stretch *s);
RawAudio *stretch_output(Stretch *s, RawAudio *audio);
void stretch_destroy(Stretch *s);

#endif

