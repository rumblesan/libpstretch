#ifndef LIBPSTRETCH_AUDIOSTREAM_H
#define LIBPSTRETCH_AUDIOSTREAM_H

#include "rawaudio.h"

struct Stretch;

typedef struct AudioStream {

  int finished;

  void *source;

} AudioStream;
  
typedef RawAudio *(*stream_reader)(struct Stretch *s, AudioStream *stream);

AudioStream *audio_stream_create(void *source);

void audio_stream_destroy(AudioStream *stream);

#endif
