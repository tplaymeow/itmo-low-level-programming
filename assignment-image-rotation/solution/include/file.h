//
// Created by Timur Guliamov on 11.12.2021.
//

#ifndef ASSIGNMENT_IMAGE_ROTATION_FILE_H
#define ASSIGNMENT_IMAGE_ROTATION_FILE_H

#include <stdio.h>

enum file_mode {
    RB, WB, AB
};
enum file_open_status {
    FILE_OPEN_OK, FILE_OPEN_ERROR
};
enum file_close_status {
    FILE_CLOSE_OK, FILE_CLOSE_ERROR
};

enum file_open_status file_open(
        FILE **file_ptr,
        const char *filename,
        const enum file_mode mode
);

enum file_close_status file_close(FILE* const file);

#endif //ASSIGNMENT_IMAGE_ROTATION_FILE_H
