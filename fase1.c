#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


MODULE_LICENSE("FBG");

static int __init my_init(void) {
	printk(KERN_INFO "Ei! salutacions desde  LKM\n");
	return 0;
}

static void __exit my_exit(void) {
	printk(KERN_INFO "Adeu amic! from  LKM\n");
	return ;
}

module_init(my_init);
module_exit(my_exit);
