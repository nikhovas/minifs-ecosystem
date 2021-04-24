#include <minifs/transfer-protocol.h>



ssize_t filename_to_path_buffer(
    struct minifs_core_transfer_protocol_options * tp_options,
    const uint8_t *input_buffer,
    size_t input_buffer_size,
    uint8_t * buffer,
    uint8_t * buffer_size
) {
    if (input_buffer_size < 1) {
        return -2;
    }
    *buffer_size = input_buffer[1];
    if (input_buffer_size < 1 + *buffer_size) {
        return -2;
    }

    tp_options->parse_request_memory_copy_function(buffer, (uint8_t *) (input_buffer + 1), *buffer_size);
    buffer[*buffer_size] = '\0';
    return *buffer_size + 1;
}


ssize_t contents_to_data_buffer(
    struct minifs_core_transfer_protocol_options * tp_options,
    const uint8_t *input_buffer,
    size_t input_buffer_size,
    uint8_t * buffer,
    uint16_t * buffer_size
) {
    if (input_buffer_size < 2) {
        return -2;
    }

    const uint16_t* addr = (const uint16_t *) input_buffer;
    *buffer_size = (size_t) *addr;
    if (input_buffer_size < 2 + *buffer_size) {
        return -2;
    }

    tp_options->parse_request_memory_copy_function(buffer, (uint8_t *) (input_buffer + 2), *buffer_size);
    return *buffer_size + 2;
}


ssize_t minifs_core_protocol__parse_request(
    struct minifs_core_transfer_protocol_options * tp_options,
    const uint8_t * input_buffer,
    const uint8_t input_buffer_size,
    uint8_t * first_path_buffer,
    uint8_t * first_path_size,
    uint8_t * second_path_buffer,
    uint8_t * second_path_size,
    uint8_t * data_buffer,
    uint16_t * data_size,
    uint8_t * operation_type
) {
    if (input_buffer_size < 1) {
        return -2;
    }

    *operation_type = (uint8_t) input_buffer[0];

    ssize_t global_read = 1;
    ssize_t local_read = 0;
    switch (*operation_type) {
        case 0: // file create
        case 1: // file get
        case 2: // file remove
        case 5: // dir create
        case 6: // dir get
        case 7: // dir remove
            local_read = filename_to_path_buffer(tp_options, input_buffer + global_read, input_buffer_size - global_read, first_path_buffer, first_path_size);
            if (local_read < 0) {
                return local_read;
            }
            global_read += local_read;
            break;
        case 3: // file write
            local_read = filename_to_path_buffer(tp_options, input_buffer + global_read, input_buffer_size - global_read, first_path_buffer, first_path_size);
            if (local_read < 0) {
                return local_read;
            }
            global_read += local_read;

            if (input_buffer_size <= global_read) {
                return -2;
            }

            local_read = contents_to_data_buffer(tp_options, input_buffer + global_read, input_buffer_size - global_read, data_buffer, data_size);
            if (local_read < 0) {
                return local_read;
            }
            global_read += local_read;
            break;
        case 4: // file copy
            local_read = filename_to_path_buffer(tp_options, input_buffer + global_read, input_buffer_size - global_read, first_path_buffer, first_path_size);
            if (local_read < 0) {
                return local_read;
            }
            global_read += local_read;

            if (input_buffer_size <= global_read) {
                return -2;
            }

            local_read = filename_to_path_buffer(tp_options, input_buffer + global_read, input_buffer_size - global_read, second_path_buffer, second_path_size);
            if (local_read < 0) {
                return local_read;
            }
            global_read += local_read;
            break;
        default:
            break;
    }

    return global_read;
}