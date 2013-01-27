/*
 * Copyright (C) 2002-2012 Hiroaki Inaba
 *
 * This is a C implementation of bmp library.
 * It provides functions to load/save bmp file and to access each pixel.
 */

#include <stdio.h>
#include <string.h>
#include "bmp.h"

/*
 * BITMAP structures
 */
#pragma pack(1)
typedef struct {
    uint16_t bfType; 
    uint32_t bfSize; 
    uint16_t bfReserved1; 
    uint16_t bfReserved2; 
    uint32_t bfOffBits; 
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    BITMAPINFOHEADER bmiHeader;
    uint32_t bmiColors;
} BITMAPINFO;

#pragma pack()

#define BI_RGB 0x00000000

/*
 * bmp internal data
 */
typedef struct {
    uint8_t *image;
    uint32_t image_size;
    bmp_config config;
} bmp_data;

/*
 * private functions
 */

/* return number of bytes for one line */
static uint32_t bytes_per_line(bmp_config *config)
{
    uint32_t size;
    size = (((config->bits_per_pixel / 8) * config->width + 3) & 0xfffffffc);

    return size;
}

/* return image buffer size that needs in bmp_data->image */
static uint32_t bmp_p_image_size(bmp_config *config)
{
    return bytes_per_line(config) * config->height;
}

/* return offset in the bmp_data->image */
static unsigned int bmp_p_offset(bmp_config *config, int x, int y)
{
    uint32_t offset;
    offset = bytes_per_line(config)*(config->height - y -1) + 3*x;

    return offset;
}

/* return image buffer size that needs in bmp_data->image */
static void bmp_p_release_image(bmp_data *bmp)
{
    if (bmp->image)
        free(bmp->image);

    bmp->image = 0;
    bmp->image_size = 0;
    bmp->config.width = 0;
    bmp->config.height = 0;
    bmp->config.bits_per_pixel = 0;

    return;
}

/*
 * Public functions
 */

/*
 * Allocate new internal bmp_data structure.
 * filename will be loaded if it is not 0.
 */
int bmp_open(bmp_handle *h, const char *filename)
{
    bmp_data *bmp;
    int rc = 0;

    /* check argument */
    if (h == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    bmp = (bmp_data*)malloc(sizeof(bmp_data));
    if (bmp)
    {
        memset(bmp, 0x00, sizeof(bmp_data));
        *h = (bmp_handle)bmp;

        if (filename)
            rc = bmp_load(*h, filename);
    }
    else
    {
        rc = -1;
    }

    return rc;
}

int bmp_close(bmp_handle h)
{
    bmp_data *bmp = (bmp_data *)h;

    /* check argument */
    if (bmp == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    /* release all resources */
    bmp_p_release_image(bmp);
    free(bmp);

    return 0;
};

/* Set new config and re-allocate image buffer */
int bmp_set_config(bmp_handle h, bmp_config *config)
{
    bmp_data *bmp = (bmp_data *)h;
    int rc = 0;

    /* check argument */
    if (bmp == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (config == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid argument\n");
        return -1;
    }
    if (config->bits_per_pixel != 24)
    {
        fprintf(stderr, __FUNCTION__ ": Error Only 24 bits/pixel is supported\n");
        return -1;
    }

    /* free old bmp buffer */
    bmp_p_release_image(bmp);

    /* copy config and allocate new buffer */
    bmp->config = *config;
    bmp->image_size = bmp_p_image_size(config);
    bmp->image = (uint8_t*)malloc(bmp->image_size);
    if (bmp->image == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Can't allocate bmp buffer\n");
        memset(bmp, 0x00, sizeof(bmp_data));
        rc = -1;
    }
    memset(bmp->image, 0xff, bmp->image_size);

    return rc;
}

int bmp_get_config(bmp_handle h, bmp_config *config)
{
    bmp_data *bmp = (bmp_data *)h;

    /* check argument */
    if (bmp == 0)
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
    *config = bmp->config;

    return 0;
}

int bmp_set_color(bmp_handle h, int x, int y, uint32_t color)
{
    bmp_data *bmp = (bmp_data *)h;
    int rc = 0;
    uint32_t offset;
    uint8_t B,G,R;

    /* check argument */
    if (bmp == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if ((x < 0) || (bmp->config.width -1 < x))
    {
        fprintf(stderr, __FUNCTION__ ": Error x=%d is out of range. It must be within [0, %d]\n", x, bmp->config.width-1);
        return -1;
    }
    if ((y < 0) || (bmp->config.height -1 < y))
    {
        fprintf(stderr, __FUNCTION__ ": Error y=%d is out of range. It must be within [0, %d]\n", y, bmp->config.height-1);
        return -1;
    }

    // This code only support 24 bits per pixel
    offset = bmp_p_offset(&(bmp->config), x, y);
    B = color & 0xff;
    G = (color >> 8) & 0xff;
    R = (color >> 16) & 0xff;

    bmp->image[offset+0] = B;
    bmp->image[offset+1] = G;
    bmp->image[offset+2] = R;

    return rc;
}

int bmp_get_color(bmp_handle h, int x, int y, uint32_t *color)
{
    bmp_data *bmp = (bmp_data *)h;
    int rc = 0;
    uint32_t offset;

    /* check argument */
    if (bmp == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (color == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid parameter\n");
        return -1;
    }
    if ((x < 0) || (bmp->config.width -1 < x))
    {
        fprintf(stderr, __FUNCTION__ ": Error x=%d is out of range. It must be within [0, %d]\n", x, bmp->config.width-1);
        return -1;
    }
    if ((y < 0) || (bmp->config.height -1 < y))
    {
        fprintf(stderr, __FUNCTION__ ": Error y=%d is out of range. It must be within [0, %d]\n", y, bmp->config.height-1);
        return -1;
    }

    // This code only support 24 bits per pixel
    offset = bmp_p_offset(&(bmp->config), x, y);
    *color = *((uint32_t*)(bmp->image + offset)) & 0x00ffffff;

    return rc;
}

int bmp_copy(bmp_handle dst, bmp_handle src)
{
    bmp_data *bmp_dst = (bmp_data *)dst;
    bmp_data *bmp_src = (bmp_data *)src;
    int rc = 0;

    /* check argument */
    if (bmp_dst == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }
    if (bmp_src == 0)
    {
        fprintf(stderr, __FUNCTION__ ": Error Invalid handle\n");
        return -1;
    }

    rc = bmp_set_config(dst, &bmp_src->config);
    if (rc == 0)
        memcpy(bmp_dst->image, bmp_src->image, bmp_dst->image_size);

    return rc;
}

int bmp_load(bmp_handle h, const char *filename)
{
    bmp_data *bmp = (bmp_data *)h;
    int rc = 0;
    BITMAPFILEHEADER BitMapFileHeader;
    BITMAPINFO BitMapInfo;
    int len;
    bmp_config new_config;
    FILE *fp;

    /* check argument */
    if (bmp == 0)
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

    /* Read BITMAPFILEHEADER */
    len = fread(&BitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* Check 'B', 'M' */
    if (BitMapFileHeader.bfType != 0x4d42)
    {
        fprintf(stderr, __FUNCTION__ ": Can't find \"BM\"\n");
        rc = -1;
        goto exit;
    }

    /* Read BITMAPINFO */
    len = fread(&BitMapInfo, sizeof(BITMAPINFO), 1, fp);

    if (BitMapInfo.bmiHeader.biBitCount != 24)
    {
        fprintf(stderr, __FUNCTION__ ": Only support 24 bits per pixel (%d)\n", BitMapInfo.bmiHeader.biBitCount);
        rc = -1;
        goto exit;
    }

    if (BitMapInfo.bmiHeader.biCompression != BI_RGB)
    {
        fprintf(stderr, __FUNCTION__ ": biCompression != BI_RGB\n");
        rc = -1;
        goto exit;
    }

    /* bmp_set_config release old image buffer and allocate new one. */
    new_config.height = BitMapInfo.bmiHeader.biHeight;
    new_config.width  = BitMapInfo.bmiHeader.biWidth;
    new_config.bits_per_pixel = BitMapInfo.bmiHeader.biBitCount;
    bmp_set_config(h, &new_config);
    //printf("width = %d, height = %d, bits/pixel = %d\n", bmp->config.width, bmp->config.height, bmp->config.bits_per_pixel);

    /* Then load new bmp image */
    fseek(fp, BitMapFileHeader.bfOffBits, SEEK_SET);
    len = fread(bmp->image, 1, bmp->image_size, fp);

 exit:
    fclose(fp);
    return rc;
}

int bmp_save(bmp_handle h, const char *filename)
{
    bmp_data *bmp = (bmp_data *)h;
    int rc = 0;
    BITMAPFILEHEADER BitMapFileHeader;
    BITMAPINFO BitMapInfo;
    int len;
    FILE *fp;

    /* check argument */
    if (bmp == 0)
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

    BitMapFileHeader.bfType = 0x4d42;		// 'BM'
    BitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + bmp->image_size;
    BitMapFileHeader.bfReserved1 = 0;
    BitMapFileHeader.bfReserved2 = 0;
    BitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

    BitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitMapInfo.bmiHeader.biWidth = bmp->config.width;
    BitMapInfo.bmiHeader.biHeight = bmp->config.height;
    BitMapInfo.bmiHeader.biPlanes = 1;
    BitMapInfo.bmiHeader.biBitCount = bmp->config.bits_per_pixel;
    BitMapInfo.bmiHeader.biCompression = BI_RGB;
    BitMapInfo.bmiHeader.biSizeImage = bmp->image_size;
    BitMapInfo.bmiHeader.biXPelsPerMeter = 0;
    BitMapInfo.bmiHeader.biYPelsPerMeter = 0;
    BitMapInfo.bmiHeader.biClrUsed = 0;
    BitMapInfo.bmiHeader.biClrImportant = 0;

    len = fwrite((char*)&BitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    len = fwrite((char*)&BitMapInfo, sizeof(BITMAPINFO), 1, fp);
    len = fwrite(bmp->image, 1, bmp->image_size, fp);

    fclose(fp);

    return rc;
}
