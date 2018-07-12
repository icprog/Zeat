/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdbool.h>
#include "board.h"
#include "rtc-board.h"

RTC_HandleTypeDef RtcHandle;

/***********************************以下为RTC底层代码部分***************************************/
/* RTC init function */
void RTC_Init(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	/**Initialize RTC Only 
	*/
	RtcHandle.Instance = RTC;
	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = 127; 
	RtcHandle.Init.SynchPrediv = 255; 
	
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
	{
		printf("HAL_RTC_Init error \r\n");
		Error_Handler();
	}

	/**Initialize RTC and set the Time and Date 
	*/
	RTC_TimeStruct.TimeFormat = RTC_HOURFORMAT12_AM;	
	RTC_TimeStruct.Hours = 0;
	RTC_TimeStruct.Minutes = 0;
	RTC_TimeStruct.Seconds = 0;
	RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&RtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	RTC_DateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
	RTC_DateStruct.Month = RTC_MONTH_JANUARY;
	RTC_DateStruct.Date = 1;
	RTC_DateStruct.Year = 17;

	if (HAL_RTC_SetDate(&RtcHandle, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}	
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(RTC_IRQn);

  }
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */

/*******************************以上为RTC底层代码部分****************************************/

/*SetRtcAlarm：设置RTC闹钟休眠唤醒
*注意：RTC闹钟事件：HAL_RTC_SetAlarm_IT( &RtcHandle, &RTC_AlarmStructure, RTC_FORMAT_BIN )
*      必须设置为：RTC_FORMAT_BIN 否则唤醒失败
*/
void SetRtcAlarm(uint16_t time)
{
	RTC_AlarmTypeDef sAlarm;
	RTC_TimeTypeDef RTC_TimeStruct;
	uint8_t Hours,Minutes,Seconds = 0;

	HAL_RTC_GetTime(&RtcHandle, &RTC_TimeStruct, RTC_FORMAT_BIN);
	Hours = RTC_TimeStruct.Hours;
	Minutes = RTC_TimeStruct.Minutes;
	Seconds = RTC_TimeStruct.Seconds;
	
	sAlarm.AlarmTime.Hours = (Hours+(Minutes+(Seconds+time)/60)/60)%24;
	sAlarm.AlarmTime.Minutes = (Minutes+(Seconds+time)/60)%60;
	sAlarm.AlarmTime.Seconds = (Seconds+time)%60;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1;
	sAlarm.Alarm = RTC_ALARM_A;
	
	DEBUG(2, " 111 hour : %d min : %d second : %d \r\n",	RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		assert_param( FAIL );
	} 
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
