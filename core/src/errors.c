#include "minifs/errors.h"


const char * get_error_description(int error_code) {
    switch (error_code) {
        case NO_ERROR:
            return "";
        case NO_SUCH_FILE:
            return "No such file/dir";
        case INVALID_PATH:
            return "Invalid path";
        case FILE_ALREADY_EXISTS:
            return "File already exists";
        case NO_FREE_NODES_AVAILABLE:
            return "No free nodes available";
        case NOT_A_DIRECTORY:
            return "Not a directory";
        case NOT_A_VALID_PATH:
            return "Not a valid path";
        case WRONG_FILE_NAME:
            return "Wrong file name";
        case FILE_ERROR:
            return "Error while connection to disk. Your data can be damaged";
        case NO_SUCH_SOURCE:
            return "No such source";
        case NO_SUCH_DESTINATION:
            return "No such destination";
        case FILE_LIMIT:
            return "File limit";
        case BLOCKS_LIMIT:
            return "Blocks for data limit";
        default:
            return "Unknown error";
    }
}