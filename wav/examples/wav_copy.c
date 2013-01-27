/*
 * Copyright (C) 2003-2012 Hiroaki Inaba
 *
 * Test program for wav library.
 * It open a wav file and copy it to new wav file.
 */

#include <stdio.h>
#include "wav.h"

int main(void)
{
    wav_handle h0, h1;
    wav_config config;
    int n, ch;
    uint16_t data;

    /* Open and load wav file */
    wav_open(&h0, "..\\examples\\sample.wav");
    wav_get_config(h0, &config);
    printf("channels        = %d\n", config.channels);
    printf("samplehz        = %d\n", config.samplehz);
    printf("bits_per_sample = %d\n", config.bits_per_sample);
    printf("size            = %d\n", config.size);

    wav_open(&h1, 0);
    wav_set_config(h1, &config);

    /* Copy each sample */
    for (ch = 0; ch < config.channels; ch++) {
        for (n = 0; n < config.size; n++) {
            wav_get_data(h0, ch, n, &data);
            wav_set_data(h1, ch, n, data);
        }
    }

    /* Save to a wav file */
    wav_save(h1, "wav_copy.wav");

    return 0;
}
