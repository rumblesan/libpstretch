#ifndef __SLOW_RADIO_OGG_ENCODER__
#define __SLOW_RADIO_OGG_ENCODER__

#include <vorbis/vorbisenc.h>

#include "pstretch/audiobuffer.h"
#include "filechunk.h"

#include "bclib/bstrlib.h"

typedef struct TrackInfo {
  bstring artist;
  bstring title;
} TrackInfo;

TrackInfo *track_info_create(bstring artist, bstring title);
void track_info_destroy(TrackInfo *info);

typedef struct OggEncoderState {

  ogg_stream_state  os;

  vorbis_comment    vc;

  vorbis_info       vi;

  vorbis_dsp_state  vd;
  vorbis_block      vb;

} OggEncoderState;

OggEncoderState *ogg_encoder_state(long channels, long samplerate, float quality);

void cleanup_encoder(OggEncoderState *encoder);

void set_metadata(OggEncoderState *encoder, TrackInfo *info);

int write_headers(OggEncoderState *encoder, FileChunk *chunk);

int add_audio(OggEncoderState *encoder, AudioBuffer *audio);

int file_finished(OggEncoderState *encoder);

int write_audio(OggEncoderState *encoder, FileChunk *chunk);

#endif
