#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

char *md1_data = "Hellow world";

extern char *md1_proc(void)
{
	return md1_data;
}


EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);

static int __init md_init(void)
{
	printk("module md1 start\n");
	return 0;
}

static void __exit md_exit(void)
{
	printk("module md1 end\n");
}

module_init(md_init);
module_exit(md_exit);
