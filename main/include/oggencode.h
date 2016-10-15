#ifndef LIBPSTRETCH_OGG_ENCODER_H
#define LIBPSTRETCH_OGG_ENCODER_H

#include <stdlib.h>
#include <stdio.h>

#include <vorbis/vorbisenc.h>

typedef struct OggEncoderState {

  ogg_stream_state  os;

  vorbis_info       vi;

  vorbis_dsp_state  vd;
  vorbis_block      vb;

} OggEncoderState;


OggEncoderState *ogg_encoder_state(long channels, long samplerate, float quality);

void cleanup_encoder(OggEncoderState *encoder);

void add_headers(OggEncoderState *encoder, FILE *fp);

int add_audio(OggEncoderState *encoder, long channels, long length, float **audio);

int write_audio(OggEncoderState *encoder, FILE *fp);

#endif
