#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "rawaudio.h"
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
  s->input      = raw_audio_create(s->channels, s->window_size);
  check(s->input, "Could not create input audio buffer");
  s->old_output = raw_audio_create(s->channels, s->window_size);
  check(s->old_output, "Could not create old_output audio buffer");
  int i,j;

  for (i = 0; i < s->channels; i++) {
    for (j = 0; j < s->window_size; j++) {
      s->input->buffers[i][j] = 0.0;
      s->old_output->buffers[i][j] = 0.0;
    }
  }

  return s;
 error:
  return NULL;
}

RawAudio *stretch_run(Stretch *stretch) {
  if (stretch->need_more_audio) {
    stretch_add_samples(stretch);
  }
  RawAudio *tmp_audio = stretch_window(stretch);
  fft_run(stretch->fft, tmp_audio);
  return stretch_output(stretch, tmp_audio);
}

void stretch_add_samples(Stretch *s) {

  RawAudio *tmp_samples;
  int i,j;
  int offset = (int) floor(s->input_offset);
  int size   = s->input->size;
  int rem    = size - offset;

  if (s->stream->finished) return;
  RawAudio *audio = s->reader(s, s->stream);
  if (s->stream->finished) {
    log_info("Stream is finished");
    s->stream_finished = 1;
  }
  tmp_samples = raw_audio_create(s->channels, rem+audio->size);
  check(tmp_samples, "Could not create temporary audio buffer");
  for (i = 0; i < s->channels; i++) {
    for (j = 0; j < rem; j++) {
      tmp_samples->buffers[i][j] = s->input->buffers[i][j+offset];
    }
    for (j = 0; j < audio->size; j++) {
      tmp_samples->buffers[i][j+rem] = audio->buffers[i][j];
    }
  }

  raw_audio_destroy(s->input);
  s->input           = tmp_samples;
  s->input_offset   -= floor(s->input_offset);
  s->need_more_audio = 0;

  raw_audio_destroy(audio);
  return;
 error:
  debug("Error adding samples to stretch");
}

RawAudio *stretch_window(Stretch *s) {

  int i, j;
  int offset = (int) floor(s->input_offset);

  RawAudio *audio = raw_audio_create(s->channels, s->window_size);
  check(audio, "Could not create stretch window audio");
  for (i = 0; i < s->channels; i++) {
    for (j = 0; j < s->window_size; j++) {
      audio->buffers[i][j] = s->input->buffers[i][j+offset];
    }
  }

  float offset_inc = s->speed * ((float)s->window_size * 0.5);
  s->input_offset += offset_inc;

  if (floor(s->input_offset)+offset_inc >= s->window_size) {
    s->need_more_audio = 1;
    log_info("Need more audio");
    if (s->stream_finished) {
      log_info("Stream also finished");
      s->finished = 1;
    }
  }

  return audio;
 error:
  return NULL;
}

RawAudio *stretch_output(Stretch *s, RawAudio *audio) {

  int i,j;
  float data;
  int halfwindow = s->window_size/2;
  RawAudio *output = raw_audio_create(s->channels, halfwindow);
  check(output, "Could not create stretch output audio");
  for (i = 0; i < s->channels;i++) {
    for (j = 0; j < halfwindow;j++) {
      data  = s->old_output->buffers[i][j+halfwindow];
      data += audio->buffers[i][j];
      output->buffers[i][j] = data;
    }
  }
  raw_audio_destroy(s->old_output);
  s->old_output = audio;
  return output;
 error:
  return NULL;
}

void stretch_destroy(Stretch *s) {

  check(s, "Must provide valid stretch structure");
  fft_cleanup(s->fft);
  raw_audio_destroy(s->input);
  raw_audio_destroy(s->old_output);

  free(s);
  return;
 error:
  debug("Error cleaning up stretch");
}

