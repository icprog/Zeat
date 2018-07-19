/*
**************************************************************************************************************
*	@file	sensor.h
*	@author 
*	@version 
*	@date    
*	@brief	读取传感器数据
***************************************************************************************************************
*/
#ifndef __RS485_H
#define __RS485_H

#include <stdint.h>
#include "gpio.h"

#ifdef __cplusplus
	extern "C" {
#endif
		
#define NBI_RS485_SEND_DATA_LEN     20 
#define NBI_RS485_REV_DATA_LEN      50 
#define NBI_RS485_REV_TIME_OUT      200
#define NBI_RS485_SEND_BUFF_LEN     13
#define NBI_RS485_PIN_COUNT         6
#define NBI_RS485_EXBOX_COUNT       5


#define NBI_RS485_SEARCH_CODE       0x03
#define NBI_RS485_SET_CODE          0x05
		
 
/*
*rs485_t: Rs485处理功能代码
*/
typedef struct u_rs485
{
	/*
	*接收Rs485传感器数据缓存区
	*/
	uint8_t 	Revbuff[20];
	
	void 			(*PinInit)(void);
	void 			(*OpenPin)(int index);
	void 			(*ClosePin)(void);
	void 			(*PowerOn)(void);
	void 			(*PowerOff)(void);
	uint16_t 	(*Crc16)(uint8_t *data, uint8_t len); 
	int 			(*GetData)(uint8_t *data);
	void      (*Print)(uint8_t *buff,int len, uint8_t DebugLive);
	int 			(*Cmd)(uint8_t *sendData , int len, uint32_t time_out);
}rs485_t;		

extern rs485_t Rs485s;


void Rs485Init(void);

void Rs485PinInit(void);
		
void Rs485OpenPin(int index);
		
void Rs485ClsoePin(void);

void _12VPowerOn(void);

void _12VPowerOff(void);

int Rs485GetData(uint8_t *data);

void Rs485Print(uint8_t *buff,int len, uint8_t DebugLive);

int Rs485Cmd(uint8_t *sendData, int len, uint32_t time_out);

uint16_t CalcCRC16(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
