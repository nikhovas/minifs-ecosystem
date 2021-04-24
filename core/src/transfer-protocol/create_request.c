#include <minifs/transfer-protocol.h>
#include <minifs/std-wrapper/string.h>


ssize_t minifs_core_protocol__simple_request(struct minifs_core_transfer_protocol_options * tp_options, const char * name, uint8_t *buffer, uint8_t request_num) {
    tp_options->create_request_memory_copy_function(buffer, &request_num, 1);

    uint8_t filename_length = strlen(name);
    tp_options->create_request_memory_copy_function(buffer + 1, &filename_length, 1);

    tp_options->create_request_memory_copy_function(buffer + 2, name, filename_length);
    return filename_length + 2;
}


ssize_t minifs_core_protocol__file_create(struct minifs_core_transfer_protocol_options * tp_options, const char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__FILE_CREATE_REQUEST_NUM);
}


ssize_t minifs_core_protocol__file_get(struct minifs_core_transfer_protocol_options * tp_options, const char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__FILE_GET_REQUEST_NUM);
}



ssize_t minifs_core_protocol__file_write(struct minifs_core_transfer_protocol_options * tp_options, char* name, void* contents, int content_length, uint8_t *buffer) {
    uint8_t request_num = MINIFS_CORE_PROTOCOL__FILE_WRITE_REQUEST_NUM;
    tp_options->create_request_memory_copy_function(buffer, &request_num, 1);

    uint8_t filename_length = strlen(name);
    tp_options->create_request_memory_copy_function(buffer + 1, &filename_length, 1);
    tp_options->create_request_memory_copy_function(buffer + 2, name, filename_length);

    uint16_t file_content_length = content_length;
    tp_options->create_request_memory_copy_function(buffer + 2 + filename_length, (uint8_t *) &file_content_length, 2);
    tp_options->create_request_memory_copy_function(buffer + 2 + filename_length + 2, contents, file_content_length);
    return 2 + filename_length + 2 + file_content_length;
}


ssize_t minifs_core_protocol__file_delete(struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__FILE_DELETE_REQUEST_NUM);
}


ssize_t minifs_core_protocol__file_copy(struct minifs_core_transfer_protocol_options * tp_options, char * source, char * destination, uint8_t *buffer) {
    uint8_t request_num = MINIFS_CORE_PROTOCOL__FILE_COPY_REQUEST_NUM;
    tp_options->create_request_memory_copy_function(buffer, &request_num, 1);

    uint8_t source_length = strlen(source);
    tp_options->create_request_memory_copy_function(buffer + 1, &source_length, 1);
    tp_options->create_request_memory_copy_function(buffer + 2, source, source_length);

    uint8_t destination_length = strlen(destination);
    tp_options->create_request_memory_copy_function(buffer + 2 + source_length, &source_length, 1);
    tp_options->create_request_memory_copy_function(buffer + 2 + source_length + 1, destination, destination_length);

    return 2 + source_length + 1 + destination_length;
}


ssize_t minifs_core_protocol__dir_create(struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__DIR_CREATE_REQUEST_NUM);
}


ssize_t minifs_core_protocol__dir_delete(struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__DIR_DELETE_REQUEST_NUM);
}


ssize_t minifs_core_protocol__dir_get_contents(struct minifs_core_transfer_protocol_options * tp_options, char * name, uint8_t *buffer) {
    return minifs_core_protocol__simple_request(tp_options, name, buffer, MINIFS_CORE_PROTOCOL__DIR_GET_REQUEST_NUM);
}