#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>	/* for put_user */

static int chr_dev_open(struct inode *, struct file *);
static int chr_dev_release(struct inode *, struct file *);
static ssize_t chr_dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t chr_dev_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "chardev_fops"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 24					/* Max length of the message from the device */
#define NUM_OF_DEVICES	1

static int device_open = 0;		     /* Is device open? */
									 /* Used to prevent multiple access to device */
static char msg[BUF_LEN] = { '\0' }; /* The msg the device will give when asked */
static char *msgptr = NULL;
static dev_t dev;
static struct cdev* p_cdev = NULL;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = chr_dev_read,
	.write = chr_dev_write,
	.open = chr_dev_open,
	.release = chr_dev_release
};

/*
 * This function is called when the module is loaded
 */
static int __init init_chrdev_fops(void)
{
	int ret = 0;
	ret = alloc_chrdev_region(&dev, 0, NUM_OF_DEVICES, DEVICE_NAME);
	if(ret < 0)
	{
		printk(KERN_ERR "alloc_chrdev_region failed with error - %d\n", ret);
		return -1;
	}

	p_cdev = cdev_alloc();
	if(p_cdev == NULL)
	{
		printk(KERN_ERR "cdev_alloc failed\n");
		return -1;	
	}

	p_cdev->owner = THIS_MODULE;
	p_cdev->ops = &fops;

	ret = cdev_add(p_cdev, dev, NUM_OF_DEVICES);
	if(ret < 0)
	{
		printk(KERN_ERR "cdev_add failed with error - %d\n", ret);
		return -1;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", MAJOR(dev));
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, MAJOR(dev));
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");

	return 0;
}

/*
 * This function is called when the module is unloaded
 */
static void __exit clean_chrdev_fops(void)
{
	printk(KERN_INFO "Cleaning up device...\n");
	cdev_del(p_cdev);
	unregister_chrdev_region(dev, NUM_OF_DEVICES);
	printk(KERN_INFO "Device cleanup done\n");
	printk(KERN_INFO "Now please delete the device file /dev/%s\n",
		   DEVICE_NAME);
}

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/chardev_fops"
 */
static int chr_dev_open(struct inode *inode, struct file *file)
{
	static unsigned int counter = 1;

	if (device_open)
	{
		printk(KERN_WARNING "Device is already in use...try again!!!\n");
		return -EBUSY;
	}

	device_open = 1;
	printk(KERN_DEBUG "%s device opened\n", DEVICE_NAME);
	sprintf(msg, "Hello world : %d\n", counter++);
	msgptr = msg;

	return 0;
}

/* 
 * Called when a process closes the device file.
 */
static int chr_dev_release(struct inode *inode, struct file *file)
{
	device_open = 0;		/* We're now ready for our next caller */
	printk(KERN_DEBUG "%s device released\n", DEVICE_NAME);
	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t chr_dev_read(struct file* filp,	/* see include/linux/fs.h   */
			   char __user* buffer,				/* buffer to fill with data */
			   size_t length,					/* length of the buffer     */
			   loff_t* offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	while(*msgptr && length)
	{
		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		if((put_user(*(msgptr++), buffer++)) != 0)
		{
			printk(KERN_ERR "%s: read data failed\n", __func__);
			return -EFAULT;
		}
		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t chr_dev_write(struct file *filp, const char *buff, size_t len,
							loff_t * off)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}

module_init(init_chrdev_fops);
module_exit(clean_chrdev_fops);

MODULE_AUTHOR("Sunil Vaghela");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demonstrate file operations functionality");

