#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/bitrev.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>
#include "led_opr.h"

#define LED_NUM 2

static int major;
static struct class *led_class;
struct led_operations *p_ledopr;


static ssize_t led_read(struct file *filp, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

				 
static int led_open(struct inode *inode, struct file *filp)
{
	int minor = iminor(inode);	//这里open函数本身有inode这个参数，可以直接用，但是write没有

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	//根据次设备号初始化led
	p_ledopr->init(minor);

	return 0;
}


static ssize_t led_write(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	int err;
	char status;

	err = copy_from_user(&status, buf, 1);
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	//根据次设备号和status控制LED

	//获取次设备号
	struct inode *inode = file_inode(file);//wrtie没有inode参数，要先获得一遍
	int minor = iminor(inode) ;
	p_ledopr->ctl(minor, status);
		
	return 1;
}
		
static const struct file_operations led_fops = {
	.owner   = THIS_MODULE,
	.open	  = led_open,
	.read	  = led_read,
	.write   = led_write,

};

static int __init led_init(void)
{
	int i;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	p_ledopr = get_board_led_opr();
	major = register_chrdev(0, "zpq_led_test02", &led_fops);//0:系统分配主设备号
	
	led_class = class_create(THIS_MODULE, "myled02");
	for( i = 0; i < LED_NUM; i++)
	{
		device_create(led_class, NULL, MKDEV(major, i), NULL, "myled0%d", i);/* 系统创建名为/dev/myled的设备节点 */
	}
	

	return 0;
}


static void __exit led_exit(void)
{
	int i;
	
	for(i = 0; i < LED_NUM; i++)
	{
		device_destroy(led_class, MKDEV(major, i));
	}
	class_destroy(led_class);
	
	unregister_chrdev(major, "zpq_led_test02");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
		




















