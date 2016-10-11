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

AudioBuffer *audio_buffer_from_float(float *audio, int channels, int size) {
  AudioBuffer *ab = audio_buffer_create(channels, size);
  check_mem(ab);
  for (int i = 0; i < channels; i++) {
    for (int j = 0; j < size; j++) {
      int pos = (j * channels) + i;
      ab->buffers[i][j] = audio[pos];
    }
  }
  return ab;
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
