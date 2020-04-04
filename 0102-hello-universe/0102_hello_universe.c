#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

int __init hello_init(void)
{
	printk(KERN_DEBUG "Hello Universe!\n");
	return 0;
}

void __exit hello_exit(void)
{
	printk(KERN_DEBUG "GoodBye Universe\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela");
MODULE_DESCRIPTION("Hello Universe");
