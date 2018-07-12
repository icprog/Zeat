/*
**************************************************************************************************************
*	@file	LED.c
*	@author Jason_531@163.com
*	@version V0.0.1
*	@date    
*	@brief Led×´Ì¬º¯Êý
***************************************************************************************************************
*/

#include <stdint.h>
#include "debug.h"
#include "led.h"

LedStates_t LedStates;

void PowerEnable_Led(void)
{
	HAL_GPIO_WritePin(LORA_LED,LORA_LED_PIN,GPIO_PIN_SET);
}

void PowerDisble_Led(void)
{
	HAL_GPIO_WritePin(LORA_LED,LORA_LED_PIN,GPIO_PIN_RESET);
}

