#include "minifs/core/bitmap.h"
#include "minifs/disk.h"
#include "minifs/errors.h"
#include "minifs/utils.h"

// #include <stdlib.h>


size_t alloc_bitmap_node(struct minifs_core__filesystem_context * ctx, size_t offset, size_t length, int * error) {
    *error = NO_ERROR;

    length = length / 8;
    // uint8_t* data = malloc(length);
    uint8_t data[length];
    ctx->sdi->get_part(ctx, data, offset, length, error);
    if (*error != NO_ERROR) {
        return 0;
    }

    for (size_t i = 0; i < length; ++i) {
        uint8_t val = data[i];
        for (uint8_t j = 0; j < 8; ++j) {
            uint8_t bit_value = get_bit(val, j);
            if (bit_value == 0) {
                val = set_bit(val, j);
                ctx->sdi->set_part(ctx, &val, offset + i * 8, 1, error);
                if (*error != NO_ERROR) {
                    return 0;
                }
                return i * 8 + j;
            }
        }
    }

    *error = NO_FREE_NODES_AVAILABLE;
    return 0;
}


void free_bitmap_node(struct minifs_core__filesystem_context * ctx, size_t offset, size_t number, int *error) {
    *error = NO_ERROR;

    uint8_t val;
    ctx->sdi->get_part(ctx, &val, offset + number / 8, 1, error);
    if (*error != NO_ERROR) {
        return;
    }

    val = set_bit(val, number % 8);

    ctx->sdi->set_part(ctx, &val, offset + number / 8, 1, error);
    if (*error != NO_ERROR) {
        return;
    }
}