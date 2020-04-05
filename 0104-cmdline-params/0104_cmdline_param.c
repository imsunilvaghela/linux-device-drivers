#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/init.h>

#define COLORS_NAMES_LEN	40
#define ARRAY_LEN			5

static short int cmd_short = 10;
static int cmd_int = 100;
static long int cmd_long = 10000;
static char* cmd_charp = "Avengers! assemble";
static char cmd_string[COLORS_NAMES_LEN] = "red, green, blue";
static int cmd_int_array[ARRAY_LEN] = {1,2,3,4,5};
static int cmd_int_array_idx = 0;
static char cmd_char_array[ARRAY_LEN] = {'a', 'b', 'c', 'd', 'e'};
static int cmd_char_array_idx = 0;

/* user and group can read/write */
module_param(cmd_short, short, 0660);
MODULE_PARM_DESC(cmd_short, "A short integer number");
/* user and group can read/write */
module_param_named(lucky_number, cmd_int, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(lucky_number, "A lucky number");
/* user and group can read only */
module_param(cmd_long, long, 0);
MODULE_PARM_DESC(cmd_long, "A readonly long number");
/* anyone can (user,group,others) read only */
module_param(cmd_charp, charp, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(cmd_charp, "A readonly string");
/* only user can read and write */
module_param_string(fav_colors, cmd_string, COLORS_NAMES_LEN, 0600);
MODULE_PARM_DESC(fav_colors, "Favourite colors");
/* only user can read */
module_param_array(cmd_int_array, int, &cmd_int_array_idx, 0640);
MODULE_PARM_DESC(cmd_int_array, "An int array of 5");
/* only user can read and write */
module_param_array_named(fav_letters, cmd_char_array, byte, &cmd_char_array_idx, 0600);
MODULE_PARM_DESC(fav_letters, "5 favourite letters");

int __init cmdline_demo_init(void)
{
	int i = 0;
	printk(KERN_INFO "command line demo init\n");
	printk(KERN_INFO "==============================\n");
	printk(KERN_INFO "=> cmd_short : %hd\n", cmd_short);
	printk(KERN_INFO "=> lucky_number : %d\n", cmd_int);
	printk(KERN_INFO "=> cmd_long : %ld\n", cmd_long);
	printk(KERN_INFO "=> cmd_charp : %s\n", cmd_charp);
	printk(KERN_INFO "=> fav_colors : %s\n", cmd_string);
	for(i = 0; i < ARRAY_LEN; i++)
	{	
		printk(KERN_INFO "=> cmd_int_array[%d] : %d\n", i, cmd_int_array[i]);
	}
	for(i = 0; i < ARRAY_LEN; i++)
	{	
		printk(KERN_INFO "=> faourite letter[%d] : %c\n", i, cmd_char_array[i]);
	}
	printk(KERN_INFO "==============================\n");
	return 0;
}

void __exit cmdline_demo_exit(void)
{
	printk(KERN_INFO "command line demo exit\n");
}

module_init(cmdline_demo_init);
module_exit(cmdline_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sunil Vaghela");
MODULE_DESCRIPTION("Demonstrates command line argument passing to a module");
