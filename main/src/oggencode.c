#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>

#include "oggencode.h"

#include "bclib/dbg.h"

#define READ 1024
signed char readbuffer[READ*4+44]; /* out of the data segment, not the stack */

OggEncoderState *ogg_encoder_state(long channels, long samplerate, float quality) {
  OggEncoderState *encoder = malloc(sizeof(OggEncoderState));
  check_mem(encoder);

  vorbis_info_init(&(encoder->vi));

  int initerr = vorbis_encode_init_vbr(&(encoder->vi), channels, samplerate, quality);
  check(!initerr, "Error initialising encoder");

  /* set up the analysis state and auxiliary encoding storage */
  vorbis_analysis_init(&(encoder->vd),&(encoder->vi));
  vorbis_block_init(&(encoder->vd),&(encoder->vb));
  return encoder;
 error:
  return NULL;
}

void add_headers(OggEncoderState *encoder, FILE *fp) {

  vorbis_comment vc;

  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;

  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc, "ENCODER", "encoder_example.c");
  vorbis_comment_add_tag(&vc, "ARTIST", "Rumblesan");
  vorbis_comment_add_tag(&vc, "TITLE", "More Efficient");

  ogg_stream_init(&(encoder->os), rand());

  /* Vorbis streams begin with three headers; the initial header (with
     most of the codec setup parameters) which is mandated by the Ogg
     bitstream spec.  The second header holds any comment fields.  The
     third header holds the bitstream codebook.  We merely need to
     make the headers, then pass them to libvorbis one at a time;
     libvorbis handles the additional Ogg bitstream constraints */

  vorbis_analysis_headerout(&(encoder->vd), &vc, &header, &header_comm, &header_code);

  ogg_stream_packetin(&(encoder->os), &header);
  ogg_stream_packetin(&(encoder->os), &header_comm);
  ogg_stream_packetin(&(encoder->os), &header_code);

  ogg_page output_page;
  while(1) {
    int result = ogg_stream_flush(&(encoder->os), &output_page);
    if(result == 0) break;
    fwrite(output_page.header, 1, output_page.header_len, fp);
    fwrite(output_page.body, 1, output_page.body_len, fp);
  }

  return;
}

int write_audio(OggEncoderState *encoder, long samplespc, float **audio, FILE *fp) {
  int finished = 0;

  float **buffer=vorbis_analysis_buffer(&(encoder->vd),READ);

  for (long t = 0; t < samplespc; t+= 1) {
    buffer[0][t] = audio[0][t];
    buffer[1][t] = audio[1][t];
  }
  free(audio[0]);
  free(audio[1]);
  free(audio);

  /* tell the library how much we actually submitted */
  vorbis_analysis_wrote(&(encoder->vd), samplespc);

  /* vorbis does some data preanalysis, then divvies up blocks for
     more involved (potentially parallel) processing.  Get a single
     block for encoding now */

  while(vorbis_analysis_blockout(&(encoder->vd), &(encoder->vb)) == 1){

    /* analysis, assume we want to use bitrate management */
    vorbis_analysis(&(encoder->vb),NULL);
    vorbis_bitrate_addblock(&(encoder->vb));

    ogg_packet *op = malloc(sizeof(ogg_packet));
    check_mem(op);

    while(vorbis_bitrate_flushpacket(&(encoder->vd), op)) {

      /* weld the packet into the bitstream */
      ogg_stream_packetin(&(encoder->os), op);

      /* write out pages (if any) */
      while(!finished) {
        ogg_page *og = malloc(sizeof(ogg_page));
        check_mem(og);
        int result = ogg_stream_pageout(&(encoder->os), og);

        if(result == 0) {
          free(og);
          break;
        }
        fwrite(og->header, 1, og->header_len, fp);
        fwrite(og->body, 1, og->body_len, fp);

        /* this could be set above, but for illustrative purposes, I do
           it here (to show that vorbis does know where the stream ends) */

        if(ogg_page_eos(og)) finished = 1;
        free(og);
      }
    }

    free(op);
    op = NULL;
  }

  return finished;
 error:
  return -1;
}

void cleanup_encoder(OggEncoderState *encoder) {
  ogg_stream_clear(&(encoder->os));
  vorbis_block_clear(&(encoder->vb));
  vorbis_dsp_clear(&(encoder->vd));
  vorbis_info_clear(&(encoder->vi));
  free(encoder);
}


