#pragma once
#include <minifs/storage-drivers/storage_driver_interface.h>


struct minifs_core__filesystem_context {
    struct storage_driver_interface * sdi;
};