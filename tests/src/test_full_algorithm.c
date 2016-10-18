#include "minunit.h"
/*  */
#include "audiobuffer.h"
#include "pstretch.h"

static char * test_full_algorithm() {
  int channels = 2;
  int window_size = 2048;
  float stretch = 10.0;
  long loopmax = 10000;

  Stretch *s = stretch_create(channels, window_size, stretch);
  for (long l = 0; l < loopmax; l += 1) {
    if (s->need_more_audio) {
      AudioBuffer *new_audio = audio_buffer_create(s->channels, s->window_size);
      stretch_load_samples(s, new_audio);
      audio_buffer_destroy(new_audio);
    }
    if (!s->need_more_audio) {
      audio_buffer_destroy(stretch_run(s));
    }
  }

  stretch_destroy(s);

  return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_full_algorithm);

    return NULL;
}

RUN_TESTS(all_tests);
