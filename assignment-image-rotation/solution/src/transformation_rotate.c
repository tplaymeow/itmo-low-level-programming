//
// Created by Timur Guliamov on 26.12.2021.
//

#include "transformation_rotate.h"

struct image transformation_rotate(struct image const source) {
    struct image result = image_create(source.height, source.width);

    for (size_t i = 0; i < source.height; i++) {
        for (size_t j = 0; j < source.width; j++) {
            const struct pixel pixel = image_get_pixel(source, j, i);
            image_set_pixel(result, pixel, result.width - i - 1, j);
        }
    }
    return result;
}
