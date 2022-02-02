//
// Created by Timur Guliamov on 11.12.2021.
//

#ifndef ASSIGNMENT_IMAGE_ROTATION_IMAGE_H
#define ASSIGNMENT_IMAGE_ROTATION_IMAGE_H

#include <inttypes.h>
#include <stddef.h>

struct image {
    size_t width;
    size_t height;
    struct pixel *data;
};

struct pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct image image_create(size_t width, size_t height);

struct image image_empty();

void image_destroy(struct image image);

struct pixel image_get_pixel(
        const struct image image,
        size_t x,
        size_t y
);

void image_set_pixel(
        struct image image,
        struct pixel pixel,
        size_t x,
        size_t y
);

#endif //ASSIGNMENT_IMAGE_ROTATION_IMAGE_H
