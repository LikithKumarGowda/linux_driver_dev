#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Character Device Driver");

#define DEVICE_NAME "chardev"
#define BUFFER_SIZE 1024

static int major;
static char buffer[BUFFER_SIZE];
static int buffer_len = 0;
static struct class *chardev_class;
static struct cdev chardev_cdev;

/* Called when userspace opens /dev/chardev */
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "chardev: device opened\n");
    return 0;
}

/* Called when userspace closes /dev/chardev */
static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "chardev: device closed\n");
    return 0;
}


static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    int bytes_to_read = min((int)count, buffer_len - (int)*offset);

    if (bytes_to_read <= 0)
        return 0;  /* EOF — tell cat to stop */

    if (copy_to_user(user_buf, buffer + *offset, bytes_to_read))
        return -EFAULT;

    *offset += bytes_to_read;
    printk(KERN_INFO "chardev: sent %d bytes to userspace\n", bytes_to_read);
    return bytes_to_read;
}

/* Called when userspace writes to /dev/chardev */
static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    buffer_len = min((int)count, BUFFER_SIZE - 1);

    if (copy_from_user(buffer, user_buf, buffer_len))
        return -EFAULT;

    buffer[buffer_len] = '\0';
    printk(KERN_INFO "chardev: received %d bytes from userspace: %s\n",
           buffer_len, buffer);
    return buffer_len;
}

/* Maps userspace file operations to our functions */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

static int __init chardev_init(void)
{
    dev_t dev;

    /* Allocate a major number dynamically */
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);

    /* Register the device */
    cdev_init(&chardev_cdev, &fops);
    cdev_add(&chardev_cdev, dev, 1);

    /* Create /dev/chardev automatically */
    chardev_class = class_create(DEVICE_NAME);
    device_create(chardev_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "chardev: registered with major number %d\n", major);
    return 0;
}

static void __exit chardev_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(chardev_class, dev);
    class_destroy(chardev_class);
    cdev_del(&chardev_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "chardev: unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
