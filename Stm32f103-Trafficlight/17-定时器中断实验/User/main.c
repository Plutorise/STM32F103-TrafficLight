#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "time.h"

int main()
{
	/* 系统初始化 */
	SysTick_Init(72);
	
	/* 配置中断优先级分组：分为2组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* 初始化LED和交通灯 */
	LED_Init();
	TrafficLight_Init();
	
	/* 初始化定时器4，用于交通灯控制计时 */
	/* TIM4配置参数：
	   - 频率计算：TIM4 = APB1 = 36MHz
	   - 分频系数(PSC) = 36000-1，使计数频率为 36MHz/36000 = 1KHz (1ms计数一次)
	   - 重装载值(ARR) = 1000-1，使中断周期为 1000ms/1000 = 1ms (实际是10ms，见下面配置)
	   - 实际：为了得到10ms中断，使用ARR=100-1，PSC=3600-1
	   - 这里采用 ARR=1000-1, PSC=3600-1 得到 10ms 中断
	*/
	TIM4_Init(1000-1, 3600-1);  /* 定时10ms */
	
	/* 初始化外部中断PA0(KEY0按键)，用于紧急模式切换 */
	EXTI_PA0_Init();
	
	/* 主循环 - 中断完全处理所有逻辑，主循环中不做任何处理 */
	while(1)
	{
		/* 所有工作都由中断驱动 */
		/* 定时器中断：每10ms切换一次灯光 */
		/* 外部中断：按键切换工作模式 */
	}
}
