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

	版本说明：
	【1】：ZETA-V0.6

  优化功能：
	【1】：实现基本通讯
	【2】：增加Zeta下行数据脉冲触发
	【3】：增加主控休眠机制
	【4】：增加Zeta下行数据INT触发MCU
	【5】：增加RTC休眠唤醒
	【6】：增加Rs485读取数据
	【7】：解决Rs485打开扩展盒第一口时接收到多一个0x00数据，Sensors.QueryPinStaus中调用Rs485s.GetData：
	       导致串口接收到多一个0x00
	【8】：增加WWDG窗口看门狗
	【9】：增加Timer
	【10】：增加传感器异常过滤机制，使用SensorToLen是否为0判断传感器是否异常
	
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
	
	 UserCheckSensors(  );
	
	 UserCheckCmd(&UserZetaCheck[MAC]);

	 UserCheckCmd(&UserZetaCheck[COUNTER]);

	 UserCheckCmd(&UserZetaCheck[RSSI]);

	 UserSetHeart(0x00);

	 User.SleepTime =	FlashRead32(SLEEP_ADDR);
	
   while (1)
   {		
		 SensorTime = HAL_GetTick(  );		

		 UserSendSensor(  );
		 
		 ////上报GPS信息
		 UserSendGps(  ); 
		 
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
		 		 
		 DEBUG_APP(2,"GetPation = %d\r\n",SetGpsAck.GetPation);
		 
		 while(ZetaRecviceBuf.RecvState);
		 SetRtcAlarm(SleepTime);///4S误差	  (User.SleepTime*60) 
		 UserIntoLowPower(  );
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

