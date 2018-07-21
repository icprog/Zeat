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
	
  *****************************************************************************************************************/
/* variable functions ---------------------------------------------------------*/	

int main(void)
{	
   BoardInitMcu(  );	
   DEBUG(2,"TIME : %s  DATE : %s\r\n",__TIME__, __DATE__); 
	
//	 UserCheckCmd(&UserZetaCheck[MAC]);
//	
//	 UserCheckCmd(&UserZetaCheck[COUNTER]);
//	 
//	 UserCheckCmd(&UserZetaCheck[RSSI]);
//	
//	 UserSetHeart(0x00);
//	
//	 UserSend(  );
			
	Sensors.QueryPinStaus(  );
	 
   while (1)
   {	
//			UserSend(  );
		
		 Sensors.Handle(  );
		 UserSend(  );
		 SetRtcAlarm(20);///4S���	  
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

