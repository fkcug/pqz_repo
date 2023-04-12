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



static int board_demo_led_init(int which)
{	
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	if(!IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3)//如果之前没映射过
	{	
		IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);
		GPIO5_GDIR = ioremap(0x020AC004, 4);
		GPIO5_DR = ioremap(0x020AC000, 4);
	}


	
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0x5;
	*GPIO5_GDIR |= (1<<3);//0x08 (1<<3)

	return 0;
}

static int board_demo_led_ctl(int which, char status)
{
	printk("%s %s line %d, led %d\n, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	
	if(status)
	{
		/*非0点亮*/
		*GPIO5_DR &= ~(1<<3);
	}
	else
	{
		/*0时熄灭*/
		*GPIO5_DR |= (1<<3);
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






















