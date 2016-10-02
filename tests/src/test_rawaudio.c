#include "minunit.h"

#include "rawaudio.h"

static char * test_rawaudio() {
  int channels = 4;
  int size = 2048;
  RawAudio *audio = raw_audio_create(channels, size);
  mu_assert(audio, "Failed to create RawAudio");
  mu_assert(audio->size == size, "Incorrect size");
  mu_assert(audio->channels == channels, "Incorrect channels");
  raw_audio_destroy(audio);
  return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_rawaudio);

    return NULL;
}

RUN_TESTS(all_tests);
