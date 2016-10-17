#ifndef LIBPSTRETCH_FFT_H
#define LIBPSTRETCH_FFT_H

#include <fftw3.h>

#include "audiobuffer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct FFT {

    int window_size;
    int rand_seed;

    float *window;

    float *smps;
    float *data;
    float *freq;

    fftwf_plan forward, inverse;

} FFT;

FFT *fft_create (int window_size);
void fft_run(FFT *fft, AudioBuffer *audio);
void fft_window_data(FFT *fft);
void fft_normalise_data(FFT *fft);
void fft_samp_to_freq(FFT *fft);
void fft_pauls_algo(FFT *fft);
void fft_freq_to_samp(FFT *fft);
void fft_cleanup(FFT *fft);

#endif
