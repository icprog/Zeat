/*
**************************************************************************************************************
*	@file	LED.c
*	@author Jason_531@163.com
*	@version V0.0.1
*	@date    
*	@brief Led状态函数
***************************************************************************************************************
*/

#include <stdint.h>
#include "debug.h"
#include "led.h"
#include "gpio.h"

LedStates_t LedStates = NoneCare;

LedStates_t LedSaveStates = NoneCare;

uint32_t		LedTimer = 0;

void Ledrecv(uint32_t *Counter)
{
	__HAL_TIM_DISABLE(&htim2);
	if( *Counter >=  4 ) //20ms
	{
		HAL_GPIO_TogglePin(LED_PORT,LED_PIN_GREEN);
		*Counter = 0;
	}
	__HAL_TIM_ENABLE(&htim2);
}

void Ledsuccess(uint32_t *Counter)
{
	__HAL_TIM_DISABLE(&htim2);
	if( *Counter >= 10 ) ///500ms
	{
		DEBUG(2,"--%s--\r\n",__func__);
		HAL_GPIO_TogglePin(LED_PORT,LED_PIN_GREEN);
		*Counter = 0;
	}
	__HAL_TIM_ENABLE(&htim2);
}

void Ledfail(uint32_t *Counter)
{
	__HAL_TIM_DISABLE(&htim2);
	if( *Counter >= 10 ) ///500ms
	{
		DEBUG(3,"--%s--\r\n",__func__);
		HAL_GPIO_TogglePin(LED_PORT,LED_PIN_RED);
		*Counter = 0;
	}
	__HAL_TIM_ENABLE(&htim2);
}

void Ledgps(uint32_t *Counter)
{
	__HAL_TIM_DISABLE(&htim2);
		
	if( *Counter >= 20 ) ///1000ms
	{
		DEBUG(3,"--%s--\r\n",__func__);
		HAL_GPIO_TogglePin(LED_PORT,LED_PIN_YELLOW);
		*Counter = 0;
	}
	__HAL_TIM_ENABLE(&htim2);
}

/*
*LED亮
*/
void LedOn(void)
{
	HAL_GPIO_WritePin(LED_PORT,LED_PIN_GREEN,GPIO_PIN_SET);
}

void LedOff(void)
{
  HAL_GPIO_WritePin (LED_PORT,LED_PIN_YELLOW,GPIO_PIN_RESET);
}
/*
*LED初始化
*/
void LedInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE(  );          

	GPIO_Initure.Pin=LED_PIN_YELLOW;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  
	GPIO_Initure.Pull=GPIO_PULLUP;          
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     
	HAL_GPIO_Init(LED_PORT,&GPIO_Initure);
	
	LedOff(  );
	
	LedStates = NoneCare;
	LedSaveStates = NoneCare;
	
	printf("LedStates = %d LedSaveStates = %d\r\n",LedStates,LedSaveStates);
}

/*
*LedSetStates：设置LED状态
*/
void LedSetStates(LedStates_t state)
{
	LedSaveStates = LedGetStates(  );
	LedStates = state;
		
	HAL_GPIO_WritePin (LED_PORT,LED_PIN_YELLOW,GPIO_PIN_RESET);
	
	DEBUG(2,"LedStates = %d LedSaveStates = %d\r\n",LedStates, LedSaveStates);
}

/*
*LedGetStates：获取LED状态
*/
LedStates_t LedGetStates(void)
{
	LedStates_t state;
	state = LedStates;
	
	return state;
}

/*
*LedRestStates：还原LED状态
*/
LedStates_t LedRestStates(void)
{
	LedStates = LedSaveStates;
	
	HAL_GPIO_WritePin (LED_PORT,LED_PIN_YELLOW,GPIO_PIN_RESET);

	DEBUG(2,"LedSaveStates = %d\r\n",LedSaveStates);
	return LedStates;
}

/*
*LedDisplay：LED显示处理
*参数：			 无
*返回值：		 无
*/
void LedDisplay(void)
{
	LedTimer ++;

	switch(LedGetStates(  ))
	{
		case NoneCare:
			
		break;
		
		case GpsLocation:
		
		Ledgps(&LedTimer);
			
		break;
		
		case SendSucess:
		Ledsuccess(&LedTimer);	
			
		break;
		
		case SendFail:
		Ledfail(&LedTimer);
			
		break;
		
		case Receive:
			
		Ledrecv(&LedTimer);
			
		break;
		
		default :
			break;
	}
}


