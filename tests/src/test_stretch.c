#include "minunit.h"

#include "stretch.h"
#include "audiobuffer.h"

static char * test_stretch() {
  int channels = 4;
  int window_size = 2048;
  float stretch = 5.0;
  Stretch *s = stretch_create(channels, window_size, stretch);
  mu_assert(s, "Failed to create Stretch");
  mu_assert(s->window_size == window_size, "Incorrect window size");
  mu_assert(s->channels == channels, "Incorrect channels");
  stretch_destroy(s);
  return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_stretch);

    return NULL;
}

RUN_TESTS(all_tests);
