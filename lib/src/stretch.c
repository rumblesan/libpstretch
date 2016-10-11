#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "audiobuffer.h"
#include "fft.h"

#include "bclib/dbg.h"

Stretch *stretch_create(int channels,
                        int window_size,
                        float stretch,
                        stream_reader reader,
                        AudioStream *stream
                        ) {

  Stretch *s      = malloc(sizeof(Stretch));
  check_mem(s);
  s->window_size  = window_size;
  s->channels     = channels;
  s->speed        = 1.0/stretch;

  s->fft          = fft_create(window_size);
  check(s->fft, "Could not create FFT");

  s->reader = reader;
  s->stream = stream;
  s->stream_finished = 0;

  s->need_more_audio = 1;
  s->finished = 0;
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
 * Get audio from stream when necessary
 * Select window of audio to stretch
 * Process audio through FFT
 */
AudioBuffer *stretch_run(Stretch *stretch) {
  if (stretch->need_more_audio) {
    stretch_read_samples(stretch);
  }
  AudioBuffer *tmp_audio = stretch_window(stretch);
  fft_run(stretch->fft, tmp_audio);
  return stretch_output(stretch, tmp_audio);
}

/* Retrieve required number of samples from audio input stream
 * add it to the input buffer of the stretch struct
 */
void stretch_read_samples(Stretch *s) {
  if (s->stream->finished) return;
  AudioBuffer *audio = s->reader(s->stream, s->window_size);
  if (s->stream->finished) {
    log_info("Stream is finished");
    s->stream_finished = 1;
  }
  stretch_load_samples(s, audio);
}

/* Add samples to the input buffer of the stretch struct
 */
void stretch_load_samples(Stretch *s, AudioBuffer *audio) {

  AudioBuffer *tmp_samples;
  int offset = (int) floor(s->input_offset);
  int size   = s->input->size;
  int rem    = size - offset;

  tmp_samples = audio_buffer_create(s->channels, rem+audio->size);
  check(tmp_samples, "Could not create temporary audio buffer");
  for (int c = 0; c < s->channels; c++) {
    for (int j = 0; j < rem; j++) {
      tmp_samples->buffers[c][j] = s->input->buffers[c][j+offset];
    }
    for (int k = 0; k < audio->size; k++) {
      tmp_samples->buffers[c][k+rem] = audio->buffers[c][k];
    }
  }

  audio_buffer_destroy(s->input);
  s->input           = tmp_samples;
  s->input_offset   -= floor(s->input_offset);
  if (floor(s->input_offset) < s->window_size) {
    s->need_more_audio = 0;
  }

  audio_buffer_destroy(audio);
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
    for (int j = 0; j < s->window_size; j++) {
      audio->buffers[c][j] = s->input->buffers[c][j+offset];
    }
  }

  float offset_inc = s->speed * ((float)s->window_size * 0.5);
  s->input_offset += offset_inc;

  if (floor(s->input_offset)+offset_inc >= s->window_size) {
    s->need_more_audio = 1;
    if (s->stream_finished) {
      log_info("Stream finished");
      s->finished = 1;
    }
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
