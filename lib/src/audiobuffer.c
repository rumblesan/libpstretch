#include <stdlib.h>
#include "audiobuffer.h"

#include "bclib/dbg.h"

AudioBuffer *audio_buffer_create(int channels, int size) {
  AudioBuffer *audio = malloc(sizeof(AudioBuffer));
  check_mem(audio);
  audio->size     = size;
  audio->channels = channels;

  audio->buffers = malloc(sizeof(float*) * channels);
  check_mem(audio->buffers);
  for (int c = 0; c < channels; c++) {
    audio->buffers[c] = malloc(sizeof(float*) * size);
    check_mem(audio->buffers[c]);
  }

  return audio;
 error:
  return NULL;
}

void audio_buffer_destroy(AudioBuffer *audio) {
  check(audio != NULL, "Invalid audio");
  check(audio->buffers != NULL, "Invalid audio buffers");
  for (int c = 0; c < audio->channels; c++) {
    check(audio->buffers[c] != NULL, "Invalid audio buffer #%d", c);
    free(audio->buffers[c]);
  }
  free(audio->buffers);
  free(audio);
  return;
error:
  debug("Error cleaning up audio");
}
