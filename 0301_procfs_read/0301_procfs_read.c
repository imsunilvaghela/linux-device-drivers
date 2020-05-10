#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>  /* Necessary because we use the proc fs */
#include <linux/uaccess.h>  /* For copy_to_user/copy_from_user */

#define procfs_name "helloworld"

static struct proc_dir_entry *pd_entry;      /* Hold information about /proc file */

static int procfile_read(struct file* filp, char *buf, size_t count, loff_t *offp)
{
    static int i = 1;
    int ret = 0;
    char tmp[20] = { 0 };

    printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);
    if(*offp > 0)
    {
        return 0;
    }
    
    sprintf(tmp, "helloworld-%d\n", i++);
    if(copy_to_user(buf, tmp, strlen(tmp)))
    {
        printk(KERN_ERR "Error in copy to user\n");
        return -EFAULT;
    }
    ret = *offp = strlen(tmp);

    return ret;
}

static struct file_operations proc_fops = /* for /proc operations */
{
    owner : THIS_MODULE,
    read  : procfile_read
};

static int __init proc_module_init(void)
{
    pd_entry = proc_create(procfs_name, 0, NULL, &proc_fops);
    
    if(pd_entry == NULL) {
        remove_proc_entry(procfs_name, NULL);
        printk(KERN_ERR "Could not initialize /proc/%s\n", procfs_name);
        return -ENOMEM;
    }

    printk(KERN_INFO "/proc/%s created\n", procfs_name);    
    printk(KERN_INFO "Try using \"cat /proc/%s\"\n", procfs_name);   
    return 0;
}

static void __exit proc_module_exit(void)
{
    remove_proc_entry(procfs_name, NULL);
    printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}

module_init(proc_module_init);
module_exit(proc_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela <sunilvaghela09@gmail.com>");
MODULE_DESCRIPTION("Example to create and read /proc/ entries");
