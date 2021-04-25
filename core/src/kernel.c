#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#define KERNEL_MODULE
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>



#include <minifs/constants.h>
#include <minifs/middle_level.h>
// #include <stdlib.h>
// #include <string.h>

#include <minifs/core/directory.h>
#include <minifs/core/file.h>
#include <minifs/transfer-protocol.h>
#include <minifs/storage-drivers/in_memory_storage_driver.h>


#define ROOT_DIR_ID 0


int scull_minor = 0;
int scull_major = 0;


struct char_device {
    char data[100];
} device;


struct minifs_core_transfer_protocol_options tp_options = {
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) copy_from_user,
    NULL,
    NULL,
    (ssize_t (*)(uint8_t *, const uint8_t *, size_t)) copy_to_user
};


struct minifs_core__filesystem_context ctx;
struct in_memory_storage_driver imsd;


char storing = 0;
uint8_t buffer[1024];
uint16_t buffer_size = 0;
uint8_t buffer_one[128];
uint8_t buffer_one_size = 0;
uint8_t buffer_two[128];
uint8_t buffer_two_size = 0;

uint8_t status = 0;
uint8_t current_operation = 0;


struct cdev *p_cdev;


ssize_t scull_read(struct file *flip, char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_INFO "minifs-kernel: handle responce\n");

    if (!storing) {
        return -1;
    }

    if (storing == 1) {
        if (count != 3) {
            return -2;
        }

        buffer[0] = status;
        *((uint16_t*) (buffer + 1)) = buffer_size;
        copy_to_user(buf, (char *) buffer, 3);

        if (buffer_size == 0 || status != NO_ERROR) {
            storing = 0;
        } else {
            storing = 2;
        }

        return 3;
    } else {
        if (count != buffer_size) {
            return -2;
        }

        copy_to_user(buf, (char *) (buffer + 3), buffer_size);
        storing = 0;

        return buffer_size;
    }
}


ssize_t scull_write(struct file *flip, const char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_INFO "minifs-kernel: start handle request\n");

    if (storing) {
        return -1;
    } else if (count < 1) {
        return -2;
    }

    uint8_t operation;

    ssize_t global_read = minifs_core_protocol__parse_request(&tp_options, buf, count, buffer_one, &buffer_one_size, buffer_two, &buffer_two_size, buffer, &buffer_size, &operation);
    if (global_read < 0) {
        return global_read;
    }

    int meta = 0;
    uint8_t meta2 = 0;

    switch (operation) {
        case MINIFS_CORE_PROTOCOL__FILE_CREATE_REQUEST_NUM:
            middle_level__file_create(&ctx, buffer_one, (int *) &status);
            buffer_size = 0;
            break;
        case MINIFS_CORE_PROTOCOL__FILE_GET_REQUEST_NUM:
            middle_level__file_get(&ctx, buffer_one, (uint8_t *) (buffer + 3), &meta, (int *) &status);
            buffer_size = (uint16_t) meta;
            break;
        case MINIFS_CORE_PROTOCOL__FILE_DELETE_REQUEST_NUM:
            middle_level__file_delete(&ctx, buffer_one, (int *) &status);
            buffer_size = 0;
            break;
        case MINIFS_CORE_PROTOCOL__FILE_WRITE_REQUEST_NUM:
            middle_level__file_write(&ctx, buffer_one, buffer, (int) buffer_size, (int *) &status);
            buffer_size = 0;
            break;
        case MINIFS_CORE_PROTOCOL__FILE_COPY_REQUEST_NUM:
            middle_level__file_copy(&ctx, buffer_one, buffer_two, buffer, (int *) &status);
            buffer_size = 0;
            break;
        case MINIFS_CORE_PROTOCOL__DIR_CREATE_REQUEST_NUM:
            middle_level__dir_create(&ctx, buffer_one, (int *) &status);
            buffer_size = 0;
            break;
        case MINIFS_CORE_PROTOCOL__DIR_GET_REQUEST_NUM:
            middle_level__dir_get_contents(&ctx, buffer_one, (directory_item_t *) (buffer + 3), &meta2, (int *) &status);
            buffer_size = meta2 * sizeof(directory_item_t);
            break;
        case MINIFS_CORE_PROTOCOL__DIR_DELETE_REQUEST_NUM:
            middle_level__dir_delete(&ctx, buffer_one, (int *) &status);
            buffer_size = 0;
            break;
        default:
            status = MINIFS_ERROR__NOT_ENOUGH_REQUEST_SIZE;
            buffer_size = 0;
            break;
    }

    

    printk(KERN_INFO "minifs-kernel: end handle request\n");
    storing = 1;
    return global_read;
}

int scull_open(struct inode *inode, struct file *flip) {
    printk(KERN_INFO "minifs-kernel: device is opend\n");
    return 0;
}

int scull_release(struct inode *inode, struct file *flip) {
    printk(KERN_INFO "minifs-kernel: device is closed\n");
    return 0;
}

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .read = scull_read,
    .write = scull_write,
    .open = scull_open,
    .release = scull_release,
};

void scull_cleanup_module(void) {
    dev_t devno = MKDEV(scull_major, scull_minor);
    cdev_del(p_cdev);
    unregister_chrdev_region(devno, 1); 
}

static int scull_init_module(void) {
    int rv;
    dev_t dev;
    rv = alloc_chrdev_region(&dev, scull_minor, 1, "scull");
    if (rv) {
        printk(KERN_WARNING "minifs_kernel: can't get major %d\n", scull_major);
        return rv;
    }

    scull_major = MAJOR(dev);

    p_cdev = cdev_alloc();
    cdev_init(p_cdev, &scull_fops);

    p_cdev->owner = THIS_MODULE;
    p_cdev->ops = &scull_fops;

    rv = cdev_add(p_cdev, dev, 1);

    if (rv)
        printk(KERN_NOTICE "Error %d adding minifs_kernel", rv);

    printk(KERN_INFO "minifs_kernel: register device major = %d minor = %d\n", scull_major, scull_minor);

    int error;
    minifs_core__create_in_memory_storage_driver(&imsd);
    ctx.sdi = (struct storage_driver_interface *) &imsd;
    imsd.init(&ctx, &error);

    return 0;
}

MODULE_AUTHOR("Nikolay Vasilev");
MODULE_LICENSE("GPL");

module_init(scull_init_module);
module_exit(scull_cleanup_module);
