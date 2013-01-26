/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * Test program for bmp library.
 * It create blank bmp file, then draw a red-rectangle with cross.
 */

#include <stdio.h>
#include "bmp.h"

int main(void)
{
    bmp_handle h;
    bmp_config config;
    int x, y;
    uint32_t color;
    int rc;

    /* Create bmp and set configuration */
    rc = bmp_open(&h, 0);
    config.width = 100;
    config.height = 100;
    config.bits_per_pixel = 24;
    rc = bmp_set_config(h, &config);
    printf ("width = %d, height = %d, bit_count = %d\n", config.width, config.height, config.bits_per_pixel);

    /* Draw a bmp */
    /* vertical line (left) */
    for (y = 0; y < config.height; y++)
        bmp_set_color(h, 0, y, RGB_A(255, 0, 0));

    /* vertical line (right) */
    for (y = 0; y < config.height; y++)
        bmp_set_color(h, config.width-1, y, RGB_A(255, 0, 0));

    /* horizontal line (top) */
    for (x = 0; x < config.height; x++)
        bmp_set_color(h, x, 0, RGB_A(255, 0, 0));

    /* horizontal line (bottom) */
    for (x = 0; x < config.height; x++)
        bmp_set_color(h, x, config.height-1, RGB_A(255, 0, 0));

    /* cross lines */
    for (y = 0, x=0; y < config.height; y++, x++)
        bmp_set_color(h, x, y, RGB_A(255, 0, 0));
    for (y = 0, x=config.width-1; y < config.height; y++, x--)
        bmp_set_color(h, x, y, RGB_A(255, 0, 0));

    rc = bmp_save(h, "bmp_draw.bmp");

    bmp_close(h);

    return 0;
}
