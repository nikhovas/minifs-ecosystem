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


#define EPOLL_WAIT_TIME 1000


#define OP_UNKNOWN 0
#define OP_FILE_CREATE 1
#define OP_FILE_GET 2
#define OP_FILE_REMOVE 3
#define OP_FILE_WRITE 4
#define OP_FILE_COPY 5
#define OP_DIR_CREATE 6
#define OP_DIR_GET 7
#define OP_DIR_REMOVE 8


int get_operation_type(size_t method_length, char * method_name) {
    if (method_length == 4) {
        if (memcmp(method_name, "FCRT", 4) == 0) {
            return OP_FILE_CREATE;
        } else if (memcmp(method_name, "FGET", 4) == 0) {
            return OP_FILE_GET;
        } else if (memcmp(method_name, "FWRT", 4) == 0) {
            return OP_FILE_WRITE;
        } else if (memcmp(method_name, "FCPY", 4) == 0) {
            return OP_FILE_COPY;
        } else if (memcmp(method_name, "DCRT", 4) == 0) {
            return OP_FILE_WRITE;
        } else if (memcmp(method_name, "DGET", 4) == 0) {
            return OP_DIR_GET;
        } else {
            return OP_UNKNOWN;
        }
    } else if (method_length == 3) {
        if (memcmp(method_name, "FRM", 3) == 0) {
            return OP_FILE_REMOVE;
        } else if (memcmp(method_name, "DRM", 4) == 0) {
            return OP_DIR_REMOVE;
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

    void * data = buf + rret - 5;

    int operation = get_operation_type(method_len, method);
    int error = 0;
    char * data2;
    int data2_size;
    char * token;
    char * source;
    char * destination;

    switch (operation) {
        case OP_FILE_CREATE:
            high_level__file_create(path, &error);
            break;
        
        case OP_FILE_GET:
            data2 = high_level__file_get(path, &data2_size, &error);
            if (error == NO_ERROR) {
                printf("data of file: %s\n", data2);
                answer_ok(connection_fd, data2_size, data2);
                return;
            }
            break;

        case OP_FILE_REMOVE:
            high_level__file_delete(path, &error);
            break;

        case OP_FILE_WRITE:
            high_level__file_write(path, data, content_length, &error);
            break;

        case OP_FILE_COPY:
            token = strtok(path, "@");
            source = token;
            token = strtok(NULL, " ");
            destination = token;
            if (destination == NULL) {
                answer_bad_request(connection_fd);
                return;
            }
            high_level__file_copy(source, destination, &error);
            break;

        case OP_DIR_CREATE:
            high_level__dir_create(path, &error);
            break;

        case OP_DIR_GET:
            data2 = minifs_core_textual__dir_get_contents(path, &data2_size, &error);
            if (error == NO_ERROR) {
                answer_ok(connection_fd, data2_size, data2);
                return;
            }
            break;

        case OP_DIR_REMOVE:
            high_level__dir_delete(path, &error);
            break;

        default:
            answer_method_not_allowed(connection_fd);
            return;
    }

    if (error == NO_ERROR) {
        answer_ok_only(connection_fd);
    } else if (error == NO_SUCH_FILE || error == NOT_A_VALID_PATH) {
        answer_not_found(connection_fd);
    } else {
        answer_internal_server_error(connection_fd);
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
    int signal_pipe[2];
    if (pipe(signal_pipe) == -1) {
        perror("can't create signal pipe");
        exit(EXIT_FAILURE);
    }

    int child_pid = fork();
    if (child_pid == 0) {
        close(signal_pipe[1]);

        int error;
        high_level__filesystem_create(argv[2], &error);

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
            perror("synce processes error");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("can't fork");
        exit(EXIT_FAILURE);
    }
    return 0;
}

