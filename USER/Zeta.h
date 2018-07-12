/*
**************************************************************************************************************
*	@file		Zeta.h
*	@author Jason
*	@version 
*	@date    
*	@brief	Zeta���ڴ���
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ZETA_H
#define __ZETA_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "stm32l0xx_hal.h"

#define  ZETAINT_IO						GPIOA
#define  ZETAINT_PIN   				GPIO_PIN_0

#define  ZETAWAKUP_IO					GPIOB
#define  ZETAWAKUP_PIN				GPIO_PIN_13

#define  ZETASTATU_IO					GPIOB
#define  ZETASTATU_PIN				GPIO_PIN_14

typedef enum uZetaState
{
	/***********����֡Ӧ��***********/
	DataAck 				= 0x01,
	
	/***********��������***********/
	BufFull 				= 0x02,
	
	/***********���ȳ���***********/
	LenError 				= 0x03,
	
	/***********���óɹ�***********/
	SetSecess 			= 0x20,
	
	/***********����ʧ��***********/
	SetFail 				= 0x21,
	
	/***********��Ч����***********/
	InvaildWarning 	= 0x40,
	
	/***********��ͣ��ʾ***********/
	StopPrompt			= 0x41,
	
	/***********ʣ����Ϊ0***********/
	SendDatafalse 	= 0x42,
	
	/***********�ն�δע��***********/
	Unregistered		= 0x43,
	
	/***********��������Ч����ȡ����/��������***********/
	Payload					= 0x44,
	
	/***********Ĭ�ϳ�ʼ״̬***********/
	Reset						= 0x45
	
}ZetaState_t;

typedef struct uZeta
{
	uint8_t 		Len;
	uint8_t			*Buf;
	uint32_t    Uart_time;
	ZetaState_t States;
}Zeta_t;

typedef struct uZetaTimer
{
	uint8_t 		*StartTime;
	uint8_t			*SpaceTime; ///���ʱ��
	uint8_t    	TimerID;
}ZetaTimer_t;

typedef struct uZetaHandle
{
	void 					(*Init)(void);
	void 					(*WakeupEnable)(void);
	void 					(*WakeupDisable)(void);
	void 					(*Interrupt)(void);
	void 					(*Send)(Zeta_t *ZetaBuf);
	GPIO_PinState (*Status)(void);
	ZetaState_t 	(*Recv)(void);
	
}ZetaHandle_t;


extern Zeta_t 			ZetaRecviceBuf;

extern Zeta_t 			ZetaSendBuf;

extern ZetaTimer_t	ZetaTimer;

extern ZetaHandle_t ZetaHandle;

void ZetaInit(void);

void WakeupZetaEnable(void);

void WakeupZetaDisable(void);

void ZetaInterrupt(void);

GPIO_PinState ZetaStatus(void);

void ZetaSend(Zeta_t *ZetaBuf);

uint8_t ZetaRecv(void);


#endif /* __ZETA_H  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/