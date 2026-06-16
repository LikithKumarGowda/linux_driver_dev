#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Interrupt/Timer Driver");

#define DEVICE_NAME "timerdev"
#define TIMER_INTERVAL_MS 2000  /* 2 seconds */

static int major;
static struct class *timer_class;
static struct cdev timer_cdev;
static struct timer_list my_timer;
static int counter = 0;
static int timer_running = 0;

/* This function is called every 2 seconds — like an interrupt handler */
static void timer_callback(struct timer_list *t)
{
    counter++;
    printk(KERN_INFO "timerdev: timer fired! counter = %d\n", counter);

    /* Reschedule the timer to fire again */
    if (timer_running)
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
}

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "timerdev: opened\n");

    /* Start the timer when device is opened */
    if (!timer_running) {
        timer_running = 1;
        timer_setup(&my_timer, timer_callback, 0);
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
        printk(KERN_INFO "timerdev: timer started\n");
    }
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "timerdev: closed\n");
    return 0;
}

/* Read current counter value */
static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    char buf[32];
    int len;

    if (*offset > 0)
        return 0;

    len = snprintf(buf, sizeof(buf), "Timer count: %d\n", counter);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    *offset += len;
    return len;
}

/* Write "stop" to stop timer, "start" to start it */
static ssize_t dev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    char buf[16];
    int len = min((int)count, 15);

    if (copy_from_user(buf, user_buf, len))
        return -EFAULT;

    buf[len] = '\0';

    if (strncmp(buf, "stop", 4) == 0) {
        timer_running = 0;
        timer_delete_sync(&my_timer);
        printk(KERN_INFO "timerdev: timer stopped\n");
    } else if (strncmp(buf, "start", 5) == 0) {
        if (!timer_running) {
            timer_running = 1;
            mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
            printk(KERN_INFO "timerdev: timer restarted\n");
        }
    } else if (strncmp(buf, "reset", 5) == 0) {
        counter = 0;
        printk(KERN_INFO "timerdev: counter reset\n");
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

static int __init timer_init(void)
{
    dev_t dev;

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&timer_cdev, &fops);
    cdev_add(&timer_cdev, dev, 1);
    timer_class = class_create(DEVICE_NAME);
    device_create(timer_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "timerdev: registered — open /dev/timerdev to start\n");
    return 0;
}

static void __exit timer_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    timer_running = 0;
    timer_delete_sync(&my_timer);

    device_destroy(timer_class, dev);
    class_destroy(timer_class);
    cdev_del(&timer_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "timerdev: unregistered\n");
}

module_init(timer_init);
module_exit(timer_exit);
