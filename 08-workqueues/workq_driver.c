#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Kernel Timer + Workqueue Driver");

#define DEVICE_NAME "workqdev"
#define TIMER_INTERVAL_MS 2000

static int major;
static struct class *workq_class;
static struct cdev workq_cdev;
static struct timer_list my_timer;
static int counter = 0;
static int timer_running = 0;

/* Work item - this runs in process context, can sleep */
static struct work_struct my_work;

static void work_handler(struct work_struct *work)
{
    printk(KERN_INFO "workqdev: workqueue task started (can sleep here)\n");

    /* Simulate heavy work - this would crash in a timer callback! */
    msleep(500);

    counter++;
    printk(KERN_INFO "workqdev: workqueue task finished, counter = %d\n", counter);
}

/* Timer callback - runs in interrupt context, must be FAST */
static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "workqdev: timer fired (interrupt context, scheduling work)\n");

    /* Hand off heavy work to workqueue instead of doing it here */
    schedule_work(&my_work);

    if (timer_running)
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
}

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "workqdev: opened\n");

    if (!timer_running) {
        timer_running = 1;
        timer_setup(&my_timer, timer_callback, 0);
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
        printk(KERN_INFO "workqdev: timer started\n");
    }
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "workqdev: closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    char buf[32];
    int len;

    if (*offset > 0)
        return 0;

    len = snprintf(buf, sizeof(buf), "Work completed: %d\n", counter);

    if (copy_to_user(user_buf, buf, len))
        return -EFAULT;

    *offset += len;
    return len;
}

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
        printk(KERN_INFO "workqdev: timer stopped\n");
    } else if (strncmp(buf, "start", 5) == 0) {
        if (!timer_running) {
            timer_running = 1;
            mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
            printk(KERN_INFO "workqdev: timer restarted\n");
        }
    } else if (strncmp(buf, "reset", 5) == 0) {
        counter = 0;
        printk(KERN_INFO "workqdev: counter reset\n");
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

static int __init workq_init(void)
{
    dev_t dev;

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    major = MAJOR(dev);
    cdev_init(&workq_cdev, &fops);
    cdev_add(&workq_cdev, dev, 1);
    workq_class = class_create(DEVICE_NAME);
    device_create(workq_class, NULL, dev, NULL, DEVICE_NAME);

    /* Initialize the work item with our handler function */
    INIT_WORK(&my_work, work_handler);

    printk(KERN_INFO "workqdev: registered — open /dev/workqdev to start\n");
    return 0;
}

static void __exit workq_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    timer_running = 0;
    timer_delete_sync(&my_timer);
    cancel_work_sync(&my_work);

    device_destroy(workq_class, dev);
    class_destroy(workq_class);
    cdev_del(&workq_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "workqdev: unregistered\n");
}

module_init(workq_init);
module_exit(workq_exit);
