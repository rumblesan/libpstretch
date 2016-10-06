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
  int i;
  for (i = 0; i < channels; i++) {
    audio->buffers[i] = malloc(sizeof(float*) * size);
    check_mem(audio->buffers[i]);
  }

  return audio;
 error:
  return NULL;
}

void audio_buffer_destroy(AudioBuffer *audio) {
  check(audio != NULL, "Invalid audio");
  check(audio->buffers != NULL, "Invalid audio buffers");
  int i;
  for (i = 0; i < audio->channels; i++) {
    check(audio->buffers[i] != NULL, "Invalid audio buffer #%d", i);
    free(audio->buffers[i]);
  }
  free(audio->buffers);
  free(audio);
  return;
error:
  debug("Error cleaning up audio");
}

