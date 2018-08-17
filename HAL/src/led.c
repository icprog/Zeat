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

LedStates_t LedSaveState;

void SetLedStates(LedStates_t States)
{
	LedSaveState = GetLedStates(  );
	
	DEBUG_APP(2,"LedSaveState = %d",LedSaveState);

	LedStates = States;
	
	DEBUG_APP(2,"LedStates = %d",LedStates);
}

/*
*»¹Ô­LED×´Ì¬
*/
void RestLedStates(LedStates_t States)
{
	LedStates = States;
	
	DEBUG_APP(2,"RestLedStates = %d",LedStates);
}

LedStates_t GetLedStates(void)
{
	LedStates_t States;
	
	States = LedStates;
	
	return States;
}

void LedInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE(  );          

	GPIO_Initure.Pin=LED_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  
	GPIO_Initure.Pull=GPIO_PULLUP;          
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     
	HAL_GPIO_Init(LED_PORT,&GPIO_Initure);
	
	SetLedStates(NoneCare);
	
	LedOff(  );
}

void LedOn(void)
{
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
}

void LedOff(void)
{
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
}

void LedToggle(void)
{
	HAL_GPIO_TogglePin(LED_PORT,LED_PIN);
}

uint32_t LedTime = 0;
uint8_t  LedCounter = 0;
void LedDisplay(void)
{
	LedTime++;
	
	switch(GetLedStates(  ))
	{
		case GpsLocation:
			
			if(1000 <= LedTime)
			{
				DEBUG_APP(2,"LedStates = %d",GpsLocation);
				LedToggle(  );	
				LedTime = 0;
			}
			
		break;
		
		case SendSucess:
			
			if(500 <= LedTime)
			{
				DEBUG_APP(3,"LedStates = %d",SendSucess);

				LedToggle(  );	
				LedTime = 0;
				
				LedCounter ++;
			}
			
		break;
		
		case SendFail:   ////³¤ÁÁ1S£¬¶ÌÃð200ms
			 if( LedTime <= 1000)
       {
          LedOn(  );
       }
       else if( LedTime <= 1200)
       {				 				
				  LedOff(  );
					LedTime = 0;	
					LedCounter++;			 
       }
			 
			 if(LedCounter == 4)
			 {
					SetLedStates(NoneCare);
			 }
			
		break;
		
		case Receive:
			if(200 <= LedTime)
			{
				LedToggle(  );
				LedTime = 0;
				LedCounter ++;
			}
			
		break;
		
		default:
			
		break;
	}
}
