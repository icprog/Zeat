/*
**************************************************************************************************************
*	@file	main.c
*	@author Ysheng
*	@version V1.1
*	@date    2017/2/23
*	@brief	NBI_LoRaWAN���ܴ���: add OTAA
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


/*******************************************************************************
  * @��������		main
  * @����˵��   ������ 
  * @�������   ��
  * @�������   ��
  * @���ز���   ��

	�汾˵����
	��1����ZETA-V0.3

  �Ż����ܣ�
	��1����ʵ�ֻ���ͨѶ
	��2��������Zeta�����������崥��
	��3���������������߻���
	��4��������Zeta��������INT����MCU
	��5��������RTC���߻���
	
  *****************************************************************************/
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
		SensorsInit(  );
	  	 
   while (1)
   {	
//			UserSend(  );
//			SetRtcAlarm(10);///4S���	  
//			UserIntoLowPower(  );
		 Sensors.Handle(  );
//		 
		 HAL_Delay(30000);
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

