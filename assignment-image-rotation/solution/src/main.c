#include "bmp.h"
#include "bmp_str_helper.h"
#include "file.h"
#include "transformation_rotate.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

void print_string_err(const char *string) {
    fprintf(stderr, "%s\n", string);
}

int main(int argc, char **argv) {
    if (argc != 3) { print_string_err("Expected two arguments."); return 1; }

    FILE *input_file = NULL;
    FILE *output_file = NULL;

    if (file_open(&input_file, argv[1], RB) != FILE_OPEN_OK) {
        print_string_err("Can't open output file...");
        print_string_err(strerror(errno));
        return 1;
    }

    if (file_open(&output_file, argv[2], WB) != FILE_OPEN_OK) {
        print_string_err("Can't open output file...");
        print_string_err(strerror(errno));
        file_close(input_file);
        return 1;
    }

    struct image source_image = image_empty();

    enum read_status read_status = from_bmp(input_file, &source_image);
    if (read_status == READ_OK) {
        struct image rotated = transformation_rotate(source_image);

        enum write_status write_status = to_bmp(output_file, &rotated);
        if (write_status != WRITE_OK)
            print_string_err(str_write_status(write_status));

        image_destroy(rotated);
    }
    else {
        print_string_err(str_read_status(read_status));
    }

    image_destroy(source_image);
    
    int status = 0;
    if (file_close(input_file) != FILE_CLOSE_OK) { print_string_err("Can't close input file..."); status = 1; } 
    if (file_close(output_file) != FILE_CLOSE_OK) { print_string_err("Can't close output file..."); status = 1; }
    return status;
}
