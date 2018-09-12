/*
**************************************************************************************************************
*	@file	gpio-board.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief	GPIO
***************************************************************************************************************
*/
#include "board.h"
#include "gpio-board.h"
#include "Zeta.h"

void EXTI0_1_IRQHandler( void )
{
#if !defined( USE_NO_TIMER )
 //   RtcRecoverMcuStatus( );
#endif
	
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );

}

void EXTI2_3_IRQHandler( void )
{
#if !defined( USE_NO_TIMER )
 //   RtcRecoverMcuStatus( );
#endif
	
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
	
}


void EXTI4_15_IRQHandler( void )
{
#if !defined( USE_NO_TIMER )
 //   RtcRecoverMcuStatus( );
#endif
	
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );
	HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );

}


/**
  * @brief  EXTI callback
  * @param  EXTI : EXTI handle
  * @retval None
	* @brief	中断回调函数：处理中断事件----进行IO判断，处理相应的DIO0---DIO5
  */
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
//	printf("xiaxing");
   	switch(GPIO_Pin)
    {
			case GPIO_PIN_1:		
      if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1))
       {
	       printf("下行数据：");
				 
				 ZetaRecviceBuf.RecvState = true; 
				 
				 ZetaHandle.Interrupt(  );	
				 
				 LedRestStates(  );  ///可能打断发送数据LED状态,先恢复现场
				 
				 LedSetStates(Receive);
				 
				 HAL_Delay(2000);
				 
				 LedRestStates(  );
				 
				 UserDownCommand(  );
				 
				 ZetaRecviceBuf.RecvState = false; 
			 }
			default	:
				break;
   }
	

}
