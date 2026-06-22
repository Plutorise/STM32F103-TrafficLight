#include "led.h"

/*******************************************************************************
* 函 数 名         : LED_Init
* 函数功能		   : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	
	RCC_APB2PeriphClockCmd(LED0_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED2_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED3_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED4_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED5_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED6_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED7_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LED8_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED0_PIN;  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(LED0_PORT,&GPIO_InitStructure); 	   /* 初始化GPIO */
	GPIO_SetBits(LED0_PORT,LED0_PIN);   //将LED端口拉高，熄灭所有LED
	
	GPIO_InitStructure.GPIO_Pin=LED1_PIN; 
	GPIO_Init(LED1_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED1_PORT,LED1_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED2_PIN; 
	GPIO_Init(LED2_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED2_PORT,LED2_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED3_PIN; 
	GPIO_Init(LED3_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED3_PORT,LED3_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED4_PIN; 
	GPIO_Init(LED4_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED4_PORT,LED4_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED5_PIN; 
	GPIO_Init(LED5_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED5_PORT,LED5_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED6_PIN; 
	GPIO_Init(LED6_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED6_PORT,LED6_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED7_PIN; 
	GPIO_Init(LED7_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED7_PORT,LED7_PIN);
	
	GPIO_InitStructure.GPIO_Pin=LED8_PIN; 
	GPIO_Init(LED8_PORT,&GPIO_InitStructure);
	GPIO_SetBits(LED8_PORT,LED8_PIN);
}

/*******************************************************************************
* 函 数 名         : TrafficLight_Init
* 函数功能		   : 交通灯初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TrafficLight_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 启用PB端口时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* 配置绿灯 PB0 */
	GPIO_InitStructure.GPIO_Pin = GREEN_LIGHT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GREEN_LIGHT_PORT, &GPIO_InitStructure);
	GPIO_SetBits(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN);  /* 初始熄灭 */
	
	/* 配置黄灯 PB1 */
	GPIO_InitStructure.GPIO_Pin = YELLOW_LIGHT_PIN;
	GPIO_Init(YELLOW_LIGHT_PORT, &GPIO_InitStructure);
	GPIO_SetBits(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN);  /* 初始熄灭 */
	
	/* 配置红灯 PB2 */
	GPIO_InitStructure.GPIO_Pin = RED_LIGHT_PIN;
	GPIO_Init(RED_LIGHT_PORT, &GPIO_InitStructure);
	GPIO_SetBits(RED_LIGHT_PORT, RED_LIGHT_PIN);  /* 初始熄灭 */
}

/*******************************************************************************
* 函 数 名         : All_Light_Off
* 函数功能		   : 所有灯熄灭
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void All_Light_Off(void)
{
	GPIO_SetBits(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN);
	GPIO_SetBits(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN);
	GPIO_SetBits(RED_LIGHT_PORT, RED_LIGHT_PIN);
}

/*******************************************************************************
* 函 数 名         : Green_Light_On
* 函数功能		   : 绿灯点亮
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Green_Light_On(void)
{
	All_Light_Off();
	GPIO_ResetBits(GREEN_LIGHT_PORT, GREEN_LIGHT_PIN);
}

/*******************************************************************************
* 函 数 名         : Yellow_Light_On
* 函数功能		   : 黄灯点亮
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Yellow_Light_On(void)
{
	All_Light_Off();
	GPIO_ResetBits(YELLOW_LIGHT_PORT, YELLOW_LIGHT_PIN);
}

/*******************************************************************************
* 函 数 名         : Red_Light_On
* 函数功能		   : 红灯点亮
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Red_Light_On(void)
{
	All_Light_Off();
	GPIO_ResetBits(RED_LIGHT_PORT, RED_LIGHT_PIN);
}

//LED端口8位数据同时操作，不影响高位
//写入数据到8位端口，数据低位对应端口低引脚
//GPIO_Pin：8位端口低位引脚
//data：写入数据
void LED_Write_Data(u16 GPIO_Pin,u8 data)
{
	u8 i,j=GPIO_Pin;    
	for(i=0;i<8;i++)
	{
		if(data&0x01)
			GPIO_WriteBit(GPIOA, j<<i ,Bit_SET); 
		else
			GPIO_WriteBit(GPIOA, j<<i ,Bit_RESET); 
		data = data >> 1 ; 
	}
}
