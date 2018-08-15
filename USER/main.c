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

extern UART_HandleTypeDef 			UartHandle;
extern RTC_HandleTypeDef 				RtcHandle;


/*******************************************************************************************************************
  * @��������		main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����ZETA-V0.6

  �Ż����ܣ�
	��1����ʵ�ֻ���ͨѶ
	��2��������Zeta�����������崥��
	��3���������������߻���
	��4��������Zeta��������INT����MCU
	��5��������RTC���߻���
	��6��������Rs485��ȡ����
	��7�������Rs485����չ�е�һ��ʱ���յ���һ��0x00���ݣ�Sensors.QueryPinStaus�е���Rs485s.GetData��
	       ���´��ڽ��յ���һ��0x00
	��8��������WWDG���ڿ��Ź�
	��9��������Timer
	��10�������Ӵ������쳣���˻��ƣ�ʹ��SensorToLen�Ƿ�Ϊ0�жϴ������Ƿ��쳣
	
  *****************************************************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
   BoardInitMcu(  );	
   DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__); 
		
	 UserCheckCmd(&UserZetaCheck[MAC]);

	 UserCheckCmd(&UserZetaCheck[COUNTER]);
	
	 UserCheckCmd(&UserZetaCheck[RSSI]);

	 UserSetHeart(0x00);
	
	if(FlashRead16(SLEEP_ADDR)==0||FlashRead32(SLEEP_ADDR)==0xffff)
	{
			uint16_t time = 5;//Ĭ��300�룬���Ϸ��͹��̴��20��
			FlashWrite16(SLEEP_ADDR,&time,1);
	}

//	uint8_t data[3] = {0xa2, 0x00, 0x01};		//0x11

	 Sensors.QueryPinStaus(  );
	 
   while (1)
   {		
		 Sensors.Handle(  );
		 
		 UserSendSensor(  );
		 DEBUG(2,"test power\r\n");
		 
		 User.SleepTime =	FlashRead16(SLEEP_ADDR);
//		 SetRtcAlarm(20);///4S���	  (User.SleepTime*60)
//		 UserIntoLowPower(  );		
			HAL_Delay(20000);
		  
		 if( User.Ack )
		 {
				User.Ack = false;
			  UserDownCommand(  );
		 }
		 	HAL_Delay(10000);
		 
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

