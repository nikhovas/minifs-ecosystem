#include <minifs/std-wrapper/stdint.h>


#define MINIFS_CORE_PROTOCOL__FILE_CREATE_REQUEST_NUM 0
#define MINIFS_CORE_PROTOCOL__FILE_GET_REQUEST_NUM 1
#define MINIFS_CORE_PROTOCOL__FILE_DELETE_REQUEST_NUM 2
#define MINIFS_CORE_PROTOCOL__FILE_WRITE_REQUEST_NUM 3
#define MINIFS_CORE_PROTOCOL__FILE_COPY_REQUEST_NUM 4
#define MINIFS_CORE_PROTOCOL__DIR_CREATE_REQUEST_NUM 5
#define MINIFS_CORE_PROTOCOL__DIR_GET_REQUEST_NUM 6
#define MINIFS_CORE_PROTOCOL__DIR_DELETE_REQUEST_NUM 7


struct minifs_core_transfer_protocol_options {
    ssize_t (*parse_request_memory_copy_function) (uint8_t * dest, const uint8_t * src, size_t n);
    ssize_t  (*create_request_memory_copy_function) (uint8_t * dest, const uint8_t * src, size_t n);
    ssize_t  (*parse_response_memory_copy_function) (uint8_t * dest, const uint8_t * src, size_t n);
    ssize_t  (*create_response_memory_copy_function) (uint8_t * dest, const uint8_t * src, size_t n);
};


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
);


ssize_t minifs_core_protocol__file_create( struct minifs_core_transfer_protocol_options * tp_options, const char * name, uint8_t *buffer);
ssize_t minifs_core_protocol__file_get( struct minifs_core_transfer_protocol_options * tp_options, const char * name, int* data_length, uint8_t *buffer);
ssize_t minifs_core_protocol__file_write( struct minifs_core_transfer_protocol_options * tp_options, char* name, void* contents, int content_length, uint8_t *buffer);
ssize_t minifs_core_protocol__file_delete( struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer);
ssize_t minifs_core_protocol__file_copy( struct minifs_core_transfer_protocol_options * tp_options, char * source, char * destination, uint8_t *buffer);


ssize_t minifs_core_protocol__dir_create( struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer);
ssize_t minifs_core_protocol__dir_delete( struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer);
ssize_t minifs_core_protocol__dir_get_contents( struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t * size, uint8_t *buffer);


ssize_t minifs_core_protocol__create_response(
    struct minifs_core_transfer_protocol_options * tp_options,
    uint8_t status,
    uint8_t * data,
    uint16_t data_size,
    uint8_t * buffer
);


ssize_t minifs_core_protocol__parse_response(
    struct minifs_core_transfer_protocol_options * tp_options,
    const uint8_t * input_buffer,
    const uint16_t input_buffer_size,
    uint8_t * status,
    uint8_t * data,
    uint16_t * data_size
);
