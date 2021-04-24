#include <memory.h>
#include "minifs/disk.h"
#include "minifs/constants.h"
#include "minifs/errors.h"


FILE* fileptr;


void set_part_file(const uint8_t *data, FILE* file, size_t offset, size_t length, int *error) {
    *error = NO_ERROR;

    if (fseek(file, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fwrite(data, length, 1, file) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void get_part_file(uint8_t *data, FILE* file, size_t offset, size_t length, int * error) {
    *error = NO_ERROR;

    if (fseek(file, offset, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    if (fread(data, length, 1, file) == 0) {
        *error = FILE_ERROR;
        return;
    }
}


void create_file(char * path, int * error) {
    *error = NO_ERROR;

    FILE *fp = fopen(path, "wb");
    if (fp == NULL) {
        *error = FILE_ERROR;
        return;
    }

    if (fseek(fp, END_OFFSET - 1, SEEK_SET) != 0) {
        *error = FILE_ERROR;
        return;
    }

    fputc('\0', fp);
    if (ferror(fp)) {
        *error = FILE_ERROR;
        return;
    }

    fclose(fp);
}


void set_file(FILE * file) {
    fileptr = file;
}