/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * Test program for bmp library.
 * It open a bmp file and print bmp info.
 */

#include <stdio.h>
#include "bmp.h"

int main(void)
{
    bmp_handle h0;
    bmp_config config;
    int x, y;
    uint32_t color;
    int rc;

    /* Create bmp and load bmp file */
    rc = bmp_open(&h0, 0);
    rc = bmp_load(h0, "..\\examples\\sample.bmp");

    rc = bmp_get_config(h0, &config);
    printf ("width = %d, height = %d, bit_count = %d\n", config.width, config.height, config.bits_per_pixel);

    bmp_close(h0);

    return 0;
}
