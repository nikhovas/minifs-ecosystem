#include <minifs/transfer-protocol.h>


ssize_t minifs_core_protocol__create_response(
    struct minifs_core_transfer_protocol_options * tp_options,
    uint8_t status,
    uint8_t * data,
    uint16_t data_size,
    uint8_t * buffer
) {
    tp_options->create_response_memory_copy_function(buffer, &status, 1);
    tp_options->create_response_memory_copy_function(buffer + 1, (uint8_t *) &data_size, 2);
    if (data_size != 0) {
        tp_options->create_response_memory_copy_function(buffer + 3, data, data_size);
    }

    return 3 + data_size;
}


ssize_t minifs_core_protocol__parse_response(
    struct minifs_core_transfer_protocol_options * tp_options,
    const uint8_t * input_buffer,
    const uint16_t input_buffer_size,
    uint8_t * status,
    uint8_t * data,
    uint16_t * data_size
) {
    if (input_buffer_size < 3) {
        return -2;
    }

    *status = input_buffer[0];

    const char* with_offset = input_buffer + 1;
    const uint16_t* addr = (const uint16_t *) with_offset;
    *data_size = (uint16_t) *addr;
    if (input_buffer_size < 3 + *data_size) {
        return -2;
    }

    if (*data_size != 0) {
        tp_options->parse_response_memory_copy_function(data, (uint8_t *) (input_buffer + 3), *data_size);
    }
    
    return 3 + *data_size;
}