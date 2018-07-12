/*
**************************************************************************************************************
*	@file	led.h
*	@author Jason_531@163.com
*	@version V0.0.1
*	@date    
*	@brief	ÑÓÊ±º¯Êý
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#include <stdint.h>
#include "timer.h"

typedef enum ledStates
{
    NoneCare = 0,
    GpsLocation,
    SendSucess,
    SendFail,
    Receive,
}LedStates_t;

extern LedStates_t LedStates;

extern TimerEvent_t LedTimer;

void OnLedTimerEvent( void );

void SetLedStates(uint8_t States);

uint8_t GetLedStates(void);

void PowerEnable_Led(void);

void PowerDisble_Led(void);

void SendDone_Led(void);

void Error_Led(void);


#endif /* __LED_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
