//
// Created by Timur Guliamov on 25.12.2021.
//

#include "bmp.h"
#include "image.h"
#include <stdio.h>

static const uint16_t BF_TYPE = 0x4D42;
static const uint16_t BI_PLANES = 1;
static const uint16_t BI_BIT_COUNT = 24;
static const uint32_t BF_RESERVED = 0x0;
static const uint32_t BMP_HEADER_SIZE = 54;
static const uint32_t BI_SIZE = 40;

struct __attribute__((packed)) bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
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
};

// MARK: - Helpers
static size_t calculate_padding(struct image const *img) {
    const size_t width_in_bytes = img->width * 3;
    return (4 - width_in_bytes % 4) % 4;
}

static size_t calculate_size(struct image const *image) {
    return image->height * (image->width * 3 + calculate_padding(image));
}

// MARK: - Reading
static enum read_status read_header(FILE *file, struct bmp_header *header) {
    const size_t items_count = 1;
    return items_count == fread(header, sizeof(struct bmp_header), items_count, file)
           ? READ_OK
           : READ_FREAD_ERROR;
}

static enum read_status validate_header(const struct bmp_header *header) {
    if (header->bfType != BF_TYPE) return READ_INVALID_HEADER_SIGNATURE;
    if (header->bfReserved != BF_RESERVED) return READ_INVALID_HEADER_RESERVED;
    if (header->biPlanes != BI_PLANES) return READ_INVALID_HEADER_PLANES;
    if (header->biBitCount != BI_BIT_COUNT) return READ_INVALID_HEADER_BIT_PER_PIXEL;
    if (header->bOffBits != BMP_HEADER_SIZE) return READ_INVALID_HEADER_SIZE;
    return READ_OK;
}

static enum read_status read_pixels(FILE *file_in, struct image *img) {
    const size_t padding = calculate_padding(img);
    const size_t size = img->width * sizeof(struct pixel);
    const size_t items_count = 1;

    for (size_t i = 0; i < img->height; i++) {
        void *ptr = img->data + (img->width * i);

        if (items_count != fread(ptr, size, items_count, file_in))
            return READ_FREAD_ERROR;
        if (fseek(file_in, padding, SEEK_CUR) != 0)
            return READ_FSEEK_ERROR;
    }
    return READ_OK;
}

// MARK: - Writing
static enum write_status write_header(FILE *file, struct image const *img) {
    const int32_t size = (int32_t) calculate_size(img);
    struct bmp_header header = {
            .bfType = BF_TYPE,
            .bfileSize = BMP_HEADER_SIZE + size,
            .bfReserved = 0,
            .bOffBits = BMP_HEADER_SIZE,
            .biSize = BI_SIZE,
            .biWidth = img->width,
            .biHeight = img->height,
            .biPlanes = BI_PLANES,
            .biBitCount = BI_BIT_COUNT,
            .biCompression = 0,
            .biSizeImage = size,
            .biXPelsPerMeter = 0,
            .biYPelsPerMeter = 0,
            .biClrUsed = 0,
            .biClrImportant = 0
    };

    const size_t items_count = 1;
    return items_count == fwrite(&header, sizeof(struct bmp_header), items_count, file)
           ? WRITE_OK
           : WRITE_FWRITE_ERROR;
}

static enum write_status write_pixels(FILE *file, struct image const *img) {
    const char padding_filler[] = {0, 0, 0, 0};
    const size_t padding = calculate_padding(img);

    for (size_t i = 0; i < img->height; i++) {
        const struct pixel *ptr = img->data + img->width * i;
        if (img->width != fwrite(ptr, sizeof(struct pixel), img->width, file))
            return WRITE_FWRITE_ERROR;
        if (padding != fwrite(padding_filler, sizeof(char), padding, file))
            return  WRITE_FWRITE_ERROR;
    }
    return WRITE_OK;
}

enum read_status from_bmp(FILE *in, struct image *img) {
    struct bmp_header header;

    const enum read_status status_reading = read_header(in, &header);
    if (status_reading != READ_OK) return status_reading;

    const enum read_status status_validating = validate_header(&header);
    if (status_validating != READ_OK) return status_validating;

    struct image result_image = image_create(header.biWidth, header.biHeight);
    const enum read_status result_status = read_pixels(in, &result_image);
    if (result_status != READ_OK) image_destroy(result_image);
    else *img = result_image;

    return result_status;
}

enum write_status to_bmp(FILE *out, struct image const *img) {
    enum write_status status;

    status = write_header(out, img);
    if (status != WRITE_OK) return status;

    return write_pixels(out, img);
}
