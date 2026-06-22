#ifndef _time_H
#define _time_H

#include "system.h"

/* 工作状态定义 */
typedef enum {
	NORMAL_MODE,      /* 正常模式 */
	EMERGENCY_MODE    /* 紧急模式 */
} WorkMode_TypeDef;

/* 交通灯状态定义 */
typedef enum {
	GREEN_ON,         /* 绿灯亮 */
	YELLOW_FLASH,     /* 黄灯闪烁 */
	RED_ON            /* 红灯亮 */
} TrafficState_TypeDef;

void TIM4_Init(u16 per,u16 psc);
void EXTI_PA0_Init(void);
void Traffic_Light_Manager(void);

#endif
