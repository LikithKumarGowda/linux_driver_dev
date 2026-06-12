#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Likith");
MODULE_DESCRIPTION("Platform Driver with Device Tree");

static int mydevice_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    const char *label;

    printk(KERN_INFO "mydevice: probe() called — hardware found!\n");

    if (of_property_read_string(dev->of_node, "label", &label) == 0)
        printk(KERN_INFO "mydevice: label from device tree = %s\n", label);
    else
        printk(KERN_INFO "mydevice: no label property found\n");

    return 0;
}

/* Newer kernels use void return type for remove() */
static void mydevice_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "mydevice: remove() called — hardware removed!\n");
}

static const struct of_device_id mydevice_of_match[] = {
    { .compatible = "likith,mydevice" },
    { }
};
MODULE_DEVICE_TABLE(of, mydevice_of_match);

static struct platform_driver mydevice_driver = {
    .probe  = mydevice_probe,
    .remove = mydevice_remove,
    .driver = {
        .name           = "mydevice",
        .of_match_table = mydevice_of_match,
        .owner          = THIS_MODULE,
    },
};

module_platform_driver(mydevice_driver);
