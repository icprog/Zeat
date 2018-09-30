/*
**************************************************************************************************************
*	@file			main.c
*	@author 	Jason
*	@version  V0.6
*	@date    2018/07/18
*	@brief	 ZetaͨѶ
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <math.h>
#include "stm32l0xx_hal.h"
#include "usart.h"
#include "rtc-board.h"
#include "timer.h"
#include "delay.h"
#include "board.h"
#include "user-app.h"

#define 	RTCTIKE	 

extern UART_HandleTypeDef 			UartHandle;
extern RTC_HandleTypeDef 				RtcHandle;


/*******************************************************************************************************************
  * @��������		main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��	
  *****************************************************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{		
	 uint32_t SensorTime = 0;
	 uint32_t OverTime = 0;
	 uint32_t SleepTime = 0;
	
   BoardInitMcu(  );
	
	 DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__); 
		
	 UserReadFlash(  );
	
	///��ȡ����������ͬʱ��¼ʱ�䣬ȷ������ʱ����һ��
	 SensorTime = HAL_GetTick(  );
	 UserCheckSensors(  );
						 	 						
	 UserCheckCmd(&UserZetaCheck[MAC]);

	 UserCheckCmd(&UserZetaCheck[COUNTER]);
	
	 UserCheckCmd(&UserZetaCheck[RSSI]);

	 UserSetHeart(0x00);

		while (1)
   {	
#if 1	
		 
		 ///���߻��ѻ�ȡ����������ͬʱ��¼ʱ�䣬ȷ������ʱ����һ��
		 if(User.Sleep)
		 SensorTime = HAL_GetTick(  );		

		 UserSendSensor(  );
		 
		  ////�ϱ�GPS��Ϣ
		 UserSendGps(  ); 
		 
		 ///����ǰУ׼RTCʱ��
		 RtcvRtcCalibrate(  );
		 
		 OverTime = HAL_GetTick(  ) - SensorTime;
		 
		 OverTime /= 1000;
		 		 
		 DEBUG_APP(2,"User.SleepTime = %d OverTime = %d\r\n",User.SleepTime,OverTime);
		 		 
		 if(OverTime>User.SleepTime * 60)
		 {
				SleepTime = 60;
		 }
		 else		
		 {
				SleepTime = User.SleepTime * 60 - OverTime;
		 }

		 User.Sleep = true;
		 DEBUG_APP(2,"GetPation = %d\r\n",SetGpsAck.GetPation);
		 SetRtcAlarm(SleepTime);///4S���	  (User.SleepTime*60) 
		 UserIntoLowPower(  );
#endif		
	 } 
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{ 
	DEBUG(2,"error\r\n");
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

