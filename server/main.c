#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <linux/limits.h>


#include <minifs/high_level.h>
#include <minifs/textual.h>
#include <picohttpparser.h>
#include "answers.h"
#include <minifs/transfer-protocol.h>


#define EPOLL_WAIT_TIME 1000

#define OP_UNKNOWN 100


struct minifs_core_transfer_protocol_options minifs_po = {
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy
};

char kernel_device_path[128];
int kernel_device_fd;


int get_operation_type(size_t method_length, char * method_name) {
    if (method_length == 4) {
        if (memcmp(method_name, "FCRT", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__FILE_CREATE_REQUEST_NUM;
        } else if (memcmp(method_name, "FGET", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__FILE_GET_REQUEST_NUM;
        } else if (memcmp(method_name, "FWRT", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__FILE_WRITE_REQUEST_NUM;
        } else if (memcmp(method_name, "FCPY", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__FILE_COPY_REQUEST_NUM;
        } else if (memcmp(method_name, "DCRT", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__DIR_CREATE_REQUEST_NUM;
        } else if (memcmp(method_name, "DGET", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__DIR_GET_REQUEST_NUM;
        } else {
            return OP_UNKNOWN;
        }
    } else if (method_length == 3) {
        if (memcmp(method_name, "FRM", 3) == 0) {
            return MINIFS_CORE_PROTOCOL__FILE_DELETE_REQUEST_NUM;
        } else if (memcmp(method_name, "DRM", 4) == 0) {
            return MINIFS_CORE_PROTOCOL__DIR_DELETE_REQUEST_NUM;
        } else {
            return OP_UNKNOWN;
        }
    } else {
        return OP_UNKNOWN;
    }
}


void serve_connection(int connection_fd) {
    char buf[4096], *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;

    printf("Got connection\n");

    int request_parsed = 0;
    while (!request_parsed) {
        /* read the request */
        while ((rret = read(connection_fd, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR);
        if (rret <= 0) {
            answer_internal_server_error(connection_fd);
            return;
        }
        prevbuflen = buflen;
        buflen += rret;
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen, (const char **) &method, &method_len, (const char **) &path, &path_len, &minor_version, headers, &num_headers, prevbuflen);
        if (pret > 0) {
            request_parsed = 1;
        } else if (pret == -1) {
            answer_bad_request(connection_fd);
            return;
        } else if (buflen == sizeof(buf)) {
            answer_payload_too_large(connection_fd);
            return;
        }
    }

    path[path_len] = '\0';

    int content_length = 0;
    for (int i = 0; i < num_headers; ++i) {
        if (headers[i].name_len == 14 && memcmp(headers[i].name, "Content-Length", 14) == 0) {
            content_length = atoi(headers[i].value);
            break;
        }
    }

    void * data = buf + rret - content_length;

    int operation = get_operation_type(method_len, method);
    char * data2;
    int data2_size;
    char * token;
    char * source;
    char * destination;

    uint8_t buffer[1024];
    uint16_t buffer_size = 0;
    uint8_t directory_get = 0;

    switch (operation) {
        case MINIFS_CORE_PROTOCOL__FILE_CREATE_REQUEST_NUM:
            buffer_size = minifs_core_protocol__file_create(&minifs_po, path, buffer);
            break;
        
        case MINIFS_CORE_PROTOCOL__FILE_GET_REQUEST_NUM:
            buffer_size = minifs_core_protocol__file_get(&minifs_po, path, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__FILE_DELETE_REQUEST_NUM:
            buffer_size = minifs_core_protocol__file_delete(&minifs_po, path, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__FILE_WRITE_REQUEST_NUM:
            buffer_size = minifs_core_protocol__file_write(&minifs_po, path, data, content_length, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__FILE_COPY_REQUEST_NUM:
            token = strtok(path, "@");
            source = token;
            token = strtok(NULL, " ");
            destination = token;
            if (destination == NULL) {
                answer_bad_request(connection_fd);
                return;
            }
            buffer_size = minifs_core_protocol__file_copy(&minifs_po, source, destination, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__DIR_CREATE_REQUEST_NUM:
            buffer_size = minifs_core_protocol__dir_create(&minifs_po, path, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__DIR_GET_REQUEST_NUM:
            directory_get = 1;
            buffer_size = minifs_core_protocol__dir_get_contents(&minifs_po, path, buffer);
            break;

        case MINIFS_CORE_PROTOCOL__DIR_DELETE_REQUEST_NUM:
            buffer_size = minifs_core_protocol__dir_delete(&minifs_po, path, buffer);
            break;

        default:
            answer_method_not_allowed(connection_fd);
            return;
    }

    if (write(kernel_device_fd, buffer, buffer_size) <= 0) {
        printf("ERROR: kernel module connection error\n");
        answer_internal_server_error(connection_fd);
        return;
    }

    uint8_t result_meta_data[3];
    if (read(kernel_device_fd, result_meta_data, 3) <= 0) {
        printf("ERROR: kernel module connection error\n");
        answer_internal_server_error(connection_fd);
        return;
    }

    uint16_t data_size;
    uint8_t error = result_meta_data[0];
    data_size = *((uint16_t *) (result_meta_data + 1));
    if (data_size != 0) {
        if (read(kernel_device_fd, buffer, data_size) <= 0) {
            printf("ERROR: kernel module connection error\n");
            answer_internal_server_error(connection_fd);
            return;
        }

        if (directory_get) {
            int result_size = data_size / sizeof(directory_item_t);
            directory_item_t * buffer2 = (directory_item_t *) buffer;
            char result[128];
            int printed_size = 0;
            for (uint8_t i = 0; i < result_size; ++i) {
                printed_size += sprintf(result + printed_size, "%s\n", buffer2[i].name);
            }
            answer_ok(connection_fd, printed_size, result);
        } else {
            answer_ok(connection_fd, data_size, buffer);
        }
        
        return;
    }

    if (error == NO_ERROR) {
        answer_ok_only(connection_fd);
    } else if (error == NO_SUCH_FILE || error == NOT_A_VALID_PATH) {
        answer_not_found(connection_fd);
    } else {
        answer_internal_server_error_with_data(connection_fd, 1, &error);
    }
}


void server_loop(struct sockaddr * address, socklen_t addrlen, int stop_signal_fd) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1 || bind(socket_fd, address, addrlen) == -1 || listen(socket_fd, SOMAXCONN) == -1) {
        perror("can't start server");
        exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(0);
    struct epoll_event event = {.events = EPOLLIN | EPOLLERR | EPOLLHUP, .data.fd = stop_signal_fd};
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, stop_signal_fd, &event);
    event.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);

    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    while (1) {
        if (epoll_wait(epoll_fd, &event, 1, EPOLL_WAIT_TIME) <= 0) {
            continue;
        } else if (event.data.fd == stop_signal_fd) {
            break;
        }

        int connection_fd = accept(socket_fd, (struct sockaddr*) &client, &client_size);
        serve_connection(connection_fd);

        shutdown(connection_fd, SHUT_RDWR);
        close(connection_fd);
    }

    close(epoll_fd);
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

void signal_handler_loop(sigset_t full_mask, int throw_signal_fd) {
    siginfo_t info;
    while (1) {
        sigwaitinfo(&full_mask, &info);
        int received_signal = info.si_signo;
        if (received_signal == SIGTERM || received_signal == SIGINT) {
            if (write(throw_signal_fd, "0", 1) != 1) {
                perror("writing failed");
                exit(EXIT_FAILURE);
            }
            close(throw_signal_fd);
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    sigset_t full_mask;
    sigfillset(&full_mask);
    sigprocmask(SIG_BLOCK, &full_mask, NULL);

    if (argc != 3) {
        printf("Usage minifs-server <port-number> <symbol-device-path>\n");
        return 1;
    }

    int signal_pipe[2];
    if (pipe(signal_pipe) == -1) {
        perror("can't create signal pipe");
        exit(EXIT_FAILURE);
    }

    int child_pid = fork();
    if (child_pid == 0) {
        close(signal_pipe[1]);

        strcpy(kernel_device_path, argv[2]);
        kernel_device_fd = open(kernel_device_path, O_RDWR);
        if (kernel_device_fd < 0) {
            printf("ERROR: kernel module connection error. Can't start server.\n");
            return 1;
        }

        struct sockaddr_in address = {.sin_family = AF_INET, .sin_port = htons(atoi(argv[1]))};
        server_loop((struct sockaddr*) &address, sizeof(address), signal_pipe[0]);
        close(signal_pipe[0]);
        return 0;
    } else if (child_pid > 0) {
        close(signal_pipe[0]);
        signal_handler_loop(full_mask, signal_pipe[1]);
        close(signal_pipe[1]);
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            perror("sync processes error");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("can't fork");
        exit(EXIT_FAILURE);
    }
    return 0;
}

