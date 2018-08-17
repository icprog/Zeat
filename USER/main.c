/*
**************************************************************************************************************
*	@file			main.c
*	@author 	Jason
*	@version  V0.6
*	@date    2018/07/18
*	@brief	 Zeta通讯
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

extern UART_HandleTypeDef 			UartHandle;
extern RTC_HandleTypeDef 				RtcHandle;


/*******************************************************************************************************************
  * @函数名称		main
  * @函数说明   主函数 
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无	
  *****************************************************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
   BoardInitMcu(  );
	
	 UserCheckSensors(  );
	
#if 1
   DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__); 
			 	 				
	 UserCheckCmd(&UserZetaCheck[MAC]);

	 UserCheckCmd(&UserZetaCheck[COUNTER]);
	
	 UserCheckCmd(&UserZetaCheck[RSSI]);

	 UserSetHeart(0x00);
	
	 if(FlashRead16(SLEEP_ADDR)==0||FlashRead32(SLEEP_ADDR)==0xffff)
	{
		uint16_t time = 1;//默认300秒，加上发送过程大概20秒
		FlashWrite16(SLEEP_ADDR,&time,1);
	 }
	
	 uint8_t data[2] = {0xa1,0x00};
	 uint8_t temp = ZetaHandle.CRC8( data,2 );  //0x0b
	 
	 DEBUG_APP(2,"temp = %02x",temp);
	 	 
   while (1)
   {				 
		 UserSendSensor(  );
		 
		 ////上报GPS信息
		 UserSendGps(  );  ///SetGpsAck.PationBuf
		 
		 if(GetLedStates(  ) == NoneCare)
		 {
			 DEBUG_APP(2,"GetPation = %d\r\n",SetGpsAck.GetPation);
			 User.SleepTime =	FlashRead16(SLEEP_ADDR);
			 SetRtcAlarm(User.SleepTime*60);///4S误差	  (User.SleepTime*60)
			 UserIntoLowPower(  );
		 }		
//			HAL_Delay(20000);
	 }
#endif 
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

