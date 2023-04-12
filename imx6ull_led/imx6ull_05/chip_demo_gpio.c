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
#include "led_resource.h"


static struct led_resource *led_rsc;


static int board_demo_led_init(int which)
{	
	if(!led_rsc)
	{
		led_rsc = get_led_resource();
	}
	printk("init GPIO group %d, pin %d\n", GROUP(led_rsc->pin), PIN(led_rsc->pin));
	
	switch(GROUP(led_rsc->pin))
	{
		case 0:
		{
			printk("init pin of group 0 ...\n");
		}
		case 1:
		{
			printk("init pin of group 1 ...\n");
		}
		case 2:
		{
			printk("init pin of group 2 ...\n");
		}
		case 3:
		{
			printk("init pin of group 3 ...\n");
		}	
		
	}
	

	return 0;
}

static int board_demo_led_ctl(int which, char status)
{
	printk("%s %s line %d, led %d\n, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	
	switch(PIN(led_rsc->pin))
	{
		case 0:
		{
			printk("set pin %d of group %d ...\n", PIN(led_rsc->pin), GROUP(led_rsc->pin));
		}
		case 1:
		{
			printk("set pin %d of group %d ...\n", PIN(led_rsc->pin), GROUP(led_rsc->pin));
		}
		case 2:
		{
			printk("set pin %d of group %d ...\n", PIN(led_rsc->pin), GROUP(led_rsc->pin));
		}
		case 3:
		{
			printk("set pin %d of group %d ...\n", PIN(led_rsc->pin), GROUP(led_rsc->pin));
		}	
		
	}
	return 0;
}


static int board_demo_led_status(int which)
{
	printk("%s %s line %d, led %d\n, ", __FILE__, __FUNCTION__, __LINE__, which);
	
	unsigned int val;
	int flag;
	val = *GPIO5_DR;
	flag = (val &= (1<<3));
	if(flag)
		return 0;
	else 
		return 1;
}


static struct led_operations board_demo_led_opr = {
	.num = 10,
	.init = board_demo_led_init,
	.ctl = board_demo_led_ctl,
	.get_status = board_demo_led_status,
};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}






















