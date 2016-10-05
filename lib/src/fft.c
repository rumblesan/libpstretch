#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include "fft.h"
#include "rawaudio.h"

#include "bclib/dbg.h"

FFT *fft_create (int window_size) {

  FFT *fft = malloc(sizeof(FFT));
  check_mem(fft);
  fft->window_size = window_size;
  fft->rand_seed   = 1;
  fft->rand_seed  += 161103;


  fft->window = malloc(sizeof(float) * fft->window_size);
  check_mem(fft->window);

  fft->data    = fftwf_malloc(sizeof(float) * fft->window_size);
  check_mem(fft->data);
  fft->freq    = fftwf_malloc(sizeof(float) * fft->window_size / 2+1);
  check_mem(fft->freq);

  fft->forward = fftwf_plan_r2r_1d(
                                   fft->window_size,
                                   fft->data, fft->data,
                                   FFTW_R2HC, FFTW_ESTIMATE
                                   );
  fft->inverse = fftwf_plan_r2r_1d(
                                   fft->window_size,
                                   fft->data, fft->data,
                                   FFTW_HC2R, FFTW_ESTIMATE
                                   );

  int i;
  for (i = 0; i < fft->window_size; i++) {
    fft->window[i] = pow(
                         1.0-pow((i*(2.0/(float)fft->window_size)-1.0),2.0),
                         1.25
                         );
  }

  return fft;
 error:
  log_err("Error creating FFT");
  return NULL;
}

/* Window the incoming audio data.
 * Convert it to frequency bins.
 * Randomise the phase data.
 * Convert back to audio.
 * Normalise and window.
 */
void fft_run(FFT *fft, RawAudio *audio) {
  check(fft, "Need to pass valid fft");
  check(audio, "Need to pass valid audio");
  int i;
  for (i = 0; i < audio->channels; i++) {
    fft->smps = audio->buffers[i];
    fft_window_data(fft);
    fft_samp_to_freq(fft);
    fft_pauls_algo(fft);
    fft_freq_to_samp(fft);
    fft_normalise_data(fft);
    fft_window_data(fft);
  }
  return;
 error:
  log_err("Error with fft run args");
}

void fft_window_data(FFT *fft) {
  check(fft, "Need to pass valid fft");
  int i;
  for (i = 0; i < fft->window_size; i++) {
    fft->smps[i] *= fft->window[i];
  }
  return;
 error:
  log_err("Error with fft window data args");
}

void fft_normalise_data(FFT *fft) {
  check(fft, "Need to pass valid fft");
  int i;
  for (i = 0; i < fft->window_size; i++) {
    fft->smps[i] /= fft->window_size;
  }
  return;
 error:
  log_err("Error with fft window normalise args");
}

void fft_samp_to_freq(FFT *fft) {
  check(fft, "Need to pass valid fft");
  int i;
  for (i = 0; i < fft->window_size; i++) {
    fft->data[i] = fft->smps[i];
  }

  fftwf_execute(fft->forward);

  float c,s;
  for (i = 0; i < fft->window_size/2; i++ ) {
    c = fft->data[i];
    s = fft->data[fft->window_size-i];
    fft->freq[i] = sqrt(c*c+s*s);
  }
  fft->freq[0] = 0.0;
  return;
 error:
  log_err("Error with fft samp to freq");
}

void fft_pauls_algo(FFT *fft) {
  check(fft, "Need to pass valid fft");

  int i;
  float inv_2p15_2pi=1.0/16384.0*M_PI;
  float phase;
  unsigned int rand;

  for (i = 0; i < fft->window_size/2; i++ ) {
    fft->rand_seed = (fft->rand_seed * 1103515245+12345);
    rand=(fft->rand_seed>>16)&0x7fff;
    phase = rand * inv_2p15_2pi;
    fft->data[i] = fft->freq[i]*cos(phase);
    fft->data[fft->window_size-i] = fft->freq[i]*sin(phase);
  }

  fft->data[0] = 0.0;
  fft->data[fft->window_size/2+1] = 0.0;
  fft->data[fft->window_size/2] = 0.0;

  return;
 error:
  log_err("Error with pauls algo");
}

void fft_freq_to_samp(FFT *fft) {
  check(fft, "Need to pass valid fft");

  fftwf_execute(fft->inverse);

  int i;
  for (i = 0; i < fft->window_size; i++) {
    fft->smps[i] = fft->data[i];
  }

  return;
 error:
  log_err("Error with fft freq to samp");
}

void fft_cleanup (FFT *fft) {
  check(fft, "Need to pass valid fft");
  fftwf_destroy_plan(fft->forward);
  fftwf_destroy_plan(fft->inverse);

  free(fft->window);
  fftwf_free(fft->data);
  fftwf_free(fft->freq);

  free(fft);
  return;
 error:
  log_err("Error with fft freq to samp");
}

