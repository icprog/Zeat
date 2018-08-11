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
#include "sensor.h"
#include "gps.h"

#define VERSIOS					0x01

volatile uint16_t	UpSeqCounter = 1; 


UserZeta_t UserZetaCheck[] = {
	{0x10, 1500, Payload}, ///查询mac
	{0x11, 1000, Payload}, ///查询网络时间
	{0x12, 500,  Payload}, ///查询剩发数
	{0x13, 1000, Payload}, ///查询网络质量
};

uint8_t DeviceInfo[4] = {0};

/*UserInit：	用户调用初始化
*参数：				无
*返回值：   	无
*/
void UserInit(void)
{		
	UserGetAddID(  );

	ZetaHandle.Init(  );
		
	if(GPSEXIST == DeviceInfo[1])
	{
		Gps.Init(  );
		Gps.Set(  );
	}
}


/*UserSend：用户调用Zeta发送函数：注意：发送数据前必须等待模块注册完成，否则发送失败，其它模式默认可直接执行,
*						最大发送数据MAX = 49
*参数：			无
*返回值：   无
*/
void UserSend(void)
{
	//获取传感器数据
	
	//发送数据
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*49);
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|充电状态
	
	/********************设备ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
	
	SendBufsCounter = 1;
			
	for(uint8_t SedId = 0; SedId <= SendBufsCounter - 1; SedId++)
	{
		memcpy1(&ZetaSendBuf.Buf[9], SendBufs[SedId].Buf, SendBufs[SedId].Len); ///payload
				
		ZetaSendBuf.Buf[9] += 1; 
		
		ZetaSendBuf.Buf[9] |= (SendBufsCounter<<4);  ///总包数|当前第几包
		
		DEBUG(2,"ZetaSendBuf: ");
		for(uint8_t i = 0; i < SendBufs[SedId].Len; i++)
		DEBUG(2,"%02X ",ZetaSendBuf.Buf[9+i]);
		DEBUG(2,"\r\n");
		
		ZetaSendBuf.Buf[9 + SendBufs[SedId].Len++] = (UpSeqCounter&0xff00)<<8; ///Seq
		ZetaSendBuf.Buf[9 + SendBufs[SedId].Len++] = (UpSeqCounter&0xff);
		
		ZetaSendBuf.Buf[9 + SendBufs[SedId].Len++] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[9],SendBufs[SedId].Len); ///CRC
	
		ZetaSendBuf.Buf[2] = 0x09 + SendBufs[SedId].Len; /// +sensor_len
		ZetaSendBuf.Len = ZetaSendBuf.Buf[2];
						
		for(uint8_t i = 0; i < 3; i++)
		{
			
			DEBUG(2,"start send data\r\n");
			
			for(uint8_t j = 0; j<ZetaSendBuf.Len; j++)
			DEBUG(2,"%02X ",ZetaSendBuf.Buf[j]);
			DEBUG(2,"\r\n");
			
			ZetaHandle.Send(&ZetaSendBuf);
			
			/********************缓存清除*******************/
			memset(&ZetaSendBuf.Buf[9], 0, ZetaSendBuf.Len);
			SendBufs[SedId].Len = 0;
			
			HAL_Delay(100);
			ZetaState_t  Status = ZetaHandle.Recv(  );
			
			uint32_t overtime = HAL_GetTick(  );
			while((DataAck != Status) && (HAL_GetTick(  ) - overtime < 200));
						
			if(DataAck == Status)
			{			
	//			HAL_Delay(300);	
	//			UserCheckCmd(&UserZetaCheck[NETIME]); ///结合外部flash使用
				
				if(SedId == SendBufsCounter - 1)
				UpSeqCounter ++;
				break;
			}
			else if(LenError != Status)
			{
				if(Unregistered == Status)
				{
					DEBUG(2,"---Writing registered---\r\n");
					i = 1;
					HAL_Delay(5000);
				}
				else
				HAL_Delay(300);	
			}
			else
				break;			
		}
		HAL_Delay(300);	
	}
	
	free(ZetaSendBuf.Buf);
}


/*UserSendGps：发送GPS位置信息
*参数：			   无
*返回值：      无
*/
void UserSendGps(void)
{
	uint8_t len= 0;
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*49);
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|充电状态
	
	/********************设备ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
	
	memcpy1(&ZetaSendBuf.Buf[9], &SetGpsAck.PationBuf[0], 11); 
	
	memset(SetGpsAck.PationBuf, 0, 11);
	
//	Gps.GetPosition(&ZetaSendBuf.Buf[9]);
	len += strlen((char *)ZetaSendBuf.Buf);
	DEBUG(2,"ZetaSendBuf: ");
	for(uint8_t i = 0; i < len; i++)
	DEBUG(2,"%02X ",ZetaSendBuf.Buf[9+i]);
	DEBUG(2,"\r\n");
	
	ZetaSendBuf.Buf[9 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
	ZetaSendBuf.Buf[9 + len++] = (UpSeqCounter&0xff);
	
	ZetaSendBuf.Buf[9 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[9],len); ///CRC

  len ++;
	
	ZetaSendBuf.Buf[2] = 0x09 + len; /// +sensor_len
	ZetaSendBuf.Len = ZetaSendBuf.Buf[2];
	///UpSeqCounter ++;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		
		DEBUG(2,"start send data\r\n");
		
		for(uint8_t j = 0; j<ZetaSendBuf.Len; j++)
		DEBUG(2,"%02X ",ZetaSendBuf.Buf[j]);
		DEBUG(2,"\r\n");
		
		ZetaHandle.Send(&ZetaSendBuf);
				
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((DataAck != Status) && (HAL_GetTick(  ) - overtime < 200));
		
		if(DataAck == Status)
		{			
//			HAL_Delay(300);	
//			UserCheckCmd(&UserZetaCheck[NETIME]); ///结合外部flash使用
			UpSeqCounter ++;
			
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


/*UserCheckCmd：用户查询Zeta：服务器查询下发
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
		{			
			if(Unregistered == Status)
			{
				DEBUG(2,"---Writing registered---\r\n");
				HAL_Delay(5000);
				i = 0;
			}
			else
			{
				HAL_Delay(300);
			}
		}
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

/*
 *	UserIntoLowPower:	进入低功耗模式：停机
 *	返回值: 					无
 */
void UserIntoLowPower(void)
{	       
    BoardDeInitMcu( ); ///关闭时钟线
    
    // Disable the Power Voltage Detector
    HAL_PWR_DisablePVD( );
    
    SET_BIT( PWR->CR, PWR_CR_CWUF );
    /* Set MCU in ULP (Ultra Low Power) */
    HAL_PWREx_EnableUltraLowPower( );
    
    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp( );

    /*****************进入停机模式*********************/
    /* Enter Stop Mode */
    __HAL_PWR_CLEAR_FLAG( PWR_FLAG_WU );
    HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
}

/*!
*UserGetAddID：	获取设备ID
*返回值: 		    无
*/
void UserGetAddID(void)
{
	static char String_Buffer[33] = {0}; ///读取flash写入字符串
	static uint8_t DevTemp[8] = {0};

	STMFLASH_Read(DEV_ADDR,(uint16_t*)String_Buffer,DEV_ADDR_SIZE/2);         ////DEV
	
	String_Conversion(String_Buffer, DevTemp, DEV_ADDR_SIZE);  

	///09 07 18 30 0000 0001 ///0730 0001  30: Zeta  31:Zeta+GPS	
	DeviceInfo[0] = DevTemp[1];
	DeviceInfo[1] = DevTemp[3];
	
	DeviceInfo[2] = DevTemp[6];
	DeviceInfo[3] = DevTemp[7];
	
	DEBUG(2,"DEV: ");
	for(uint8_t i = 0; i < 4; i++)
	DEBUG(2,"%02x ",DeviceInfo[i]);
	DEBUG(2,"\r\n");
	
	memset(String_Buffer, 33, 0);	
}

/*!
*String_Conversion：字符串转换为16进制
*返回值: 		    		无
*/
void String_Conversion(char *str, uint8_t *src, uint8_t len)
{
 volatile int i,v;
			
 for(i=0; i<len/2; i++)
 {
	sscanf(str+i*2,"%2X",&v);
	src[i]=(uint8_t)v;
 }
}
