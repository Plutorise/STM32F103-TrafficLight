#include "time.h"
#include "led.h"

/* 全局变量定义 */
WorkMode_TypeDef g_WorkMode = NORMAL_MODE;              /* 工作模式 */
TrafficState_TypeDef g_TrafficState = GREEN_ON;         /* 交通灯状态 */
volatile u32 g_TimerCounter = 0;                        /* 定时器计数器(10ms单位) */
volatile u8 g_KeyPressed = 0;                           /* 按键标志位 */
volatile u8 g_DebounceCounter = 0;                      /* 防抖计数器 */

/*******************************************************************************
* 函 数 名         : TIM4_Init
* 函数功能		   : TIM4初始化函数，用于交通灯控制计时
* 输    入         : per:重装载值
					 psc:分频系数
* 输    出         : 无
* 说    明         : 配置为10ms中断一次
*******************************************************************************/
void TIM4_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //开启定时器中断
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM4,ENABLE); //使能定时器	
}

/*******************************************************************************
* 函 数 名         : EXTI_PA0_Init
* 函数功能		   : 外部中断PA0初始化函数(KEY0按键)
* 输    入         : 无
* 输    出         : 无
* 说    明         : 配置为下降沿触发，用于紧急模式切换
*******************************************************************************/
void EXTI_PA0_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 步骤1: 启用GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* 步骤2: 启用AFIO时钟，用于外部中断配置 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* 步骤3: 配置GPIO为输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  /* 浮空输入 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 步骤4: 配置AFIO映射 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	/* 步骤5: 配置外部中断 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  /* 下降沿触发 */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* 清除EXTI中断标志 */
	EXTI_ClearITPendingBit(EXTI_Line0);
	
	/* 步骤6: 配置NVIC优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函 数 名         : Traffic_Light_Manager
* 函数功能		   : 交通灯管理函数，在定时器中断中调用
* 输    入         : 无
* 输    出         : 无
* 说    明         : 处理灯光切换逻辑
*******************************************************************************/
void Traffic_Light_Manager(void)
{
	static u32 lastCounter = 0;
	
	if(g_WorkMode == NORMAL_MODE)
	{
		/* 正常模式：(1)绿灯15秒 (2)黄灯闪烁3秒 (3)红灯10秒 */
		switch(g_TrafficState)
		{
			case GREEN_ON:
				/* 绿灯常亮15秒 = 1500个10ms */
				if(g_TimerCounter < 1500)
				{
					Green_Light_On();
				}
				else if(g_TimerCounter == 1500)
				{
					g_TrafficState = YELLOW_FLASH;
					lastCounter = g_TimerCounter;
				}
				break;
				
			case YELLOW_FLASH:
				/* 黄灯闪烁3秒 = 300个10ms，500ms亮/灭 = 50个10ms切换一次 */
				if(g_TimerCounter - lastCounter < 300)
				{
					u32 flashCounter = (g_TimerCounter - lastCounter) / 50;
					if(flashCounter % 2 == 0)
					{
						Yellow_Light_On();
					}
					else
					{
						All_Light_Off();
					}
				}
				else if(g_TimerCounter - lastCounter == 300)
				{
					g_TrafficState = RED_ON;
					lastCounter = g_TimerCounter;
				}
				break;
				
			case RED_ON:
				/* 红灯常亮10秒 = 1000个10ms */
				if(g_TimerCounter - lastCounter < 1000)
				{
					Red_Light_On();
				}
				else if(g_TimerCounter - lastCounter == 1000)
				{
					/* 循环回到绿灯 */
					g_TrafficState = GREEN_ON;
					g_TimerCounter = 0;
					lastCounter = 0;
				}
				break;
		}
	}
	else if(g_WorkMode == EMERGENCY_MODE)
	{
		/* 紧急模式：红灯常亮，黄灯快速闪烁(200ms切换) */
		Red_Light_On();
		
		/* 黄灯200ms切换一次 = 20个10ms */
		u32 yellowFlash = (g_TimerCounter / 20) % 2;
		if(yellowFlash == 0)
		{
			/* 点亮黄灯 */
			GPIO_ResetBits(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN);
		}
		else
		{
			/* 熄灭黄灯 */
			GPIO_SetBits(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN);
		}
	}
}

/*******************************************************************************
* 函 数 名         : TIM4_IRQHandler
* 函数功能		   : TIM4中断函数，每10ms执行一次
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{
		g_TimerCounter++;
		Traffic_Light_Manager();
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
}

/*******************************************************************************
* 函 数 名         : EXTI0_IRQHandler
* 函数功能		   : 外部中断0处理函数(KEY0按键)
* 输    入         : 无
* 输    出         : 无
* 说    明         : 软件防抖处理，防止连续误触发
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITPendingBit(EXTI_Line0))
	{
		/* 防抖处理：在中断���读取按键状态 */
		/* 延迟约20ms（可在此处使用简单的计数延迟） */
		u32 debounceCount = 0;
		for(debounceCount = 0; debounceCount < 100000; debounceCount++);
		
		/* 确认按键仍然被按下（下降沿之后） */
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
		{
			/* 按键确实被按下，执行中断逻辑 */
			if(g_WorkMode == NORMAL_MODE)
			{
				/* 进入紧急模式 */
				g_WorkMode = EMERGENCY_MODE;
			}
			else
			{
				/* 退出紧急模式，返回正常模式，从绿灯开始 */
				g_WorkMode = NORMAL_MODE;
				g_TrafficState = GREEN_ON;
				g_TimerCounter = 0;
			}
		}
		
		/* 清除中断标志 */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
