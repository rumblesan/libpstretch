#ifndef LIBPSTRETCH_STRETCH_H
#define LIBPSTRETCH_STRETCH_H

#include "rawaudio.h"
#include "audiostream.h"
#include "fft.h"

typedef struct Stretch {

  int window_size;
  int channels;
  float speed;

  int finished;
  int need_more_audio;
  float input_offset;

  FFT *fft;

  stream_reader reader;
  AudioStream *stream;
  int stream_finished;

  RawAudio *input;

  RawAudio *old_output;

} Stretch;


Stretch *stretch_create(int channels,
                        int window_size,
                        float ratio,
                        stream_reader reader,
                        AudioStream *stream
                        );

RawAudio *stretch_run(Stretch *stretch);
void stretch_add_samples(Stretch *s);
RawAudio *stretch_window(Stretch *s);
RawAudio *stretch_output(Stretch *s, RawAudio *audio);
void stretch_destroy(Stretch *s);

#endif

