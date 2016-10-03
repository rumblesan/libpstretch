#include <stdlib.h>
#include "rawaudio.h"

#include "bclib/dbg.h"

RawAudio *raw_audio_create(int channels, int size) {
  RawAudio *audio = malloc(sizeof(RawAudio));
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

void raw_audio_destroy(RawAudio *audio) {
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

