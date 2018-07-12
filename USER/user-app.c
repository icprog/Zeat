/*
**************************************************************************************************************
*	@file	user-app.c
*	@author Jason_531@163.com
*	@version 
*	@date    
*	@brief	应用层
***************************************************************************************************************
*/
#include <math.h>
#include "user-app.h"
#include "Zeta.h"

UserZeta_t UserZetaCheck[] = {
	{0x10, 1500, Payload}, ///查询mac
	{0x11, 1000, Payload}, ///查询网络时间
	{0x12, 500,  Payload}, ///查询剩发数
	{0x13, 1000, Payload}, ///查询网络质量
};

/*UserSetaInit：用户调用Zeta初始化
*参数：					无
*返回值：   		无
*/
void UserSetaInit(void)
{
	ZetaHandle.Init = ZetaInit;
	ZetaHandle.WakeupEnable = WakeupZetaEnable;
	ZetaHandle.WakeupDisable = WakeupZetaDisable;
	ZetaHandle.Interrupt = ZetaInterrupt;
	ZetaHandle.Status = ZetaStatus;
	ZetaHandle.Send = ZetaSend;
	ZetaHandle.Recv = ZetaRecv;
	
	ZetaHandle.Init(  );
}

/*UserSend：用户调用Zeta发送函数：注意：发送数据前必须等待模块注册完成，否则发送失败，其它模式默认可直接执行
*参数：			无
*返回值：   无
*/
void UserSend(void)
{
	//获取传感器数据
	
	//发送数据
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*14);
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	memcpy(&ZetaSendBuf.Buf[4],"helloworld",10);
	
	ZetaSendBuf.Buf[2] = 0x04 + 10;
	
	ZetaSendBuf.Len = ZetaSendBuf.Buf[2];
	
	for(uint8_t i = 0; i < 3; i++)
	{
		
		DEBUG(2,"start send data\r\n");
		
		for(uint8_t j = 0; j<ZetaSendBuf.Len; j++)
		DEBUG(2,"%02x ",ZetaSendBuf.Buf[j]);
		DEBUG(2,"\r\n");
		
		ZetaHandle.Send(&ZetaSendBuf);
				
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((DataAck != Status) && (HAL_GetTick(  ) - overtime < 200));
		
		if(DataAck == Status)
		{			
//			HAL_Delay(300);	
//			UserCheckCmd(&UserZetaCheck[NETIME]);
			break;
		}
		else if(LenError != Status)
		{
			if(Unregistered == Status)
			{
				DEBUG(2,"---Writing registered---\r\n");
				HAL_Delay(40000);
			}
			else
			HAL_Delay(300);	
		}
		else
			break;			
	}
	
	free(ZetaSendBuf.Buf);
}

/*UserCheckCmd：用户查询Zeta
*参数：					UserZetaCheckCmd：查询命令
*返回值：   		无
*/
void UserCheckCmd(UserZeta_t *UserZetaCheckCmd)
{	
	uint8_t temp[3] = {0xff, 0x00, 0x04};
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*4);
	
	memcpy(&ZetaSendBuf.Buf[0],&temp[0],3);
	
	ZetaSendBuf.Buf[3] = UserZetaCheckCmd->Cmd;
	
	ZetaSendBuf.Len = 4;
	
	for(uint8_t i = 0; i < 3; i++)
	{	
		for(uint8_t j = 0; j<4; j++)
		DEBUG(2,"%02x ",ZetaSendBuf.Buf[j]);
		DEBUG(2,"\r\n");
		
		ZetaHandle.Send(&ZetaSendBuf);
				
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((UserZetaCheckCmd->Expect_retval != Status) && (HAL_GetTick(  ) - overtime < UserZetaCheckCmd->Timeout)); 
		
		if(UserZetaCheckCmd->Expect_retval == Status)
			break;
		else
			HAL_Delay(300);
	}
	
	memset(ZetaSendBuf.Buf, 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
	free(ZetaSendBuf.Buf);   
}

/*UserSetHeart：用户设置Zeta心跳
*参数：					mode: 0--正常模式6H一次心跳，1--测试模式10S一次心跳
*返回值：   		无
*/
void UserSetHeart(uint8_t mode)
{	
	uint8_t temp[4] = {0xff, 0x00, 0x05, 0x22};
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*5);
	
	memcpy(&ZetaSendBuf.Buf[0],&temp[0],4);
	
	ZetaSendBuf.Buf[4] = mode;
	
	ZetaSendBuf.Len = 5;
	
	for(uint8_t i = 0; i < 3; i++)
	{		
		for(uint8_t j = 0; j<5; j++)
		DEBUG(2,"%02x ",ZetaSendBuf.Buf[j]);
		DEBUG(2,"\r\n");
		
		ZetaHandle.Send(&ZetaSendBuf);
		
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((SetSecess != Status) && (HAL_GetTick(  ) - overtime < 200)); 
		
		if(SetSecess == Status)
			break;
		else
		{
			HAL_Delay(300);
		}
	}
	
	memset(ZetaSendBuf.Buf, 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
	free(ZetaSendBuf.Buf);   
}

/*UserSetTimer：设置Zeta定时器时间
*参数：					Timer：设置中断开始时间、间隔时间、中断号
*返回值：   		无
*/
void UserSetTimer(ZetaTimer_t Timer)
{
	uint8_t temp[4] = {0xff, 0x00, 0x0f, 0x20};
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*15);
	
	memcpy(ZetaSendBuf.Buf,temp,4);
	
	memcpy(&ZetaSendBuf.Buf[4], &Timer, 11);
		
	ZetaSendBuf.Len = 15;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		ZetaHandle.Send(&ZetaSendBuf);
		
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((Payload != Status) && (HAL_GetTick(  ) - overtime < 200));
		
		if(Payload == Status)
			break;
	}
	
	memset(ZetaSendBuf.Buf, 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
	free(ZetaSendBuf.Buf);
}

/*UserCloseTimer：关闭Zeta定时器
*参数：						Timer：中断号
*返回值：   			无
*/
void UserCloseTimer(ZetaTimer_t Timer)
{
	uint8_t temp[4] = {0xff, 0x00, 0x05, 0x21};
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*5);

	memcpy(ZetaSendBuf.Buf,temp,4);
	
	ZetaSendBuf.Buf[4] = Timer.TimerID;
		
	ZetaSendBuf.Len = 5;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		ZetaHandle.Send(&ZetaSendBuf);
				
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((Payload != Status) && (HAL_GetTick(  ) - overtime < 200));
		
		if(Payload == Status)
			break;
	}
	
	memset(ZetaSendBuf.Buf, 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
	free(ZetaSendBuf.Buf);
}
