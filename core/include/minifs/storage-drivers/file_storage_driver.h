struct file_storage_driver {
    // interface declarations
    void (*set_part)(struct storage_driver_interface * sdi, const uint8_t *data, size_t offset, size_t length, int *error);
    void (*get_part)(struct storage_driver_interface * sdi, uint8_t *data, size_t offset, size_t length, int *error);
    void (*init)(struct storage_driver_interface * sdi, int* error);
    void (*free)(struct storage_driver_interface * sdi, int* error);
    // own data
    char file_path[256];
    FILE * fileptr;
};