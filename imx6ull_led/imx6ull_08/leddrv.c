#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>



/* 1. 确定主设备号                                                                 */
static int major = 0;
static struct class *led_class;
static struct gpio_desc *led_gpio;


/* 3. 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t led_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/* write(fd, &val, 1); */
static ssize_t led_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	char status;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	err = copy_from_user(&status, buf, 1);
	gpiod_set_value(led_gpio, status);

	return 1;
}

static int led_drv_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	gpiod_direction_output(led_gpio, 0);
	
	return 0;
}

static int led_drv_close (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	//gpiod_put(led_gpio);
	return 0;
}

/* 2. 定义自己的file_operations结构体                                              */
static struct file_operations led_drv = {
	.owner	 = THIS_MODULE,
	.open    = led_drv_open,
	.read    = led_drv_read,
	.write   = led_drv_write,
	.release = led_drv_close,
};

static int chip_demo_gpio_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	//从设备树中获取GPIO
	led_gpio = gpiod_get(&pdev->dev, "led", 0);
	if (IS_ERR(led_gpio)) {
		dev_err(&pdev->dev, "Failed to get GPIO for led\n");
		return PTR_ERR(&led_gpio);
	}
	
	//注册file_operations
	major = register_chrdev(0, "zpq_led", &led_drv);

	led_class = class_create(THIS_MODULE, "zpq_led_class");
	if(IS_ERR(led_class)){
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "zpq_led");
		gpiod_put(led_gpio);
		return PTR_ERR(led_class);
	}

	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled%d", 0);//dev/myled0
	
	return 0;
}
static int chip_demo_gpio_remove(struct platform_device *pdev)
{
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	unregister_chrdev(major, "zpq_led");
	gpiod_put(led_gpio);
	return 0;
}




static const struct of_device_id myled_of_match[] = {
	{ .compatible = "myled,zpq"},
	{ },
};


static struct platform_driver chio_demo_gpio_driver = {
	.probe = chip_demo_gpio_probe,
	.remove = chip_demo_gpio_remove,
	.driver 	={
		.name ="myled,zpq",
		.of_match_table= myled_of_match,
	},
};


/* 4. 把file_operations结构体告诉内核：注册驱动程序                 */
/* 5. 入口函数：安装驱动程序时，就会去调用这个入口函数 */
static int __init led_init(void)
{
	int err;
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	err = platform_driver_register(&chio_demo_gpio_driver);
	
	return err;
}


/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */
static void __exit led_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	platform_driver_unregister(&chio_demo_gpio_driver);
}



module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");


