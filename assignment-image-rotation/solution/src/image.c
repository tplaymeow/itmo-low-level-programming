//
// Created by Timur Guliamov on 11.12.2021.
//

#include "image.h"
#include <stdlib.h>

static size_t index_of_coordinates(struct image image, size_t x, size_t y) {
    return y * image.width + x;
}

struct image image_create(size_t width, size_t height) {
    const struct image result = (struct image) {
            .width = width,
            .height = height,
            .data = malloc(width * height * sizeof(struct pixel))
    };

    return result;
}

struct image image_empty() {
    const struct image result = (struct image) {
            .width = 0,
            .height = 0,
            .data = NULL
    };

    return result;
}

void image_destroy(struct image image) {
    free(image.data);
}

struct pixel image_get_pixel(const struct image image, size_t x, size_t y) {
    const size_t index = index_of_coordinates(image, x, y);
    return image.data[index];
}

void image_set_pixel(struct image image, struct pixel pixel, size_t x, size_t y) {
    const size_t index = index_of_coordinates(image, x, y);
    image.data[index] = pixel;
}
