//
// Created by Timur Guliamov on 12.12.2021.
//

#include "file.h"
#include <stdio.h>

static const char *file_mode_raw_value[] = {
        [RB] = "rb",
        [WB] = "wb",
        [AB] = "ab",
};

enum file_open_status file_open(
        FILE **file_ptr,
        const char *filename,
        const enum file_mode mode
) {
    *file_ptr = fopen(filename, file_mode_raw_value[mode]);
    return *file_ptr == NULL
           ? FILE_OPEN_ERROR
           : FILE_OPEN_OK;
}

enum file_close_status file_close(FILE* const file) {
    if (file == NULL) return FILE_CLOSE_ERROR;

    return fclose(file) == 0 
        ? FILE_CLOSE_OK 
        : FILE_CLOSE_ERROR;
}
