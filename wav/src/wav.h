/*
 * Copyright (C) 2003-2012 Hiroaki Inaba
 *
 * This is a C implementation of wav library.
 * It provides functions to load/save wav file and to access each sample.
 */

#ifndef WAV_H
#define WAV_H

#include <stdint.h>
typedef uint32_t* wav_handle;

/* Create new wav_handle (pointer to wav_data) */
int wav_open(wav_handle *h, const char *filename);

/* Release wav_data and image buffer */
int wav_close(wav_handle h);

/* 
 * Functions to access wav attribute.  Currently it only support 24 bits/pixel.
 * bmp_set_config re-allocate image buffer.
 */
typedef struct {
    uint32_t channels;
    uint32_t samplehz;
    uint32_t bits_per_sample;
    uint32_t size;
} wav_config;

int wav_set_config(wav_handle h, wav_config *config);
int wav_get_config(wav_handle h, wav_config *config);

/* Functions to access each pixel */
int wav_set_data(wav_handle h, int ch, int n, uint16_t data);
int wav_get_data(wav_handle h, int ch, int n, uint16_t *data);

int wav_copy(wav_handle dst, wav_handle src);

int wav_load(wav_handle h, const char *filename);
int wav_save(wav_handle h, const char *filename);

#endif	/* WAV_H */
