#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/bitrev.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/io.h>



static struct class *led_class;

static ssize_t led_write(struct file *filp, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	char val;
	int ret;
	ret = copy_from_user(&val, buf, 1);
	/* copy_from_user 得到数据从app */
	/* 来设置gpio寄存器的输出 */
	if(val == 1)
	{
		/*1时点亮led*/
	
	}
	else
	{
		/*非0熄灭*/
	
	}
}
				 
static int led_open(struct inode *inode, struct file *filp)
{
	/* 配置gpio来输出 包括使能power configure pin as gpio 作为gpio来使用而不是复用*/
	return 0;
}

static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.write		= led_write,
	.open		= led_open
};
	

static int __init led_init(void)
{
	/* ioremap 将硬件物理地址映射到虚拟地址 */
	printf("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	major = register_chrdev(0, "my_led_test", &led_fops);//0:系统分配主设备号
	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled");/* 系统创建名为/dev/myled的设备节点 */

	return 0;
}


static void __exit led_exit(void)
{
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	
	unregister_chrdev(major, myled);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
		






















