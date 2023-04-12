#include <linux/module.h>

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
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
                                                                
static int major = 0;

static struct gpio_key {
	int gpio;
	int irq;
	struct gpio_desc gpiod;
	int flag;
};

static struct gpio_key *gpio_keys;


static ssize_t button_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}


 

/* 2. 定义自己的file_operations结构体          */
static struct file_operations gpio_opr = {
	.owner	 = THIS_MODULE,
	.read    = button_drv_read,
};
	
static irqreturn_t gpio_button_irq(int irq, void *dev_id)
{
	struct gpio_key *gpio_key = dev_id;
	
	printk("key %d val %d\n", gpio_key->gpio, gpiod_get_value(gpio_key->gpiod));
	return IRQ_HANDLED;
}


static int gpio_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	int i;
	int irq;
	int gpio;
	int count;
	enum of_gpio_flags flags;
	struct device_node *gpio_node = pdev->dev.of_node;
	
	count = of_gpio_count(gpio_node);
	if (!count)
	{
		printk("%s %s line %d, there isn't any gpio available\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	gpio_keys = kzalloc(count * sizeof(struct gpio_key), GFP_KERNEL);

	for(i = 0; i < count ; i++)
	{  	
		gpio = of_get_gpio_flags(gpio_node, i, &flags);
		if (gpio_keys_100ask[i].gpio < 0)
		{
			printk("%s %s line %d, of_get_gpio_flags fail\n", __FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
		
		irq = gpio_to_irq(gpio);
		gpio_keys[i].gpio = gpio;
		gpio_keys[i].irq = irq;
		gpio_keys[i].flag = flags;

		request_irq(irq, gpio_button_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "zpq_irq", &gpio_keys[i]);
	}
	return 0;
}

static int gpio_remove(struct platform_device *pdev)
{	
	int i;
	int count;
	struct device_node *node = pdev->dev.of_node;
	
	count = of_gpio_count(node);
	for(i = 0; i < count ; i++)
	{  	
		free_irq(gpio_keys[i].irq, &gpio_keys[i]);
	}
	return 0;
}

static const struct of_device_id gpio_of_match[] = {
	{ .compatible = "gpio_button,zpq"},
	{ },
};

static struct platform_driver gpio_drv = {
	.probe 				= gpio_probe,
	.remove 			= gpio_remove,
	.driver 			= {
		.name			="gpio_button,zpq",
		.of_match_table = gpio_of_match,
	},
};



static int __init gpio_init(void)
{
	int err;
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	err = platform_driver_register(&gpio_drv);
	
	return err;
}


/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */
static void __exit gpio_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	platform_driver_unregister(&gpio_drv);
}



module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");






