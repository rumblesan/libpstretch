#include "minunit.h"

#include "audiostream.h"

static char * test_audio_stream() {
  void *source = NULL;
  AudioStream *stream = audio_stream_create(source);
  mu_assert(stream, "Failed to create Audio Stream");
  audio_stream_destroy(stream);
  return 0;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_audio_stream);

    return NULL;
}

RUN_TESTS(all_tests);
