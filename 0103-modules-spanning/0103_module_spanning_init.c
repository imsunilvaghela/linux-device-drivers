#include <linux/module.h>
#include <linux/kernel.h>

int __init module_split_init(void)
{
	printk(KERN_DEBUG "Welcome to the universe of linux!\n");
	return 0;
}

module_init(module_split_init);
