#include "minunit.h"

#include "audiobuffer.h"

static char * test_audiobuffer() {
  int channels = 4;
  int size = 2048;
  AudioBuffer *audio = audio_buffer_create(channels, size);
  mu_assert(audio, "Failed to create AudioBuffer");
  mu_assert(audio->size == size, "Incorrect size");
  mu_assert(audio->channels == channels, "Incorrect channels");
  audio_buffer_destroy(audio);
  return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_audiobuffer);

    return NULL;
}

RUN_TESTS(all_tests);
