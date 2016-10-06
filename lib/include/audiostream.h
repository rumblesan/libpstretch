#ifndef LIBPSTRETCH_AUDIOSTREAM_H
#define LIBPSTRETCH_AUDIOSTREAM_H

#include "audiobuffer.h"

typedef struct AudioStream {

  int finished;

  void *source;

} AudioStream;
  
typedef AudioBuffer *(*stream_reader)(AudioStream *stream, int sample_count);

AudioStream *audio_stream_create(void *source);

void audio_stream_destroy(AudioStream *stream);

#endif
