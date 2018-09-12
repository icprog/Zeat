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

extern uint32_t LedTimer;

void Ledrecv(uint32_t *Counter);
void Ledsuccess(uint32_t *Counter);
void Ledfail(uint32_t *Counter);
void Ledgps(uint32_t *Counter);
void LedOn(void);
void LedOff(void );
void LedInit(void);

LedStates_t LedRestStates(void);

LedStates_t LedGetStates(void);

void LedSetStates(LedStates_t state);

void LedDisplay(void);

#endif /* __LED_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
