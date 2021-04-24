#include <minifs/std-wrapper/memory.h>
#include "minifs/disk.h"
#include "minifs/constants.h"
#include "minifs/errors.h"
#include <minifs/std-wrapper/stdint.h>
#include <minifs/constants.h>


uint8_t filesystem_data[END_OFFSET];


void set_part_file(const uint8_t *data, FILE* file, size_t offset, size_t length, int *error) {
    *error = NO_ERROR;
    memcpy(filesystem_data + offset, data, length);
}


void get_part_file(uint8_t *data, FILE* file, size_t offset, size_t length, int * error) {
    *error = NO_ERROR;
    memcpy(data, filesystem_data + offset, length);
}


void create_file(char * path, int * error) {
    *error = NO_ERROR;
}


void set_file(FILE * file) {
}