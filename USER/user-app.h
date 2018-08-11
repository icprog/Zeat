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

#define GPSEXIST					0x31

typedef enum cmd_s
{
	MAC = 0,
	NETIME = 1,
	COUNTER = 2,
	RSSI = 3,
}cmd_t;

typedef struct d_UserZeta
{
	/**************Zeta通讯命令************/
	uint8_t 		Cmd;
	
	/**************Zeta通讯超时************/
	uint32_t		Timeout;
	
	/**************Zeta通讯回复状态************/
	ZetaState_t Expect_retval;
}UserZeta_t;


typedef struct f_User
{
	uint16_t SleepTime;
}User_t;

extern UserZeta_t UserZetaCheck[];

extern uint8_t DeviceInfo[4];

extern volatile uint16_t	UpSeqCounter;

void UserInit(void);

void UserSend(void);

void UserCheckCmd(UserZeta_t *UserZetaCheckCmd);

void UserSetHeart(uint8_t mode);

void UserSetTimer(ZetaTimer_t Timer);

void UserCloseTimer(ZetaTimer_t Timer);

void UserIntoLowPower(void);

void UserGetAddID(void);

void String_Conversion(char *str, uint8_t *src, uint8_t len);

#endif /* __USER_APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
