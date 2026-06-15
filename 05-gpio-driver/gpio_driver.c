#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Simulated GPIO Driver");

#define DEVICE_NAME "gpiodev"
#define NUM_PINS     8
#define BUFFER_SIZE  32

static int major;
static struct class *gpio_class;
static struct cdev gpio_cdev;

/* Simulated GPIO pin states */
static int gpio_pins[NUM_PINS] = {0};

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "gpiodev: opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "gpiodev: closed\n");
    return 0;
}

/* Read all pin states */
static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    char buf[BUFFER_SIZE];
    int len, i;

    if (*offset > 0)
        return 0;

    /* Print all 8 pin states */
    len = snprintf(buf, sizeof(buf),
                   "GPIO: %d%d%d%d%d%d%d%d\n",
                   gpio_pins[0], gpio_pins[1],
                   gpio_pins[2], gpio_pins[3],
                   gpio_pins[4], gpio_pins[5],
                   gpio_pins[6], gpio_pins[7]);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    *offset += len;

    printk(KERN_INFO "gpiodev: read pin states\n");
    for (i = 0; i < NUM_PINS; i++)
        printk(KERN_INFO "gpiodev: pin[%d] = %d\n", i, gpio_pins[i]);

    return len;
}

/* Write to control pins
 * Format: "pin=value"  e.g. "3=1" sets pin 3 HIGH
 *                           "3=0" sets pin 3 LOW
 */
static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    char buf[BUFFER_SIZE];
    int pin, value, ret;
    int len = min((int)count, BUFFER_SIZE - 1);

    if (copy_from_user(buf, user_buf, len))
        return -EFAULT;

    buf[len] = '\0';

    /* Parse "pin=value" format */
    ret = sscanf(buf, "%d=%d", &pin, &value);
    if (ret != 2) {
        printk(KERN_ERR "gpiodev: invalid format. Use: pin=value (e.g. 3=1)\n");
        return -EINVAL;
    }

    if (pin < 0 || pin >= NUM_PINS) {
        printk(KERN_ERR "gpiodev: invalid pin %d (0-%d only)\n", pin, NUM_PINS-1);
        return -EINVAL;
    }

    if (value != 0 && value != 1) {
        printk(KERN_ERR "gpiodev: invalid value %d (0 or 1 only)\n", value);
        return -EINVAL;
    }

    gpio_pins[pin] = value;
    printk(KERN_INFO "gpiodev: pin[%d] set to %s\n",
           pin, value ? "HIGH" : "LOW");

    return count;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

static int __init gpio_init(void)
{
    dev_t dev;
    int i;

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&gpio_cdev, &fops);
    cdev_add(&gpio_cdev, dev, 1);
    gpio_class = class_create(DEVICE_NAME);
    device_create(gpio_class, NULL, dev, NULL, DEVICE_NAME);

    /* Initialize all pins LOW */
    for (i = 0; i < NUM_PINS; i++)
        gpio_pins[i] = 0;

    printk(KERN_INFO "gpiodev: registered with %d virtual pins\n", NUM_PINS);
    return 0;
}

static void __exit gpio_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(gpio_class, dev);
    class_destroy(gpio_class);
    cdev_del(&gpio_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "gpiodev: unregistered\n");
}

module_init(gpio_init);
module_exit(gpio_exit);
