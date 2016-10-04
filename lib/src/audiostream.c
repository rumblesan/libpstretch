#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "audiostream.h"

#include "bclib/dbg.h"

AudioStream *audio_stream_create(void *source) {

  AudioStream *stream = malloc(sizeof(AudioStream));
  check_mem(stream);
  stream->source      = source;
  stream->finished    = 0;

  return stream;
 error:
  return NULL;
}

void audio_stream_destroy(AudioStream *stream) {
  free(stream);
}
