/*
**************************************************************************************************************
*	@file		Zeta.c
*	@author Jason
*	@version 
*	@date    
*	@brief	应用层头文件：连接MAC层
***************************************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "Zeta.h"
#include "usart.h"
#include "Debug.h"

Zeta_t 				ZetaRecviceBuf = {0, NULL, 0, Reset};
Zeta_t 				ZetaSendBuf		 = {0, NULL, 0, Reset};
ZetaTimer_t		ZetaTimer;
ZetaHandle_t 	ZetaHandle;

/*ZetaInit：初始化Zeta IO
*参数：			无
*返回值：   无
*/
void ZetaInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
	__HAL_RCC_GPIOB_CLK_ENABLE();       

	GPIO_Initure.Pin=ZETAINT_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      			
	GPIO_Initure.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(ZETAINT_IO,&GPIO_Initure);
			
	GPIO_Initure.Pin=ZETAWAKUP_PIN|ZETASTATU_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  
	GPIO_Initure.Pull=GPIO_PULLUP;          
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     
	HAL_GPIO_Init(ZETAINT_IO,&GPIO_Initure);
	
	GPIO_Initure.Pin=ZETASTATU_PIN;  
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  
	GPIO_Initure.Pull=GPIO_PULLUP;          
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     
	HAL_GPIO_Init(ZETASTATU_IO,&GPIO_Initure);
	
	HAL_GPIO_WritePin(ZETAINT_IO,ZETAWAKUP_PIN,GPIO_PIN_SET);	
	
	//中断线2-PC2
	HAL_NVIC_SetPriority(EXTI0_1_IRQn,2,0);       //抢占优先级为2，子优先级为0
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);   	
}

/*WakeupZetaEnable：唤醒Zeta,下降沿有效Status IO转换为高电平，1MS后串口可以使用
*参数：							无
*返回值：   				无
*/
void WakeupZetaEnable(void)
{
	HAL_GPIO_WritePin(ZETAINT_IO,ZETASTATU_PIN,  GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ZETAINT_IO,ZETAWAKUP_PIN,  GPIO_PIN_RESET);
}

/*WakeupZetaDisable：Zeta进入休眠
*参数：							 无
*返回值：   				 无
*/
void WakeupZetaDisable(void)
{
	HAL_GPIO_WritePin(ZETAINT_IO,ZETAWAKUP_PIN,  GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ZETAINT_IO,ZETAWAKUP_PIN,GPIO_PIN_SET);
}

/*ZetaInterrupt：Zeta接收数据脉冲
*参数：					 无
*返回值：   		 无
*/
void ZetaInterrupt(void)
{	
	///读取数据
	HAL_Delay(80); ///延时 = Zeta串口稳定接收数据 + 串口超时时间
	ZetaHandle.Recv(  );
	
	DEBUG(2,"%s\r\n",__func__);
}

/*ZetaStatus：Zeta串口状态
*参数：				无
*返回值：   	无
*/
GPIO_PinState ZetaStatus(void)
{
	return HAL_GPIO_ReadPin(ZETASTATU_IO,ZETASTATU_PIN);
}

/*ZetaSend：Zeta发送数据
*参数：			缓存数据集
*返回值：   无
*/
void ZetaSend(Zeta_t *ZetaBuf)
{
	HAL_UART_Transmit(&huart2, ZetaBuf->Buf, ZetaBuf->Len, 0xFFFFFFFF);
	ZetaRecviceBuf.States = Reset;
}

/*ZetaRecv：获取Zeta数据
*参数：			无
*返回值：   Zeta工作状态
*/
ZetaState_t ZetaRecv(void)
{
	if( HAL_GetTick(  )-ZetaRecviceBuf.Uart_time > 20 && UART_RX_DATA2.Rx_State) 
	{
		UART_RX_DATA2.Rx_State = false;
		ZetaRecviceBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*UART_RX_DATA2.USART_RX_Len);///需要分配空间，否则赋值失败
		memcpy(ZetaRecviceBuf.Buf,UART_RX_DATA2.USART_RX_BUF,UART_RX_DATA2.USART_RX_Len);
		ZetaRecviceBuf.Len = UART_RX_DATA2.USART_RX_Len;
		
		memset(UART_RX_DATA2.USART_RX_BUF, 0, UART_RX_DATA2.USART_RX_Len);
		UART_RX_DATA2.USART_RX_Len = 0;
		
		for(uint8_t i = 0; i < ZetaRecviceBuf.Len; i++)
		printf("%02x ",ZetaRecviceBuf.Buf[i]);
		DEBUG(2,"\r\n");
		
		if(ZetaRecviceBuf.Len == 4) ///设置状态反馈
		{
			if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x01 )
				ZetaRecviceBuf.States = DataAck;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x02 )
				ZetaRecviceBuf.States = BufFull;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x03 )
				ZetaRecviceBuf.States = LenError;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x20 )
				ZetaRecviceBuf.States = SetSecess;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x21 )
				ZetaRecviceBuf.States = SetFail;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x40 )
				ZetaRecviceBuf.States = InvaildWarning;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x41 )
				ZetaRecviceBuf.States = StopPrompt;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x42 )
				ZetaRecviceBuf.States = SendDatafalse;
			else if( ZetaRecviceBuf.Buf[2] == 0x04 && ZetaRecviceBuf.Buf[3] == 0x43 )
				ZetaRecviceBuf.States = Unregistered;
		}
		else if(ZetaRecviceBuf.Len >= 5 && ZetaRecviceBuf.Buf[0] == 0xff && ZetaRecviceBuf.Buf[1] == 0x00 )
		{
			ZetaRecviceBuf.States = Payload;
		}
		
		////下行控制数据处理:FF 00 08 30 23 45 67 89：发送数据周期、唤醒MCU发送等 
		
		DEBUG(3,"States: %02x\r\n",ZetaRecviceBuf.States);
		memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
		ZetaRecviceBuf.Len = 0;

		free(ZetaRecviceBuf.Buf);

	}
		return ZetaRecviceBuf.States;
}
