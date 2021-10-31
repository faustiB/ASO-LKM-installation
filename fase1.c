#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


MODULE_LICENSE("FBG");

static int __init my_init(void) {
	return 0;
}

static void __exit my_exit(void) {
	return ;
}

module_init(my_init);
module_exit(my_exit);
