#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("I2C Temperature Sensor Driver");

#define DEVICE_NAME "tempdev"
#define I2C_BUS      1       /* i2c-1 */
#define SENSOR_ADDR  0x48    /* virtual temp sensor address */

static int major;
static struct class *temp_class;
static struct cdev temp_cdev;
static struct i2c_client *temp_client;

/* Simulated temperature register */
static int simulated_temp = 25; /* 25 degrees Celsius */

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "tempdev: opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "tempdev: closed\n");
    return 0;
}

/* Read temperature */
static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    char buf[32];
    int len;

    if (*offset > 0)
        return 0;

    /* In real driver: simulated_temp = i2c_smbus_read_byte(temp_client); */
    len = snprintf(buf, sizeof(buf), "Temperature: %d C\n", simulated_temp);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    *offset += len;
    printk(KERN_INFO "tempdev: read temperature = %d C\n", simulated_temp);
    return len;
}

/* Write to set simulated temperature */
static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    char buf[16];
    int temp, len = min((int)count, 15);

    if (copy_from_user(buf, user_buf, len))
        return -EFAULT;

    buf[len] = '\0';

    if (sscanf(buf, "%d", &temp) == 1) {
        simulated_temp = temp;
        /* In real driver: i2c_smbus_write_byte(temp_client, temp); */
        printk(KERN_INFO "tempdev: temperature set to %d C\n", simulated_temp);
    } else {
        return -EINVAL;
    }

    return count;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

/* Called when kernel matches I2C device */
static int temp_probe(struct i2c_client *client)
{
    dev_t dev;

    temp_client = client;

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&temp_cdev, &fops);
    cdev_add(&temp_cdev, dev, 1);
    temp_class = class_create(DEVICE_NAME);
    device_create(temp_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "tempdev: probe() called — I2C sensor found at 0x%02x\n",
           client->addr);
    printk(KERN_INFO "tempdev: /dev/tempdev created\n");
    return 0;
}

/* Called when I2C device is removed */
static void temp_remove(struct i2c_client *client)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(temp_class, dev);
    class_destroy(temp_class);
    cdev_del(&temp_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "tempdev: removed\n");
}

/* I2C device ID table */
static const struct i2c_device_id temp_id[] = {
    { "virtual-temp-sensor", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, temp_id);

/* Device tree match table */
static const struct of_device_id temp_of_match[] = {
    { .compatible = "likith,virtual-temp-sensor" },
    { }
};
MODULE_DEVICE_TABLE(of, temp_of_match);

/* I2C driver structure */
static struct i2c_driver temp_driver = {
    .driver = {
        .name           = "virtual-temp-sensor",
        .of_match_table = temp_of_match,
    },
    .probe    = temp_probe,
    .remove   = temp_remove,
    .id_table = temp_id,
};

module_i2c_driver(temp_driver);
