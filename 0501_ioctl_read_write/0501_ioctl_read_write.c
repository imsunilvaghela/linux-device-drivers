#include <linux/kernel.h>           /* Kernel debug macros and many more */
#include <linux/init.h>             /* __init* and __exit* macros */
#include <linux/module.h>           /* module_* macros, Required by all modules */
#include <linux/kdev_t.h>           /* MAJOR and MINOR macros */
#include <linux/fs.h>               /* struct file_operations */
#include <linux/cdev.h>             /* struct cdev, cdev_* APIs */
#include <linux/device.h>           /* struct class, class_*, device_* APIs */
#include <linux/uaccess.h>          /* copy_to/from_user() */
#include <linux/ioctl.h>			/* __IOX macros */


#define WR_VALUE _IOW('a', 'a', int32_t*)
#define RD_VALUE _IOR('a', 'b', int32_t*)

int32_t value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = etx_read,
	.write          = etx_write,
	.open           = etx_open,
	.unlocked_ioctl = etx_ioctl,
	.release        = etx_release,
};

static int etx_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device File Opened...!!!\n");
	return 0;
}

static int etx_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device File Closed...!!!\n");
	return 0;
}

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Read Function\n");
	return 0;
}
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Write function\n");
	return 0;
}

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case WR_VALUE:
			if(copy_from_user(&value, (int32_t*) arg, sizeof(value)) == 0) {
			    printk(KERN_INFO "Value = %d\n", value);
            }
            else {
                printk(KERN_ERR "copy_from_user failed\n");
            }
			break;
		case RD_VALUE:
			if(copy_to_user((int32_t*) arg, &value, sizeof(value)) != 0) {
                printk(KERN_ERR "copy_to_user failed\n"); 
            }
			break;
	}
	return 0;
}

static int __init etx_driver_init(void)
{
	/* Allocating Major number */
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0) {
		printk(KERN_INFO "Cannot allocate major number\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&etx_cdev, &fops);

	/* Adding character device to the system */
	if((cdev_add(&etx_cdev, dev, 1)) < 0) {
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_cdev;
	}

	/* Creating struct class */
	if((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/* Creating device */
	if((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
		printk(KERN_INFO "Cannot create the Device 1\n");
		goto r_device;
	}
	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&etx_cdev);
r_cdev:
	unregister_chrdev_region(dev,1);
	return -1;
}

void __exit etx_driver_exit(void)
{
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela <sunilvaghela09@gmail.com>");
MODULE_DESCRIPTION("A simple IOCTL device driver");

