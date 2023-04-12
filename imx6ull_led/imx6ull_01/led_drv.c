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



static int major;
static struct class *led_class;

//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

//GPIO5_GDIR地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR地址：0x020AC000
static volatile unsigned int *GPIO5_DR;


static ssize_t led_write(struct file *filp, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	char val;
	int ret;
	ret = copy_from_user(&val, buf, 1);
	/* copy_from_user 得到数据从app */
	/* 来设置gpio寄存器的输出 */
	if(val)
	{
		/*非0点亮*/
		*GPIO5_DR &= ~(1<<3);
	}
	else
	{
		/*0时熄灭*/
		*GPIO5_DR |= (1<<3);
	}
	return 1;
}
				 
static int led_open(struct inode *inode, struct file *filp)
{
	/* enable gpio5
	 * configure gpio5_io3 as gpio
	 * configure gpio5 as output
	 */
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0x5;

	*GPIO5_GDIR |= (1<<3);//0x08 (1<<3)
	return 0;
}

static const struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.write		= led_write,
	.open		= led_open,
};
	

static int __init led_init(void)
{

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	major = register_chrdev(0, "zpq_led_test", &led_fops);//0:系统分配主设备号
	
	/* ioremap 将硬件物理地址映射到虚拟地址 */
	//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);
	//GPIO5_GDIR地址：0x020AC004
	GPIO5_GDIR = ioremap(0x020AC004, 4);
	//GPIO5_DR地址：0x020AC000
	GPIO5_DR = ioremap(0x020AC000, 4);

	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled");/* 系统创建名为/dev/myled的设备节点 */

	return 0;
}


static void __exit led_exit(void)
{
	iounmap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
	iounmap(GPIO5_GDIR);
	iounmap(GPIO5_DR);
	
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	
	unregister_chrdev(major, "zpq_led_test");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
		






















