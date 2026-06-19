#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Sysfs Driver Example");

static struct kobject *sysfs_kobj;
static int counter = 0;
static char message[64] = "Hello from sysfs";

/* Called when userspace reads: cat /sys/kernel/sysfs_driver/counter */
static ssize_t counter_show(struct kobject *kobj, struct kobj_attribute *attr,
                             char *buf)
{
    return sprintf(buf, "%d\n", counter);
}

/* Called when userspace writes: echo 5 > /sys/kernel/sysfs_driver/counter */
static ssize_t counter_store(struct kobject *kobj, struct kobj_attribute *attr,
                              const char *buf, size_t count)
{
    int val;

    if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

    counter = val;
    printk(KERN_INFO "sysfs_driver: counter set to %d\n", counter);
    return count;
}

/* Called when userspace reads message attribute */
static ssize_t message_show(struct kobject *kobj, struct kobj_attribute *attr,
                             char *buf)
{
    return sprintf(buf, "%s\n", message);
}

/* Called when userspace writes message attribute */
static ssize_t message_store(struct kobject *kobj, struct kobj_attribute *attr,
                              const char *buf, size_t count)
{
    int len = min((int)count, 63);
    strncpy(message, buf, len);
    message[len] = '\0';

    /* Strip trailing newline if present */
    if (len > 0 && message[len-1] == '\n')
        message[len-1] = '\0';

    printk(KERN_INFO "sysfs_driver: message set to '%s'\n", message);
    return count;
}

/* Define the attributes - links show/store functions to filenames */
static struct kobj_attribute counter_attr = __ATTR(counter, 0664, counter_show, counter_store);
static struct kobj_attribute message_attr = __ATTR(message, 0664, message_show, message_store);

/* Group all attributes together */
static struct attribute *attrs[] = {
    &counter_attr.attr,
    &message_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static int __init my_sysfs_init(void)
{
    int ret;

    /* Create /sys/kernel/sysfs_driver/ */
    sysfs_kobj = kobject_create_and_add("sysfs_driver", kernel_kobj);
    if (!sysfs_kobj)
        return -ENOMEM;

    /* Create the attribute files inside it */
    ret = sysfs_create_group(sysfs_kobj, &attr_group);
    if (ret) {
        kobject_put(sysfs_kobj);
        return ret;
    }

    printk(KERN_INFO "sysfs_driver: loaded — check /sys/kernel/sysfs_driver/\n");
    return 0;
}

static void __exit my_sysfs_exit(void)
{
    sysfs_remove_group(sysfs_kobj, &attr_group);
    kobject_put(sysfs_kobj);
    printk(KERN_INFO "sysfs_driver: unloaded\n");
}

module_init(my_sysfs_init);
module_exit(my_sysfs_exit);
