/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * Test program for bmp library.
 * It open a bmp file and copy it to new bmp file.
 */

#include <stdio.h>
#include "bmp.h"

int main(void)
{
    bmp_handle h0, h1;
    bmp_config config;
    int x, y;
    uint32_t color, color2;
    int rc;

    /* Create bmp and load bmp file */
    rc = bmp_open(&h0, 0);
    rc = bmp_load(h0, "..\\examples\\sample.bmp");
    rc = bmp_get_config(h0, &config);
    printf ("width = %d, height = %d, bit_count = %d\n", config.width, config.height, config.bits_per_pixel);

    /* Create a new bmp */
    rc = bmp_open(&h1, 0);
    rc = bmp_set_config(h1, &config);

    /* Copy each pixel with degrading each color level */
    for (y = 0; y < config.height; y++)
    {
        for (x = 0; x < config.width; x++)
        {
            rc = bmp_get_color(h0, x, y, &color);
            color2 = RGB_A(RGB_R(color)/2, RGB_G(color)/2, RGB_B(color)/2);
            rc = bmp_set_color(h1, x, y, color2);
        }
    }

    rc = bmp_save(h1, "bmp_copy2.bmp");

    bmp_close(h0);
    bmp_close(h1);

    return 0;
}
