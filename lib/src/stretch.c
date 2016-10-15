#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "audiobuffer.h"
#include "fft.h"

#include "bclib/dbg.h"

Stretch *stretch_create(int channels, int window_size, float stretch) {

  Stretch *s      = malloc(sizeof(Stretch));
  check_mem(s);
  s->window_size  = window_size;
  s->channels     = channels;
  s->speed        = 1.0/stretch;

  s->fft          = fft_create(window_size);
  check(s->fft, "Could not create FFT");

  s->need_more_audio = 1;
  s->input_offset    = 0.0;
  /*
    create buffers
  */
  s->input      = audio_buffer_create(s->channels, s->window_size);
  check(s->input, "Could not create input audio buffer");
  s->old_output = audio_buffer_create(s->channels, s->window_size);
  check(s->old_output, "Could not create old_output audio buffer");

  for (int c = 0; c < s->channels; c++) {
    for (int j = 0; j < s->window_size; j++) {
      s->input->buffers[c][j] = 0.0;
      s->old_output->buffers[c][j] = 0.0;
    }
  }

  return s;
 error:
  return NULL;
}

/* Run actual stretching algorithm
 *
 * Select window of audio to stretch
 * Process audio through FFT
 */
AudioBuffer *stretch_run(Stretch *stretch) {
  AudioBuffer *tmp_audio = stretch_window(stretch);
  fft_run(stretch->fft, tmp_audio);
  return stretch_output(stretch, tmp_audio);
}

/* Add samples to the input buffer from an array of floats
 */
void stretch_load_floats(Stretch *s, float *audio, int channels, int size) {
  AudioBuffer *new_input = audio_buffer_from_float(audio, channels, size);
  check(new_input, "Could not create new audio buffer");
  stretch_load_samples(s, new_input);
  return;
 error:
  debug("Error adding samples to stretch");
}

/* Add samples to the input buffer of the stretch struct
 */
void stretch_load_samples(Stretch *s, AudioBuffer *new_audio) {

  AudioBuffer *new_input = NULL;

  int offset   = (int) floor(s->input_offset);
  int size     = s->input->size;
  int rem      = size - offset;

  new_input = audio_buffer_create(s->channels, rem + new_audio->size);
  check(new_input, "Could not create temporary audio buffer");

  for (int c = 0; c < s->channels; c++) {
    memcpy(  new_input->buffers[c],       &(s->input->buffers[c][offset]), rem * sizeof(float));
    memcpy(&(new_input->buffers[c][rem]), new_audio->buffers[c],           new_audio->size * sizeof(float));
  }

  audio_buffer_destroy(s->input);
  s->input           = new_input;
  s->input_offset   -= floor(s->input_offset);
  if (s->input->size > s->window_size) {
    s->need_more_audio = 0;
  }

  return;
 error:
  debug("Error adding samples to stretch");
}

/* Get the next section of audio to be stretched
 * The starting offset is incremented small amounts each time
 */
AudioBuffer *stretch_window(Stretch *s) {

  int offset = (int) floor(s->input_offset);

  AudioBuffer *audio = audio_buffer_create(s->channels, s->window_size);
  check(audio, "Could not create stretch window audio");

  for (int c = 0; c < s->channels; c++) {
    memcpy(audio->buffers[c], &(s->input->buffers[c][offset]), s->window_size * sizeof(float));
  }

  float offset_inc = s->speed * ((float)s->window_size * 0.5);
  s->input_offset += offset_inc;

  if ((s->input->size - ceil(s->input_offset)) <= s->window_size) {
    s->need_more_audio = 1;
  }

  return audio;
 error:
  return NULL;
}

/* Combine the first half of newly stretch audio with last half of
 * previous stretch. Done to overlap windows correctly.
 * Save last half of newly stretched audio and return combined buffer
 */
AudioBuffer *stretch_output(Stretch *s, AudioBuffer *audio) {

  int halfwindow = s->window_size/2;

  AudioBuffer *output = audio_buffer_create(s->channels, halfwindow);
  check(output, "Could not create stretch output audio");

  for (int c = 0; c < s->channels;c++) {
    for (int j = 0; j < halfwindow;j++) {
      float data  = s->old_output->buffers[c][j+halfwindow];
      data += audio->buffers[c][j];
      output->buffers[c][j] = data;
    }
  }
  audio_buffer_destroy(s->old_output);
  s->old_output = audio;
  return output;
 error:
  return NULL;
}

void stretch_destroy(Stretch *s) {

  check(s, "Must provide valid stretch structure");
  fft_cleanup(s->fft);
  audio_buffer_destroy(s->input);
  audio_buffer_destroy(s->old_output);

  free(s);
  return;
 error:
  debug("Error cleaning up stretch");
}
