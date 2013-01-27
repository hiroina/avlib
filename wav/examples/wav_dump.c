/*
 * Copyright (C) 2003-2012 Hiroaki Inaba
 *
 * Test program for wav library.
 * It open a bmp file and print each sample.
 */

#include <stdio.h>
#include "wav.h"

int main(int argc, char* argv[])
{
    wav_handle h;
    wav_config config;
    int n, ch;
    uint16_t data;

    if (argc != 2) {
        printf("usage: wav_dump filename\n");
        return -1;
    }

    wav_open(&h, argv[1]);
    wav_get_config(h, &config);
    printf("channels        = %d\n", config.channels);
    printf("samplehz        = %d\n", config.samplehz);
    printf("bits_per_sample = %d\n", config.bits_per_sample);
    printf("size            = %d\n", config.size);

    for (n = 0; n < config.size; n++) {
        for (ch = 0; ch < config.channels; ch++) {
            wav_get_data(h, ch, n, &data);
            printf("%d  ", data);
        }
        putchar('\n');
    }

    return 0;
}
