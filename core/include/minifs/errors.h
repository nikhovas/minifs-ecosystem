#pragma once


#define NO_ERROR 0
#define NO_SUCH_FILE 1
#define INVALID_PATH 2
#define FILE_ALREADY_EXISTS 3
#define NO_FREE_NODES_AVAILABLE 4
#define NOT_A_DIRECTORY 5
#define NOT_A_VALID_PATH 6
#define WRONG_FILE_NAME 7
#define FILE_ERROR 8
#define NO_SUCH_SOURCE 9
#define NO_SUCH_DESTINATION 10
#define FILE_LIMIT 11
#define BLOCKS_LIMIT 12
#define TOO_BIG_FILE 13
#define NO_SUCH_COMMAND 14
#define MINIFS_ERROR__NOT_ENOUGH_REQUEST_SIZE 15


const char * get_error_description(int error_code);