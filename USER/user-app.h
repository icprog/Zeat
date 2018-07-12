/*
**************************************************************************************************************
*	@file	user-app.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief	应用层头文件：连接MAC层
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_APP_H
#define __USER_APP_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "Zeta.h"

typedef enum cmd_s
{
	MAC = 0,
	NETIME = 1,
	COUNTER = 2,
	RSSI = 3,
}cmd_t;

typedef struct d_UserZeta
{
	uint8_t 		Cmd;
	uint32_t		Timeout;
	ZetaState_t Expect_retval;
}UserZeta_t;

extern UserZeta_t UserZetaCheck[];

void UserZetaInit(void);

void UserSend(void);

void UserCheckCmd(UserZeta_t *UserZetaCheckCmd);

void UserSetHeart(uint8_t mode);

void UserSetTimer(ZetaTimer_t Timer);

void UserCloseTimer(ZetaTimer_t Timer);

void UserIntoLowPower(void);

#endif /* __USER_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
