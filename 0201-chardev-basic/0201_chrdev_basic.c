#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define NUM_OF_DEVICES  3
#define CHRDEV_NAME     "chrdev_basic"

static dev_t dev;

static int __init chrdev_init(void)
{
    int ret = 0;
    ret = alloc_chrdev_region(&dev, 0, NUM_OF_DEVICES, CHRDEV_NAME);
    if(ret < 0)
    {
        printk(KERN_ERR "alloc_chrdev_region failed with error - %d\n", ret);
        return -1;
    }

    printk(KERN_INFO "chrdev_basic devices region allocated\n");
    printk(KERN_INFO "Major number: %d\n", MAJOR(dev));

    printk(KERN_INFO "chrdev_basic module loaded\n");
    return 0;
}


static void __exit chrdev_exit(void)
{
    unregister_chrdev_region(dev, NUM_OF_DEVICES);
    printk(KERN_INFO "Removed chrdev_basic module\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_AUTHOR("Sunil Vaghela");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic character device module");

