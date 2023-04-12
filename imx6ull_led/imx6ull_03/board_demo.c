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

//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

//GPIO5_GDIR地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR地址：0x020AC000
static volatile unsigned int *GPIO5_DR;


static int board_demo_led_init(int which)
{	
	printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	
	/* ioremap 将硬件物理地址映射到虚拟地址 */
	//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);
	//GPIO5_GDIR地址：0x020AC004
	GPIO5_GDIR = ioremap(0x020AC004, 4);
	//GPIO5_DR地址：0x020AC000
	GPIO5_DR = ioremap(0x020AC000, 4);
	
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

static struct led_operations board_demo_led_opr = {
	.init = board_demo_led_init,
	.ctl = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}






















