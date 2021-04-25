#include "answers.h"


#include <stdio.h>
#include <unistd.h>


void answer_ok(int descr, int content_length, const void* contents) {
    dprintf(descr, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", content_length);
    write(descr, contents, content_length);
}

void answer_ok_only(int descr) {
    dprintf(descr, "HTTP/1.1 200 OK\r\n\r\n");
}

void answer_created(int descr) {
    dprintf(descr, "HTTP/1.1 201 Created\r\n\r\n");
}


void answer_not_found(int descr) {
    dprintf(descr, "HTTP/1.1 404 Not Found\r\n\r\n");
}


void answer_error_request_too_large(int descr) {
    dprintf(descr, "HTTP/1.1 413 Request Entity Too Large\r\n\r\n");
}


void answer_bad_request(int descr) {
    dprintf(descr, "HTTP/1.1 400 Bad Request\r\n\r\n");
}


void answer_internal_server_error(int descr) {
    dprintf(descr, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
}


void answer_internal_server_error_with_data(int descr, int content_length, const void* contents) {
    dprintf(descr, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\n", content_length);
    write(descr, contents, content_length);
}


void answer_method_not_allowed(int descr) {
    dprintf(descr, "HTTP/1.1 405 Method Not Allowed\r\n\r\n");
}


void answer_payload_too_large(int descr) {
    dprintf(descr, "HTTP/1.1 413 Payload Too Large\r\n\r\n");
}