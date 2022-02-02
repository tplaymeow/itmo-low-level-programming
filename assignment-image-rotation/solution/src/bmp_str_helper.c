//
// Created by Timur Guliamov on 26.12.2021.
//

#include "bmp.h"
#include "bmp_str_helper.h"

static const char *error_text_for_reading[] = {
        [READ_OK] = "Reading: Everything is good!",
        [READ_INVALID_HEADER_SIGNATURE] = "Invalid BMP-header signature",
        [READ_INVALID_HEADER_RESERVED] = "Invalid BMP-header reserved",
        [READ_INVALID_HEADER_BIT_PER_PIXEL] = "Invalid BMP-header bit per pixel count",
        [READ_INVALID_HEADER_PLANES] = "Invalid BMP-header planes count",
        [READ_INVALID_HEADER_SIZE] = "Invalid BMP-header size",
        [READ_INVALID_SIZE] = "Invalid BMP-file size",

        [READ_FREAD_ERROR] = "Reading: Error occured while reading file",
        [READ_FSEEK_ERROR] = "Reading: Error with fseek(...)",

        [READ_OTHER_ERROR] = "Reading: Unknown error"
};

static const char *error_text_for_writing[] = {
        [WRITE_OK] = "Writing: Everything is good",
        [WRITE_FWRITE_ERROR] = "Writing: Error occured while writing file",
        [WRITE_OTHER_ERROR] = "Writing: Unknown error"
};

const char *str_read_status(enum read_status status) {
    return error_text_for_reading[status];
}

const char *str_write_status(enum write_status status) {
    return error_text_for_writing[status];
}
