#include <linux/kernel.h>           /* Kernel debug macros and many more */
#include <linux/init.h>             /* __init* and __exit* macros */
#include <linux/module.h>           /* module_* macros, Required by all modules */
#include <linux/kdev_t.h>           /* MAJOR and MINOR macros */
#include <linux/fs.h>               /* struct file_operations */
#include <linux/cdev.h>             /* struct cdev, cdev_* APIs */
#include <linux/device.h>           /* struct class, class_*, device_* APIs */
#include <linux/uaccess.h>          /* copy_to/from_user() */
#include <linux/sysfs.h>            /* __ATTR macro, sysfs_* APIs */ 
#include <linux/kobject.h>          /* struct kobject , kernel_kobj */

volatile int etx_value = 0;
  
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
struct kobject *kobj_ref;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver Fuctions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);

/*************** Sysfs Fuctions **********************/
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count);

/* etx_value is name of a sysfs file would be created under /sys/kernel/etx_sysfs/ */
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);
 
static struct file_operations fops =
{
        .owner    =  THIS_MODULE,
        .read     =  etx_read,
        .write    =  etx_write,
        .open     =  etx_open,
        .release  =  etx_release
};
 
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buf)
{
        printk(KERN_INFO "Sysfs - Read!!!\n");
        return sprintf(buf, "%d\n", etx_value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count)
{
        printk(KERN_INFO "Sysfs - Write!!!\n");
        sscanf(buf,"%d",&etx_value);
        return count;
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
        printk(KERN_INFO "Read function\n");
        return 0;
}
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len,
                         loff_t *off)
{
        printk(KERN_INFO "Write Function\n");
        return 0;
}
 
 
static int __init etx_driver_init(void)
{
        /* Allocating Major number */
        if((alloc_chrdev_region(&dev, 0, 1, "etx_dev")) <0) {
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
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL) {
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /* Creating device */
        if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
 
        /* Creating a directory in /sys/kernel/ */
        kobj_ref = kobject_create_and_add("etx_sysfs", kernel_kobj);
 
        /*Creating sysfs file for etx_value*/
        if(sysfs_create_file(kobj_ref, &etx_attr.attr)) {
            printk(KERN_INFO"Cannot create sysfs file......\n");
            goto r_sysfs;
        }

        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;

r_sysfs:
        kobject_put(kobj_ref); 
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
        kobject_put(kobj_ref); 
        sysfs_remove_file(kernel_kobj, &etx_attr.attr);
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
MODULE_DESCRIPTION("A simple device driver - SysFs");
