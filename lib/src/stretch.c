#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "rawaudio.h"


Stretch stretch_create(int channels,
                       int window_size,
                       float stretch) {

    Stretch s       = malloc(sizeof(StretchData));
    s->window_size  = window_size;
    s->channels     = channels;
    s->speed        = 1.0/stretch;

    s->need_more_audio = 1;
    s->input_offset    = 0.0;
    /*
       create buffers
    */
    s->input      = raw_audio_create(s->channels, s->window_size);
    s->old_output = raw_audio_create(s->channels, s->window_size);
    int i,j;

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            s->input->buffers[i][j] = 0.0;
            s->old_output->buffers[i][j] = 0.0;
        }
    }

    return s;
}

void stretch_add_samples(Stretch s, RawAudio *audio) {

    RawAudio *tmp_samples;
    int i,j;
    int offset = (int) floor(s->input_offset);
    int size   = s->input->size;
    int rem    = size - offset;

    tmp_samples = raw_audio_create(s->channels, rem+audio->size);
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
}

RawAudio *stretch_window(Stretch s) {

    int i, j;
    int offset = (int) floor(s->input_offset);

    RawAudio *audio = raw_audio_create(s->channels, s->window_size);

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            audio->buffers[i][j] = s->input->buffers[i][j+offset];
        }
    }

    float offset_inc = s->speed * ((float)s->window_size * 0.5);
    s->input_offset += offset_inc;

    if (floor(s->input_offset)+offset_inc >= s->window_size) {
        s->need_more_audio = 1;
    }

    return audio;
}

RawAudio *stretch_output(Stretch s, RawAudio *audio) {

    int i,j;
    float data;
    int halfwindow = s->window_size/2;
    RawAudio *output = raw_audio_create(s->channels, halfwindow);
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
}

void stretch_cleanup(Stretch s) {

    raw_audio_destroy(s->input);
    raw_audio_destroy(s->old_output);

    free(s);
}

