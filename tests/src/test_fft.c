#include "minunit.h"

#include "fft.h"

static char * test_fft() {
  int window_size = 2048;

  FFT *fft = fft_create(window_size);
  mu_assert(fft, "Failed to create FFT");
  mu_assert(fft->window_size == window_size, "Incorrect window size");
  fft_cleanup(fft);
  return 0;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_fft);

  return NULL;
}

RUN_TESTS(all_tests);
