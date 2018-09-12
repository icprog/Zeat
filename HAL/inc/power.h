/*
**************************************************************************************************************
*	@file	power.h
*	@author Ysheng
*	@version V0.0.1
*	@date    
*	@brief	ÑÓÊ±º¯Êý
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POWER_H
#define __POWER_H

#include <stdint.h>

void BatteryInit(void);

uint8_t CheckBattery(void);

uint8_t ReadBattery(void);

void BatEnableCharge(void);

void BatDisableCharge(void);

#endif /* __POWER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
