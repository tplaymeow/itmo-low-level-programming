//
// Created by Timur Guliamov on 11.12.2021.
//

#ifndef ASSIGNMENT_IMAGE_ROTATION_BMP_H
#define ASSIGNMENT_IMAGE_ROTATION_BMP_H

#include "image.h"
#include <stdio.h>

enum read_status {
    READ_OK = 0,

    READ_INVALID_HEADER_SIGNATURE,
    READ_INVALID_HEADER_RESERVED,
    READ_INVALID_HEADER_BIT_PER_PIXEL,
    READ_INVALID_HEADER_PLANES,
    READ_INVALID_HEADER_SIZE,

    READ_INVALID_SIZE,

    READ_FREAD_ERROR,
    READ_FSEEK_ERROR,

    READ_OTHER_ERROR,
};

enum write_status {
    WRITE_OK = 0,
    WRITE_FWRITE_ERROR,
    WRITE_OTHER_ERROR,
};

enum read_status from_bmp(FILE *in, struct image *img);

enum write_status to_bmp(FILE *out, struct image const *img);

#endif //ASSIGNMENT_IMAGE_ROTATION_BMP_H
