#ifndef LIBPSTRETCH_AUDIO_BUFFER_H
#define LIBPSTRETCH_AUDIO_BUFFER_H

typedef struct AudioBuffer {

    int size;
    int channels;

    float **buffers;

} AudioBuffer;

AudioBuffer *audio_buffer_create(int channels, int size);
AudioBuffer *audio_buffer_from_float(float *audio, int channels, int size);
void audio_buffer_destroy(AudioBuffer *audio);

#endif
