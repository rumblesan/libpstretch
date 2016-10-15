#ifndef LIBPSTRETCH_STRETCH_H
#define LIBPSTRETCH_STRETCH_H

#include "audiobuffer.h"
#include "fft.h"

typedef struct Stretch {

  int window_size;
  int channels;
  float speed;

  int need_more_audio;
  float input_offset;

  FFT *fft;

  AudioBuffer *input;

  AudioBuffer *old_output;

} Stretch;


Stretch *stretch_create(int channels, int window_size, float ratio);

void stretch_load_samples(Stretch *s, AudioBuffer *audio);
void stretch_load_floats(Stretch *s, float *audio, int channels, int size);

AudioBuffer *stretch_run(Stretch *stretch);
AudioBuffer *stretch_window(Stretch *s);
AudioBuffer *stretch_output(Stretch *s, AudioBuffer *audio);
void stretch_destroy(Stretch *s);

#endif
