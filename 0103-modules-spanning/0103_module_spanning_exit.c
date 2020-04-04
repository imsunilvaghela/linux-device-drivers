#include <linux/module.h>
#include <linux/kernel.h>

void module_split_exit(void)
{
	printk(KERN_DEBUG "Remember! There is no exit once you entered!\n");
}

module_exit(module_split_exit);
