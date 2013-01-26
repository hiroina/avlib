/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * Test program for bmp library.
 * It prints color of each pixel.
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

    rc = bmp_open(&h0, "a.bmp");

    rc = bmp_get_config(h0, &config);
    printf ("width = %d, height = %d, bit_count = %d\n", config.width, config.height, config.bits_per_pixel);

    for (y = 0; y < config.height; y++)
    {
        for (x = 0; x < config.width; x++)
        {
            rc = bmp_get_color(h0, x, y, &color);
            printf("(%d, %d) = %d\n", x, y, color);
        }
    }
    bmp_close(h0);

    return 0;
}
