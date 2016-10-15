#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wavstretch.h"
#include "audiofile.h"
#include "oggencode.h"
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
    add_headers(encoder, outfile);

    int oggfinished = 0;
    while (!(af->finished && stretch->need_more_audio && !oggfinished)) {
      if (stretch->need_more_audio) {
        new_audio = get_audio_data(af, stretch->window_size);
        stretch_load_samples(stretch, new_audio);
      }
      if (!stretch->need_more_audio) {
        output = stretch_run(stretch);
        add_audio(encoder, output->size, output->buffers);
        write_audio(encoder, outfile);
      }
    }

    stretch_destroy(stretch);
    cleanup_audio_file(af);
    cleanup_encoder(encoder);

    return 0;
}
