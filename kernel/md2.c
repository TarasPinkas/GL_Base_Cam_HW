#include <linux/kernel.h>
#include <linux/module.h>

#include "md.h"


static int __init md_init(void)
{
	printk("module md2 started\n");
	printk("Call md1_proc: %s\n", md1_proc());
	return 0;
}

static void __exit md_exit(void)
{
	printk("md2 end\n");
}

module_init(md_init);
module_exit(md_exit);
