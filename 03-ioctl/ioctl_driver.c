#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include "chardev_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("IOCTL Driver");

#define DEVICE_NAME "ioctldev"
#define BUFFER_SIZE 1024

static int major;
static char buffer[BUFFER_SIZE];
static int buffer_len = 0;
static struct class *ioctl_class;
static struct cdev ioctl_cdev;

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "ioctldev: opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "ioctldev: closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    int bytes = min((int)count, buffer_len - (int)*offset);
    if (bytes <= 0) return 0;
    if (copy_to_user(user_buf, buffer + *offset, bytes))
        return -EFAULT;
    *offset += bytes;
    return bytes;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    buffer_len = min((int)count, BUFFER_SIZE - 1);
    if (copy_from_user(buffer, user_buf, buffer_len))
        return -EFAULT;
    buffer[buffer_len] = '\0';
    return buffer_len;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {

    case IOCTL_CLEAR:
        memset(buffer, 0, BUFFER_SIZE);
        buffer_len = 0;
        printk(KERN_INFO "ioctldev: buffer cleared\n");
        break;

    case IOCTL_GET_LEN:
        if (copy_to_user((int __user *)arg, &buffer_len, sizeof(int)))
            return -EFAULT;
        printk(KERN_INFO "ioctldev: buffer length = %d\n", buffer_len);
        break;

    case IOCTL_SET_MSG:
        buffer_len = strncpy_from_user(buffer, (char __user *)arg, BUFFER_SIZE - 1);
        if (buffer_len < 0) return -EFAULT;
        buffer[buffer_len] = '\0';
        printk(KERN_INFO "ioctldev: message set via ioctl: %s\n", buffer);
        break;

    default:
        return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = dev_open,
    .release        = dev_release,
    .read           = dev_read,
    .write          = dev_write,
    .unlocked_ioctl = dev_ioctl,
};

static int __init ioctl_init(void)
{
    dev_t dev;
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&ioctl_cdev, &fops);
    cdev_add(&ioctl_cdev, dev, 1);
    ioctl_class = class_create(DEVICE_NAME);
    device_create(ioctl_class, NULL, dev, NULL, DEVICE_NAME);
    printk(KERN_INFO "ioctldev: registered with major %d\n", major);
    return 0;
}

static void __exit ioctl_exit(void)
{
    dev_t dev = MKDEV(major, 0);
    device_destroy(ioctl_class, dev);
    class_destroy(ioctl_class);
    cdev_del(&ioctl_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "ioctldev: unregistered\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);
