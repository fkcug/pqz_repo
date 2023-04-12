#ifndef _LED_OPR_H
#define _LED_OPR_H

//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

//GPIO5_GDIR地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR地址：0x020AC000
static volatile unsigned int *GPIO5_DR;

struct led_operations{
	int (*init) (int which);
	int (*ctl) (int which, char status);
	int (*get_status) (int which);
};

struct led_operations *get_board_led_opr(void);


#endif
