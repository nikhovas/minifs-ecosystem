#include "minifs/utils.h"
#include <minifs/std-wrapper/string.h>


uint8_t get_bit(uint8_t val, uint8_t bit_num) {
    return (val >> bit_num) & 1;
}


uint8_t set_bit(uint8_t val, uint8_t bit_num) {
    return (val | (1 << bit_num));
}


char* trim_path(char *path) {
    unsigned long length = strlen(path);

    if (length == 0) {
        return path;
    }

    if (path[length - 1] == '/') {
        --length;
        path[length - 1] = 0;
    }

    if (path[0] == '/') {
        --length;
        return path + 1;
    }

    return path;
}