#include <linux/kernel.h>           /* Kernel debug macros and many more */
#include <linux/init.h>             /* __init* and __exit* macros */
#include <linux/module.h>           /* module_* macros, Required by all modules */
#include <linux/kdev_t.h>           /* MAJOR and MINOR macros */
#include <linux/fs.h>               /* struct file_operations */
#include <linux/cdev.h>             /* struct cdev, cdev_* APIs */
#include <linux/device.h>           /* struct class, class_*, device_* APIs */
#include <linux/wait.h>             /* Required for the wait queues */
#include <linux/kthread.h>          /* kthread_create */

uint32_t read_count = 0;
static struct task_struct *wait_thread = NULL;

DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
int wait_queue_flag = 0;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver Functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
    .owner    =  THIS_MODULE,
    .read     =  etx_read,
    .write    =  etx_write,
    .open     =  etx_open,
    .release  =  etx_release,
};

static int wait_function(void *unused)
{
    while(1) {
        printk(KERN_INFO "Waiting For Event...\n");
        wait_event_interruptible(wait_queue_etx, wait_queue_flag != 0 );
        if(wait_queue_flag == 2) {
            printk(KERN_INFO "Event Came From Exit Function\n");
            return 0;
        }
        printk(KERN_INFO "Event Came From Read Function - %d\n", ++read_count);
        wait_queue_flag = 0;
    }
    do_exit(0);
    return 0;
}

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

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,
                        loff_t *off)
{
    printk(KERN_INFO "Read Function\n");
    wait_queue_flag = 1;
    wake_up_interruptible(&wait_queue_etx);
    return 0;
}

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len,
                         loff_t *off)
{
    printk(KERN_INFO "Write function\n");
    return 0;
}

static int __init etx_driver_init(void)
{
    /* Allocating Major number */
    if((alloc_chrdev_region(&dev, 0, 1, "wq_dev")) < 0) {
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

    /* Creating cdev structure */
    cdev_init(&etx_cdev,&fops);
    etx_cdev.owner = THIS_MODULE;
    etx_cdev.ops = &fops;

    /* Adding character device to the system */
    if((cdev_add(&etx_cdev,dev,1)) < 0) {
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_cdev;
    }

    /* Creating struct class */
    if((dev_class = class_create(THIS_MODULE, "wq_class")) == NULL) {
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }

    /* Creating device */
    if((device_create(dev_class, NULL, dev, NULL, "waitqueue_static")) == NULL) {
        printk(KERN_INFO "Cannot create the Device 1\n");
        goto r_device;
    }

    /* Create the kernel thread */
    wait_thread = kthread_create(wait_function, NULL, "WaitThread");
    if (wait_thread) {
        printk("Kernel thread created successfully\n");
        /* start thread */
        wake_up_process(wait_thread);
    } else {
        printk(KERN_INFO "Thread creation failed\n");
        goto r_kthread;
    }

    printk(KERN_INFO "Waitqueue static probed !!!\n");
    return 0;

r_kthread:
    device_destroy(dev_class,dev);
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
    wait_queue_flag = 2;
    wake_up_interruptible(&wait_queue_etx);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Waitqueue static Removed !!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela <sunilvaghela09@gmail.com>");
MODULE_DESCRIPTION("Demonstration of waitqueue using static initialization");
