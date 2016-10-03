#ifndef LIBPSTRETCH_AUDIOFILE_H
#define LIBPSTRETCH_AUDIOFILE_H

#include <sndfile.h>
#include "rawaudio.h"

typedef struct audio_file *AudioFile;

typedef struct audio_file {
    SNDFILE *sf;
    char *filename;
    SF_INFO info;
    int finished;
} AudioFile_Data;

AudioFile read_audio_file(char *filename);
AudioFile write_audio_file(char *filename,
                           int samplerate,
                           int channels,
                           int format);
RawAudio *get_audio_data(AudioFile af, int size);
void write_audio_data(AudioFile af, RawAudio *audio);
void cleanup_audio_file(AudioFile af);

#endif
