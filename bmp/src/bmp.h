/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * This is a C implementation of bmp library.
 * It provides functions to load/save bmp file and to access each pixel.
 */

#ifndef BMP_H
#define BMP_H

#include <stdint.h>

typedef uint32_t* bmp_handle;

/* Create new bmp_handle (pointer to bmp_data) */
int bmp_open(bmp_handle *h, const char *filename);

/* Release bmp_data and image buffer */
int bmp_close(bmp_handle h);

/* 
 * Functions to access bmp attribute.  Currently it only support 24 bits/pixel.
 * bmp_set_config re-allocate image buffer.
 */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
} bmp_config;

int bmp_set_config(bmp_handle h, bmp_config *config);
int bmp_get_config(bmp_handle h, bmp_config *config);

/* Functions to access each pixel */
int bmp_set_color(bmp_handle h, int x, int y, uint32_t color);
int bmp_get_color(bmp_handle h, int x, int y, uint32_t *color);

int bmp_copy(bmp_handle dst, bmp_handle src);

int bmp_load(bmp_handle h, const char *filename);
int bmp_save(bmp_handle h, const char *filename);

/* Macros to pack/unpack R,G,B to/from uint32_t */
#define RGB_R(rgb) (((rgb) >> 16) & 0xff)
#define RGB_G(rgb) (((rgb) >> 8) & 0xff)
#define RGB_B(rgb) ((rgb) & 0xff)
#define RGB_A(r,g,b) (((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b)

#endif /* BMP_H */
