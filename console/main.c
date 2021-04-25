#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <minifs/high_level.h>
#include <minifs/textual.h>
#include <minifs/storage-drivers/file_storage_driver.h>
#include <minifs/storage-drivers/in_memory_storage_driver.h>
#include <minifs/transfer-protocol.h>
#include <minifs/std-wrapper/memory.h>
#include <minifs/std-wrapper/string.h>
#include <fcntl.h>
#include <unistd.h>
#include <minifs/core/directory.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <picohttpparser.h>
#include <errno.h>


struct minifs_core__filesystem_context ctx;
//struct file_storage_driver sdi;
struct in_memory_storage_driver imsd;
void (*execute_cmd)(int args_count, char** args);

struct minifs_core_transfer_protocol_options minifs_po = {
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) &memcpy
};


int kernel_device_fd;


char server_host[128];
int server_port = 0;


void cmd_file_create(char * name, int *error) {
    minifs_core_textual__file_create(&ctx, name, error);
}


void cmd_file_get(char * name, int *error) {
    int file_length;
    char * data = minifs_core_textual__file_get(&ctx, name, &file_length, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


void cmd_file_write(char * name, char * contents, int *error) {
    minifs_core_textual__file_write(&ctx, name, contents, (int) strlen(contents), error);
}


void cmd_file_delete(char * name, int *error) {
    minifs_core_textual__file_delete(&ctx, name, error);
}


void cmd_file_copy(char * source, char * destination, int *error) {
    minifs_core_textual__file_copy(&ctx, source, destination, error);
}


void cmd_dir_create(char * name, int *error) {
    minifs_core_textual__dir_create(&ctx, name, error);
}

void cmd_dir_delete(char * name, int *error) {
    minifs_core_textual__dir_delete(&ctx, name, error);
}


void cmd_dir_get(char * name, int *error) {
    uint8_t size;
    char * data = minifs_core_textual__dir_get_contents(&ctx, name, (int *) &size, error);
    if (*error == NO_ERROR) {
        printf("%s\n", data);
    }
}


int socket_connect(char *host, int port) {
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;
    
    if((hp = gethostbyname(host)) == NULL){
        herror("gethostbyname");
        exit(1);
    }

    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
    
    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }
    
    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("connect");
        exit(1);
    }
    return sock;
}


void error_print(int error) {
    switch (error) {
        case NO_ERROR:
            break;
        case NO_SUCH_FILE:
            printf("No such file/dir\n");
            break;
        case INVALID_PATH:
            printf("Invalid path\n");
            break;
        case FILE_ALREADY_EXISTS:
            printf("File already exists\n");
            break;
        case NO_FREE_NODES_AVAILABLE:
            printf("No free nodes available\n");
            break;
        case NOT_A_DIRECTORY:
            printf("Not a directory\n");
            break;
        case NOT_A_VALID_PATH:
            printf("Not a valid path\n");
            break;
        case WRONG_FILE_NAME:
            printf("Wrong file name\n");
            break;
        case FILE_ERROR:
            printf("Error while connection to disk. Your data can be damaged.\n");
            break;
        case NO_SUCH_SOURCE:
            printf("No such source.\n");
            break;
        case NO_SUCH_DESTINATION:
            printf("No such destination.\n");
            break;
        case FILE_LIMIT:
            printf("File limit.\n");
            break;
        case BLOCKS_LIMIT:
            printf("Blocks for data limit.\n");
            break;
        case TOO_BIG_FILE:
            printf("Too big file\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }

    if (error == FILE_ERROR) {
        printf("Disk error. Data can be damaged.\n");
    }
}


void embedded_execute_cmd(int args_count, char** args) {
    int error = 0;
    if (args_count >= 3) {
        if (strcmp(args[0], "file") == 0) {
            if (strcmp(args[1], "create") == 0) {
                cmd_file_create(args[2], &error);
            } else if (strcmp(args[1], "get") == 0) {
                cmd_file_get(args[2], &error);
            } else if (strcmp(args[1], "write") == 0) {
                if (args_count >= 4) {
                    cmd_file_write(args[2], args[3], &error);
                } else {
                    printf("Wrong cmd\n");
                }
            } else if (strcmp(args[1], "delete") == 0) {
                cmd_file_delete(args[2], &error);
            } else if (strcmp(args[1], "copy") == 0) {
                if (args_count >= 4) {
                    cmd_file_copy(args[2], args[3], &error);
                } else {
                    printf("Wrong cmd\n");
                }
            } else {
                printf("Wrong cmd\n");
            }
        } else if (strcmp(args[0], "dir") == 0) {
            if (strcmp(args[1], "create") == 0) {
                cmd_dir_create(args[2], &error);
            } else if (strcmp(args[1], "get") == 0) {
                cmd_dir_get(args[2], &error);
            } else if (strcmp(args[1], "delete") == 0) {
                cmd_dir_delete(args[2], &error);
            } else {
                printf("Wrong cmd\n");
            }
        }
    } else {
        printf("Wrong cmd\n");
    }

    error_print(error);
}


void kernel_execute_cmd(int args_count, char** args) {
    uint8_t buffer[1024];
    uint16_t buffer_size = 0;
    uint8_t directory_get = 0;
    if (args_count >= 3) {
        if (strcmp(args[0], "file") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size = minifs_core_protocol__file_create(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size = minifs_core_protocol__file_get(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "write") == 0) {
                if (args_count >= 4) {
                    buffer_size = minifs_core_protocol__file_write(&minifs_po, args[2], args[3], strlen(args[3]), buffer);
                } else {
                    printf("Wrong cmd\n");
                }
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size = minifs_core_protocol__file_delete(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "copy") == 0) {
                if (args_count >= 4) {
                    buffer_size = minifs_core_protocol__file_copy(&minifs_po, args[2], args[3], buffer);
                } else {
                    printf("Wrong cmd\n");
                }
            } else {
                printf("Wrong cmd\n");
            }
        } else if (strcmp(args[0], "dir") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size = minifs_core_protocol__dir_create(&minifs_po, args[2], buffer);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size = minifs_core_protocol__dir_get_contents(&minifs_po, args[2], buffer);
                directory_get = 1;
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size = minifs_core_protocol__dir_delete(&minifs_po, args[2], buffer);
            } else {
                printf("Wrong cmd\n");
            }
        }
    } else {
        printf("Wrong cmd\n");
    }

    if (write(kernel_device_fd, buffer, buffer_size) <= 0) {
        printf("ERROR: kernel module connection error");
    }

    uint8_t result_meta_data[3];
    if (read(kernel_device_fd, result_meta_data, 3) <= 0) {
        printf("ERROR: kernel module connection error");
    }
    uint16_t data_size;
    uint8_t error = result_meta_data[0];
    data_size = *((uint16_t *) (result_meta_data + 1));
    if (data_size != 0) {
        if (read(kernel_device_fd, buffer, data_size) <= 0) {
            printf("ERROR: kernel module connection error");
        }

        if (directory_get) {
            int result_size = data_size / sizeof(directory_item_t);
            directory_item_t * buffer2 = (directory_item_t *) buffer;
            for (uint8_t i = 0; i < result_size; ++i) {
                printf("%s\n", buffer2[i].name);
            }
        } else {
            printf("%.*s\n", data_size, buffer);
        }
        
    }

    error_print(error);
}


void server_execute_cmd(int args_count, char** args) {
    char buffer[1024];
    uint16_t buffer_size = 0;
    
    if (args_count >= 3) {
        if (strcmp(args[0], "file") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "FCRT %s HTTP/1.1\r\n\r\n", args[2]);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "FGET %s HTTP/1.1\r\n\r\n", args[2]);
            } else if (strcmp(args[1], "write") == 0) {
                if (args_count >= 4) {
                    int content_length = strlen(args[3]);
                    buffer_size += sprintf(buffer + buffer_size, "FWRT %s HTTP/1.1\r\nContent-Length: %d\r\n\r\n", args[2], content_length);
                    memcpy(buffer + buffer_size, args[3], content_length);
                    buffer_size += content_length;
                } else {
                    printf("Wrong cmd\n");
                }
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "FRM %s HTTP/1.1\r\n\r\n", args[2]);
            } else if (strcmp(args[1], "copy") == 0) {
                if (args_count >= 4) {
                    buffer_size += sprintf(buffer + buffer_size, "FCPY %s@%s HTTP/1.1\r\n\r\n", args[2], args[3]);
                } else {
                    printf("Wrong cmd\n");
                }
            } else {
                printf("Wrong cmd\n");
            }
        } else if (strcmp(args[0], "dir") == 0) {
            if (strcmp(args[1], "create") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "DCRT %s HTTP/1.1\r\n\r\n", args[2]);
            } else if (strcmp(args[1], "get") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "DGET %s HTTP/1.1\r\n\r\n", args[2]);
            } else if (strcmp(args[1], "delete") == 0) {
                buffer_size += sprintf(buffer + buffer_size, "DRM %s HTTP/1.1\r\n\r\n", args[2]);
            } else {
                printf("Wrong cmd\n");
            }
        }
    } else {
        printf("Wrong cmd\n");
    }

    int socket_fd = socket_connect(server_host, server_port); 
    write(socket_fd, buffer, buffer_size);

    int meta_buflen = 0;
    buffer_size = 0;
    while (1) {
        while ((meta_buflen = read(socket_fd, buffer + meta_buflen, sizeof(buffer) - meta_buflen)) == -1 && errno == EINTR);
        if (meta_buflen > 0) {
            buffer_size += meta_buflen;
        } else if (meta_buflen == 0) {
            break;
        } else {
            printf("Error: server connection error\n");
            exit(1);
        }

        if (buffer_size == sizeof(buffer)) {
            printf("Error: too big response\n");
            exit(1);
        }
    }

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    printf("Response size: %d\n", buffer_size);
    printf("%.*s\n", buffer_size, buffer);
}


void cmd() {
    while (1) {
        char data[1024];
        int args_count = 0;
        char *args[32];
        fgets(data, 1024, stdin);
        data[strlen(data) - 1] = 0;
        if (strcmp(data, "exit") == 0) {
            break;
        }
        const char delim[] = " ";

        for (char *ptr = strtok(data, delim); ptr != NULL; ptr = strtok(NULL, delim)) {
            args[args_count++] = ptr;
        }

        execute_cmd(args_count, args);
    }
}


void embedded_filesystem() {
    int error;
    minifs_core__create_in_memory_storage_driver(&imsd);
    ctx.sdi = (struct storage_driver_interface *) &imsd;
    imsd.init(&ctx, &error);
}


void kernel_filesystem(const char * symbol_device_path) {
    kernel_device_fd = open(symbol_device_path, O_RDWR);
    if (kernel_device_fd < 0) {
        printf("Can't connect to kernel module\n");
        exit(1);
    }
}


int main(int argc, char* argv[]) {
    if (argc == 4 && strcmp(argv[1], "server") == 0) {
        strcpy(server_host, argv[2]);
        server_port = atoi(argv[3]);
        execute_cmd = &server_execute_cmd;
    } else if (argc == 3 && strcmp(argv[1], "kernel") == 0) {
        execute_cmd = &kernel_execute_cmd;
        kernel_filesystem(argv[2]);
    } else {
        execute_cmd = &embedded_execute_cmd;
        embedded_filesystem();
    }

    cmd();

    return 0;
}
