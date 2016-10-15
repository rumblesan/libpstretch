#ifndef LIBPSTRETCH_OGG_ENCODER_H
#define LIBPSTRETCH_OGG_ENCODER_H

#include <stdlib.h>
#include <stdio.h>

#include <vorbis/vorbisenc.h>

typedef struct OggEncoderState {

  ogg_stream_state  os;

  vorbis_info       vi;

  vorbis_comment    vc;

  vorbis_dsp_state  vd;
  vorbis_block      vb;

} OggEncoderState;


OggEncoderState *ogg_encoder_state();

int setup_encoder(OggEncoderState *encoder, int channels);

void cleanup_encoder(OggEncoderState *encoder);

void add_headers(OggEncoderState *encoder, FILE *fp);

int write_audio(OggEncoderState *encoder, long length, float **audio, FILE *fp);

#endif
