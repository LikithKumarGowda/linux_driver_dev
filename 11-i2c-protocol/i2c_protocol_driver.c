#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("I2C Protocol Driver with Register Map");

#define DEVICE_NAME "i2cproto"

/* Simulated register map - like a real sensor chip */
#define REG_CONFIG  0x00
#define REG_TEMP    0x01
#define REG_STATUS  0x02
#define NUM_REGS    3

static int major;
static struct class *proto_class;
static struct cdev proto_cdev;
static struct i2c_client *proto_client;

/* Simulated device registers */
static u8 virtual_regs[NUM_REGS] = {
    [REG_CONFIG] = 0x00,  /* sensor disabled by default */
    [REG_TEMP]   = 25,    /* default temp */
    [REG_STATUS] = 0x00,  /* no flags set */
};

/* Simulates i2c_smbus_read_byte_data() against our virtual chip */
static int virtual_read_reg(u8 reg)
{
    if (reg >= NUM_REGS)
        return -EINVAL;
    return virtual_regs[reg];
}

/* Simulates i2c_smbus_write_byte_data() against our virtual chip */
static int virtual_write_reg(u8 reg, u8 value)
{
    if (reg >= NUM_REGS)
        return -EINVAL;

    virtual_regs[reg] = value;

    /* Side-effect: enabling the sensor sets a status bit, like real hardware */
    if (reg == REG_CONFIG) {
        if (value & 0x01)
            virtual_regs[REG_STATUS] |= 0x01;  /* "running" flag */
        else
            virtual_regs[REG_STATUS] &= ~0x01;
    }
    return 0;
}

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "i2cproto: opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "i2cproto: closed\n");
    return 0;
}

/* userspace writes "reg=val" e.g. "1=30" to set REG_TEMP to 30 */
static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    char buf[32];
    unsigned int reg, val;
    int len = min((int)count, 31);

    if (copy_from_user(buf, user_buf, len))
        return -EFAULT;
    buf[len] = '\0';

    if (sscanf(buf, "%u=%u", &reg, &val) != 2) {
        printk(KERN_ERR "i2cproto: bad format, use reg=value\n");
        return -EINVAL;
    }

    if (virtual_write_reg((u8)reg, (u8)val) < 0)
        return -EINVAL;

    /* In real hardware this would be:
     *   i2c_smbus_write_byte_data(proto_client, reg, val);
     */
    printk(KERN_INFO "i2cproto: wrote reg[0x%02x] = 0x%02x\n", reg, val);
    return count;
}

/* userspace reads back all registers as text */
static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    char buf[128];
    int len, i;

    if (*offset > 0)
        return 0;

    len = snprintf(buf, sizeof(buf),
                    "CONFIG=0x%02x TEMP=%d STATUS=0x%02x\n",
                    virtual_regs[REG_CONFIG],
                    virtual_regs[REG_TEMP],
                    virtual_regs[REG_STATUS]);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    *offset += len;

    /* In real hardware this would be a series of:
     *   i2c_smbus_read_byte_data(proto_client, REG_x);
     */
    for (i = 0; i < NUM_REGS; i++)
        printk(KERN_INFO "i2cproto: reg[0x%02x] = 0x%02x\n", i, virtual_read_reg(i));

    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

static int proto_probe(struct i2c_client *client)
{
    dev_t dev;

    proto_client = client;

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&proto_cdev, &fops);
    cdev_add(&proto_cdev, dev, 1);
    proto_class = class_create(DEVICE_NAME);
    device_create(proto_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "i2cproto: probe() — virtual sensor at 0x%02x\n", client->addr);
    return 0;
}

static void proto_remove(struct i2c_client *client)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(proto_class, dev);
    class_destroy(proto_class);
    cdev_del(&proto_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "i2cproto: removed\n");
}

static const struct i2c_device_id proto_id[] = {
    { "i2c-proto-dev", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, proto_id);

static const struct of_device_id proto_of_match[] = {
    { .compatible = "likith,i2c-proto-dev" },
    { }
};
MODULE_DEVICE_TABLE(of, proto_of_match);

static struct i2c_driver proto_driver = {
    .driver = {
        .name           = "i2c-proto-dev",
        .of_match_table = proto_of_match,
    },
    .probe    = proto_probe,
    .remove   = proto_remove,
    .id_table = proto_id,
};

module_i2c_driver(proto_driver);
