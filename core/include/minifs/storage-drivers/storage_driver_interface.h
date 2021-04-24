struct storage_driver_interface {
    void (*set_part)(struct storage_driver_interface * sdi, const uint8_t *data, size_t offset, size_t length, int *error);
    void (*get_part)(struct storage_driver_interface * sdi, uint8_t *data, size_t offset, size_t length, int *error);
    void (*init)(struct storage_driver_interface * sdi, int* error);
    void (*free)(struct storage_driver_interface * sdi, int* error);
};