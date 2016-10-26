#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wavstretch.h"
#include "audiofile.h"
#include "ogg_encoder.h"
#include "pstretch.h"

void usage(int exitval) {
    printf("Wave Stretch usage:\n");
    printf("wavstretch -i inputfile -o outputfile [-w WINDOW] [-s STRETCH]\n");
    printf("            WINDOW defaults to 2048\n");
    printf("           STRETCH defaults to 5\n");
    exit(exitval);
}

Args parse_args(int argc, char *argv[]) {

    Args args = {"","",2048,5};

    int c;
    while ( (c = getopt(argc, argv, "i:o:s:w:h")) != -1) {
        switch (c)
        {
            case 'i':
                args.input_file = optarg;
                break;
            case 'o':
                args.output_file = optarg;
                break;
            case 'w':
                args.window_size = atoi(optarg);
                break;
            case 's':
                args.stretch = atof(optarg);
                break;
            case 'h':
                usage(0);
                break;
        }
    }

    if (*args.input_file == '\0') {
        printf("Need to specify an input file\n");
        usage(1);
    } else {
        printf("Stretching %s\n", args.input_file);
    }

    if (*args.output_file == '\0') {
        printf("Need to specify an output file\n");
        usage(1);
    } else {
        printf("Writing to %s\n", args.output_file);
    }

    printf("Using a window of %i samples\n", args.window_size);
    printf("Stretching by %.3f times\n", args.stretch);

    return args;
}


int main (int argc, char *argv[]) {

    Args args = parse_args(argc, argv);

    AudioFile *af = read_audio_file(args.input_file);

    Stretch *stretch = stretch_create(af->info.channels,
                                      args.window_size,
                                      args.stretch);
    AudioBuffer *new_audio;

    FILE *outfile = fopen(args.output_file, "w");
    AudioBuffer *output = NULL;

    OggEncoderState *encoder = ogg_encoder_state(2, 44100, 0.5);

    TrackInfo *info = track_info_create(bfromcstr("Artist"), bfromcstr("Title"));

    set_metadata(encoder, info);
    FileChunk *hfc = file_chunk_create();
    write_headers(encoder, hfc);
    fwrite(hfc->data, hfc->length, sizeof(unsigned char), outfile);
    file_chunk_destroy(hfc);

    FileChunk *next_chunk = NULL;
    while (!(af->finished && stretch->need_more_audio)) {
      if (stretch->need_more_audio) {
        new_audio = get_audio_data(af, stretch->window_size);
        stretch_load_samples(stretch, new_audio);
        audio_buffer_destroy(new_audio);
      }
      if (!stretch->need_more_audio) {
        output = stretch_run(stretch);
        add_audio(encoder, output);
        audio_buffer_destroy(output);
        next_chunk = file_chunk_create();
        write_audio(encoder, next_chunk);
        fwrite(next_chunk->data, next_chunk->length, sizeof(unsigned char), outfile);
        file_chunk_destroy(next_chunk);
        next_chunk = NULL;
      }
    }

    int oggfinished = 0;
    file_finished(encoder);
    while (!oggfinished) {
      next_chunk = file_chunk_create();
      oggfinished = write_audio(encoder, next_chunk);
      fwrite(next_chunk->data, next_chunk->length, sizeof(unsigned char), outfile);
      file_chunk_destroy(next_chunk);
      next_chunk = NULL;
    }

    stretch_destroy(stretch);
    cleanup_audio_file(af);
    cleanup_encoder(encoder);
    fclose(outfile);

    return 0;
}
