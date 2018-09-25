/*
**************************************************************************************************************
*	@file	rtc-board.h
*	@author Ysheng
*	@version 
*	@date    
*	@brief	RTCʱ�������ļ�
***************************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_BOARD_H__
#define __RTC_BOARD_H__

#include <stdio.h>
#include <stdint.h>
#include "timer.h"
#include "board.h"

extern RTC_HandleTypeDef RtcHandle;
/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint64_t TimerTime_t;
#endif

/*!
 * \brief Initializes the RTC timer
 *
 * \remark The timer is based on the RTC
 */
void RTC_Init(void);

void RtcvRtcCalibrate(void);

void SetRtcAlarm(uint16_t time);


#endif // __RTC_BOARD_H__
