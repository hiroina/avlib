/*
 * Copyright (C) 2003-2012 Hiroaki Inaba
 *
 * This is a C implementation of wav library.
 * It provides functions to load/save wav file and to access each sample.
 */

#include <stdio.h>
#include "wav.h"

/*
 * WAVE related structure
 */
#define CHUNK_ID(a0, a1, a2, a3)	((uint32_t)(a3) << 24 | (uint32_t)(a2) << 16 | (uint32_t)(a1) << 8 | (uint32_t)(a0))

#pragma pack(1)
typedef struct {
  uint16_t wFormatTag;			// 1 for PCM
  uint16_t nChannels;			// Number of channels
  uint32_t nSamplesPerSec;		// Sampling Frequency (typ. 44100, 48000)
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
} PCMWAVEFORMAT;
#pragma pack()

/*
 * wav internal data
 */
typedef struct {
    uint8_t *image;
    uint32_t image_size;
    wav_config config;
} wav_data;

/*
 * private functions
 */

/* return image buffer size that needs in wav_data->image */
static uint32_t wav_p_image_size(wav_config *config)
{
    uint32_t size;
    size = (config->channels * (config->bits_per_sample/8) * config->size);

    return size;
}

/* return image buffer size that needs in wav_data->image */
static void wav_p_release_image(wav_data *wav)
{
    if (wav->image)
        free(wav->image);

    wav->image = 0;
    wav->image_size = 0;
    wav->config.channels = 0;
    wav->config.samplehz = 0;
    wav->config.bits_per_sample = 0;

    return;
}

/*
 * Public functions
 */

/* Create new wav_handle (pointer to wav_data) */
int wav_open(wav_handle *h, const char *filename)
{
    wav_data *wav;
    int rc = 0;

    /* check argument */
    if (h == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    wav = (wav_data*)malloc(sizeof(wav_data));
    if (wav)
    {
        memset(wav, 0x00, sizeof(wav_data));
        *h = (wav_handle)wav;

        if (filename)
            rc = wav_load(*h, filename);
    }
    else
    {
        rc = -1;
    }

    return rc;
}

/* Release wav_data and image buffer */
int wav_close(wav_handle h)
{
    wav_data *wav = (wav_data *)h;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    /* release all resources */
    wav_p_release_image(wav);
    free(wav);

    return 0;
}

/* Set new config and re-allocate image buffer */
int wav_set_config(wav_handle h, wav_config *config)
{
    wav_data *wav = (wav_data *)h;
    int rc = 0;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (config == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid argument\n");
        return -1;
    }

    /* free old wav buffer */
    wav_p_release_image(wav);

    /* copy config and allocate new buffer */
    wav->config = *config;
    wav->image_size = wav_p_image_size(config);
    wav->image = (uint8_t*)malloc(wav->image_size);
    if (wav->image == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Can't allocate wav buffer\n");
        memset(wav, 0x00, sizeof(wav_data));
        rc = -1;
    }
    memset(wav->image, 0x00, wav->image_size);

    return rc;
}

int wav_get_config(wav_handle h, wav_config *config)
{
    wav_data *wav = (wav_data *)h;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (config == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid argument\n");
        return -1;
    }

    /* return data */
    *config = wav->config;

    return 0;
}

/* Functions to access each sample */
int wav_set_data(wav_handle h, int ch, int n, uint16_t data)
{
    wav_data *wav = (wav_data *)h;
    int rc = 0;
    int bytes_per_sample;
    uint16_t sample;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if ((n < 0) || (wav->config.size -1 < n))
    {
        fprintf(stderr, __FUNCTION__ ": Error n=%d is out of range. It must be within [0, %d]\n", n, wav->config.size-1);
        return -1;
    }
    if ((ch < 0) || (wav->config.channels -1 < ch))
    {
        fprintf(stderr, __FUNCTION__ ": Error ch=%d is out of range. It must be within [0, %d]\n", ch, wav->config.channels-1);
        return -1;
    }

    bytes_per_sample = wav->config.bits_per_sample/8;

    if (bytes_per_sample == 1)
        *(wav->image + (wav->config.channels*n+ch) * bytes_per_sample) = data;
    else if (bytes_per_sample == 2)
        *((uint16_t*)(wav->image + (wav->config.channels*n+ch) * bytes_per_sample)) = data;
    else
    {
        rc = -1;
        fprintf(stderr, __FUNCTION__ ": Error invalid bytes_per_sample\n");
    }

    return rc;
}

int wav_get_data(wav_handle h, int ch, int n, uint16_t *data)
{
    wav_data *wav = (wav_data *)h;
    int rc = 0;
    int bytes_per_sample;
    uint16_t sample;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (data == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid parameter\n");
        return -1;
    }
    if ((n < 0) || (wav->config.size -1 < n))
    {
        fprintf(stderr, __FUNCTION__ ": Error n=%d is out of range. It must be within [0, %d]\n", n, wav->config.size-1);
        return -1;
    }
    if ((ch < 0) || (wav->config.channels -1 < ch))
    {
        fprintf(stderr, __FUNCTION__ ": Error ch=%d is out of range. It must be within [0, %d]\n", ch, wav->config.channels-1);
        return -1;
    }

    /*
      ch=1, b=8:	0, 1, 2, 3, 4, 5, ...							:
      ch=2, b=8:	0:0, 1:0, 0:1, 1:1, 0:2, 1:2, 0:3, 1:3, ...		:
      ch=1, b=16: 0L, 0H, 1L, 1H, 2L, 2H, ...						:
      ch=2, b=16: 0:0L, 0:0H, 1:0L, 1:0H, 0:1L, 0:1H, 1:1L, 1:1H,	: data[(m_channels*n+ch) * (m_samplebits/8)]
    */

    bytes_per_sample = wav->config.bits_per_sample/8;

    if (bytes_per_sample == 1)
        sample = *(wav->image + (wav->config.channels*n+ch) * bytes_per_sample);
    else if (bytes_per_sample == 2)
        sample = *((uint16_t*)(wav->image + (wav->config.channels*n+ch) * bytes_per_sample));
    else
    {
        rc = -1;
        fprintf(stderr, __FUNCTION__ ": Error invalid bytes_per_sample\n");
    }

    *data = sample;

    return rc;
}

int wav_copy(wav_handle dst, wav_handle src)
{
    wav_data *wav_dst = (wav_data *)dst;
    wav_data *wav_src = (wav_data *)src;
    int rc = 0;

    /* check argument */
    if (wav_dst == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (wav_src == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    rc = wav_set_config(dst, &wav_src->config);
    if (rc == 0)
        memcpy(wav_dst->image, wav_src->image, wav_dst->image_size);

    return rc;
}

int wav_load(wav_handle h, const char *filename)
{
    wav_data *wav = (wav_data *)h;
    int rc = 0;
    wav_config new_config;
    FILE *fp;
    int i, len;
    uint32_t data, chunkSize;
    PCMWAVEFORMAT pwf;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (filename == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return -1;
    }
    /* 'RIFF' */
    len = fread(&data, 4, 1, fp);
    if (data != CHUNK_ID('R', 'I', 'F', 'F')) {
        fprintf(stderr, __FUNCTION__ ": Can't find \"RIFF\"\n");
        rc = -1;
        goto exit;
    }

    /* chunkSize */
    len = fread(&data, 4, 1, fp);

    /* 'WAVE' */
    len = fread(&data, 4, 1, fp);
    if (data != CHUNK_ID('W', 'A', 'V', 'E')) {
        fprintf(stderr, __FUNCTION__ ": Can't find \"WAVE\"\n");
        rc = -1;
        goto exit;
    }

    /* 'fmt ' */
    len = fread(&data, 4, 1, fp);
    if (data != CHUNK_ID('f', 'm', 't', ' ')) {
        fprintf(stderr, __FUNCTION__ ": Can't find \"fmt \"\n");
        rc = -1;
        goto exit;
    }

    /* chunkSize */
    len = fread(&chunkSize, 4, 1, fp);

    // PCMWAVEFORMAT
    len = fread(&pwf, sizeof(PCMWAVEFORMAT), 1, fp);
    if (pwf.wFormatTag != 1) {
        fprintf(stderr, __FUNCTION__ ": WAVEFORMAT.wFormatTag != 1(PCM)\n");
        rc = -1;
        goto exit;
    }

    /* Read and throw Extend Data (should be able to replace with seek) */
    for (i = 0; i < chunkSize - 16; i++)
        len = fread((char*)&data, 1, 1, fp);

    /* 'data' */
    len = fread(&data, 4, 1, fp);
    if (data != CHUNK_ID('d', 'a', 't', 'a')) {
        fprintf(stderr, __FUNCTION__ ": Can't find 'data'\n");
        rc = -1;
        goto exit;
    }

    /* chunkSize */
    len = fread(&chunkSize, 4, 1, fp);

    new_config.channels = pwf.nChannels;
    new_config.samplehz = pwf.nSamplesPerSec;
    new_config.bits_per_sample = pwf.wBitsPerSample;
    new_config.size = (uint32_t)chunkSize / (new_config.channels * (new_config.bits_per_sample/8));
    wav_set_config(h, &new_config);

    if (chunkSize != wav->image_size)
    {
        fprintf(stderr, __FUNCTION__ ": Error chunkSize (%d) != wav->image_size (%d)\n", chunkSize, wav->image_size);
    }

    /* Load new wav data */
    len = fread(wav->image, 1, wav->image_size, fp);

 exit:
    fclose(fp);
    return rc;
}

int wav_save(wav_handle h, const char *filename)
{
    wav_data *wav = (wav_data *)h;
    int rc = 0;
    FILE *fp;
    int len;
    uint32_t chunkID, chunkSize;
    PCMWAVEFORMAT pwf;

    /* check argument */
    if (wav == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (filename == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    fp = fopen(filename, "wb+");

    /* 'RIFF' */
    chunkID = CHUNK_ID('R', 'I', 'F', 'F');
    len = fwrite(&chunkID, 4, 1, fp);

    /* chunkSize */
    chunkSize = wav->image_size + 38;
    len = fwrite(&chunkSize, 4, 1, fp);

    /* 'WAVE' */
    chunkID = CHUNK_ID('W', 'A', 'V', 'E');
    len = fwrite(&chunkID, 4, 1, fp);

    /* 'fmt ' */
    chunkID = CHUNK_ID('f', 'm', 't', ' ');
    len = fwrite(&chunkID, 4, 1, fp);

    /* chunkSize */
    chunkSize = 18;
    len = fwrite(&chunkSize, 4, 1, fp);

    /* PCMWAVEFORMAT */
    pwf.wFormatTag = 1;			// PCM
    pwf.nChannels = wav->config.channels;
    pwf.nSamplesPerSec = wav->config.samplehz;
    pwf.nBlockAlign = wav->config.channels * (wav->config.bits_per_sample/8);
    pwf.nAvgBytesPerSec = pwf.nSamplesPerSec * pwf.nBlockAlign;
    pwf.wBitsPerSample = wav->config.bits_per_sample;
    len = fwrite(&pwf, sizeof(PCMWAVEFORMAT), 1, fp);

    /* Extended data */
    chunkSize = 0;
    len = fwrite(&chunkSize, 2, 1, fp);

    /* 'data' */
    chunkID = CHUNK_ID('d', 'a', 't', 'a');
    len = fwrite(&chunkID, 4, 1, fp);

    /* chunkSize */
    chunkSize = wav->image_size;
    len = fwrite(&chunkSize, 4, 1, fp);

    /* audio samples */
    len = fwrite(wav->image, 1, wav->image_size, fp);
    if (len != wav->image_size) {
        fprintf(stderr, __FUNCTION__ ": Write error %d bytes were written\n", len);
    }

    fclose(fp);

    return 0;
}
