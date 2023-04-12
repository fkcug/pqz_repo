﻿#ifndef _LED_OPR_H
#define _LED_OPR_H

struct led_operations {
	int (*init) (int which); /* 初始化LED, which-哪个LED */       
	int (*ctl) (int which, char status); /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
};



#endif

