/*
**************************************************************************************************************
*	@file	user-app.c
*	@author Jason_531@163.com
*	@version 
*	@date    
*	@brief	Ӧ�ò�
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
	{0x10, 1500, Payload}, ///��ѯmac
	{0x11, 1000, Payload}, ///��ѯ����ʱ��
	{0x12, 500,  Payload}, ///��ѯʣ����
	{0x13, 1000, Payload}, ///��ѯ��������
};

uint8_t DeviceInfo[4] = {0};

/*UserInit��	�û����ó�ʼ��
*������				��
*����ֵ��   	��
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


/*UserSend���û�����Zeta���ͺ�����ע�⣺��������ǰ����ȴ�ģ��ע����ɣ�������ʧ�ܣ�����ģʽĬ�Ͽ�ֱ��ִ��,
*						���������MAX = 49
*������			��
*����ֵ��   ��
*/
void UserSend(void)
{
	//��ȡ����������
	
	//��������
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*49);
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬
	
	/********************�豸ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
	
	SendBufsCounter = 1;
			
	for(uint8_t SedId = 0; SedId <= SendBufsCounter - 1; SedId++)
	{
		memcpy1(&ZetaSendBuf.Buf[9], SendBufs[SedId].Buf, SendBufs[SedId].Len); ///payload
				
		ZetaSendBuf.Buf[9] += 1; 
		
		ZetaSendBuf.Buf[9] |= (SendBufsCounter<<4);  ///�ܰ���|��ǰ�ڼ���
		
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
			
			/********************�������*******************/
			memset(&ZetaSendBuf.Buf[9], 0, ZetaSendBuf.Len);
			SendBufs[SedId].Len = 0;
			
			HAL_Delay(100);
			ZetaState_t  Status = ZetaHandle.Recv(  );
			
			uint32_t overtime = HAL_GetTick(  );
			while((DataAck != Status) && (HAL_GetTick(  ) - overtime < 200));
						
			if(DataAck == Status)
			{			
	//			HAL_Delay(300);	
	//			UserCheckCmd(&UserZetaCheck[NETIME]); ///����ⲿflashʹ��
				
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


/*UserSendGps������GPSλ����Ϣ
*������			   ��
*����ֵ��      ��
*/
void UserSendGps(void)
{
	uint8_t len= 0;
	
	ZetaSendBuf.Buf = (uint8_t*)malloc(sizeof(uint8_t)*49);
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬
	
	/********************�豸ID*****************/
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
//			UserCheckCmd(&UserZetaCheck[NETIME]); ///����ⲿflashʹ��
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


/*UserCheckCmd���û���ѯZeta����������ѯ�·�
*������					UserZetaCheckCmd����ѯ����
*����ֵ��   		��
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

/*UserSetHeart���û�����Zeta����
*������					mode: 0--����ģʽ6Hһ��������1--����ģʽ10Sһ������
*����ֵ��   		��
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

/*UserSetTimer������Zeta��ʱ��ʱ��
*������					Timer�������жϿ�ʼʱ�䡢���ʱ�䡢�жϺ�
*����ֵ��   		��
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

/*UserCloseTimer���ر�Zeta��ʱ��
*������						Timer���жϺ�
*����ֵ��   			��
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
 *	UserIntoLowPower:	����͹���ģʽ��ͣ��
 *	����ֵ: 					��
 */
void UserIntoLowPower(void)
{	       
    BoardDeInitMcu( ); ///�ر�ʱ����
    
    // Disable the Power Voltage Detector
    HAL_PWR_DisablePVD( );
    
    SET_BIT( PWR->CR, PWR_CR_CWUF );
    /* Set MCU in ULP (Ultra Low Power) */
    HAL_PWREx_EnableUltraLowPower( );
    
    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp( );

    /*****************����ͣ��ģʽ*********************/
    /* Enter Stop Mode */
    __HAL_PWR_CLEAR_FLAG( PWR_FLAG_WU );
    HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
}

/*!
*UserGetAddID��	��ȡ�豸ID
*����ֵ: 		    ��
*/
void UserGetAddID(void)
{
	static char String_Buffer[33] = {0}; ///��ȡflashд���ַ���
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
*String_Conversion���ַ���ת��Ϊ16����
*����ֵ: 		    		��
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
