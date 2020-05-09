#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define CHARDEV_NAME 			"0203_chardev"
#define CHARDEV_CLASS_NAME 		"0203_chardev_class"
#define DEVICE_FILE_NAME 		"chrdev_null"
#define NUM_OF_DEVICES    		1

static dev_t first;         		/* first device number */
static struct cdev c_dev;   		/* character device structure */
static struct class* devclass;    	/* device class */

static int my_open(struct inode* i, struct file* f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int my_close(struct inode* i, struct file* f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file* f, char __user* buf, size_t len, loff_t* off)
{
    printk(KERN_INFO "Driver: read()\n");
    return 0;
}

static ssize_t my_write(struct file* f, const char __user* buf, size_t len, loff_t* off)
{
    printk(KERN_INFO "Driver: write()\n");
    return len;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static int __init chardev_class_example_init(void) /* Constructor */
{
    int ret;
    struct device* dev_ret;

    if((ret = alloc_chrdev_region(&first, 0, NUM_OF_DEVICES, CHARDEV_NAME)) < 0)
    {
		printk(KERN_ERR "alloc_chrdev_region() failed, error:%d\n", ret);
        return ret;
    }

    if(IS_ERR(devclass = class_create(THIS_MODULE, CHARDEV_CLASS_NAME)))
    {
		printk(KERN_ERR "class_create() failed\n");
        ret = PTR_ERR(devclass);
		goto UNREG_CHRDEV;
    }

    if(IS_ERR(dev_ret = device_create(devclass, NULL, first, NULL, DEVICE_FILE_NAME)))
    {
		printk(KERN_ERR "device_create() failed\n");
        ret = PTR_ERR(dev_ret);
		goto DEST_CLASS;
    }

    cdev_init(&c_dev, &fops);
    if((ret = cdev_add(&c_dev, first, NUM_OF_DEVICES)) < 0)
    {
		printk(KERN_ERR "cdev_add() failed, error:%d\n", ret);
		goto DEST_DEVICE;
    }
    printk(KERN_INFO "0203_chardev: device registered\n");

	/* success */
	return 0;

DEST_DEVICE:
	device_destroy(devclass, first);
DEST_CLASS:
	class_destroy(devclass);
UNREG_CHRDEV:
	unregister_chrdev_region(first, NUM_OF_DEVICES);

    return ret;
}

static void __exit chardev_class_example_exit(void) /* Destructor */
{
    cdev_del(&c_dev);
    device_destroy(devclass, first);
    class_destroy(devclass);
    unregister_chrdev_region(first, NUM_OF_DEVICES);
    printk(KERN_INFO "0203_chardev: device unregistered\n");
}

module_init(chardev_class_example_init);
module_exit(chardev_class_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela <sunilvaghela09@gmail.com>");
MODULE_DESCRIPTION("A demonstration of device class APIs to create the device "
				   "file automatically");
