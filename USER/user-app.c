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
#include "debug.h"
#include "Zeta.h"
#include "sensor.h"
#include "gps.h"
#include "led.h"

#define VERSIOS					0x01

volatile uint16_t	UpSeqCounter = 1; 


UserZeta_t UserZetaCheck[] = {
	{0x10, 1500, Payload}, ///��ѯmac
	{0x11, 1000, Payload}, ///��ѯ����ʱ��
	{0x12, 500,  Payload}, ///��ѯʣ����
	{0x13, 1000, Payload}, ///��ѯ��������
};

User_t User = {0, 0, false, false, false};

static uint8_t DeviceInfo[4] = {0};

/*UserCheckSensors��	�û���ѯ��������Ϣ
*������								��
*����ֵ��   					��
*/
void UserCheckSensors(void)
{		
	UserGetAddID(  );
	
	LedOn(  );
			
	if(GPSEXIST == DeviceInfo[1])
	{
		Gps.Init(  );
		
		DEBUG_APP(2,"*** Now Start positioning ***"); 
		Gps.Set(  );
		
		SetLedStates(GpsLocation);
		
		SetGpsAck.GetPation = PATIONNULL;
	}
	
	Sensors.WaterSensor = false;
	
	/****************��ȡ����������ʱ����ܽϳ����ȿ���GPS��λ��ʡʱ��*****************/
	Sensors.QueryPinStaus(  );
	
	if(Sensors.WaterSensor)
	{
		DEBUG_APP(2, "Sensors.WaterSensor Sleep time: 30min");
		User.SleepTime = 1;
	}
		
  LedOff(  );
}


/*UserSend���û�����Zeta���ͺ�����ע�⣺��������ǰ����ȴ�ģ��ע����ɣ�������ʧ�ܣ�����ģʽĬ�Ͽ�ֱ��ִ��,
*						���������MAX = 49
*������			��
*����ֵ��   ��
*/
void UserSend(Zeta_t *SendBuf)
{
	uint8_t ApplyCounter = 0;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		
		DEBUG(2,"start send data\r\n");
		
		for(uint8_t j = 0; j<SendBuf->Len; j++)
		DEBUG(2,"%02X ",SendBuf->Buf[j]);
		DEBUG(2,"\r\n");
		
		ZetaHandle.Send(SendBuf);
		
		HAL_Delay(100);
		ZetaState_t  Status = ZetaHandle.Recv(  );
		
		uint32_t overtime = HAL_GetTick(  );
		while((DataAck != Status) && (HAL_GetTick(  ) - overtime < 200));
					
		if(DataAck == Status)
		{			
//			HAL_Delay(300);	
//			UserCheckCmd(&UserZetaCheck[NETIME]); ///����ⲿflashʹ��
			break;
		}
		else if(LenError != Status)
		{			
			if(Unregistered == Status)
			{
				ApplyCounter ++;
				DEBUG(2,"---Writing registered---\r\n");
				i = 1;
				LedSendFail(5);    ///����ʧ����˸6S;
				
				if(ApplyCounter == 10)  ///1min��ʱ������������
				{
					ApplyCounter = 0;
					
					ZetaHandle.PowerOff(  );
					SetRtcAlarm(60); 
					UserIntoLowPower(  );
				}
			}
			else
			HAL_Delay(300);	
		}
		else
		{		
			break;		
		}			
	}
}


/*UserSendSensor���û�����Zeta���ʹ���������
*						
*������						��
*����ֵ��   			��
*/
void UserSendSensor(void)
{	
	if(!User.Sleep) ///�ϵ��һ�β�ѯ����
	Sensors.CheckHandle(  );
	else   ///���ߺ����
	Sensors.Handle(  );
	
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬
	
	/********************�豸ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
				
	for(uint8_t SedId = 0; SedId <= SendBufsCounter - 1; SedId++)
	{
		memcpy1(&ZetaSendBuf.Buf[9], SendBufs[SedId].Buf, SendBufs[SedId].Len); ///payload
		
		ZetaSendBuf.Buf[4] |= User.BatState;
				
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
		
		UserSend(&ZetaSendBuf);
		
		HAL_Delay(1000);
						
		/********************�������*******************/
		memset(&ZetaSendBuf.Buf[9], 0, ZetaSendBuf.Len);
		SendBufs[SedId].Len = 0; ///payload����						
	}
	UpSeqCounter ++;
	
	LedSendSucess(8);   ///ÿ�����ݼ��4S
}

/*UserSendGps������GPSλ����Ϣ
*������			   ��
*����ֵ��      ��
*/
void UserSendGps(void)
{
	uint8_t len= 0;
	uint32_t overtime  = 0;
		
	if(GPSEXIST == DeviceInfo[1])
	{			
		DEBUG_APP(2,"11SetGpsAck.GetPation = %d",SetGpsAck.GetPation);

		while( SetGpsAck.GetPation == PATIONNULL && SetGpsAck.Posfix);
		
		DEBUG_APP(2,"SetGpsAck.GetPation = %d",SetGpsAck.GetPation);
		
		overtime = HAL_GetTick();
		while(!SetGpsAck.GpsDone && (HAL_GetTick( ) - overtime <3000));
		
		if(SetGpsAck.GpsDone)
		{
			SetGpsAck.GpsDone = false;
			SetGpsAck.Posfix = false;
			
			ZetaSendBuf.Buf[0] = 0xff;
			ZetaSendBuf.Buf[1] = 0x00;
			
			ZetaSendBuf.Buf[3] = 0x02;
			
			ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬
			
			/********************�豸ID*****************/
			memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
			
			ZetaSendBuf.Buf[9] = 0x11; //֡��
					
			len += SetGpsAck.Len;
			
			DEBUG_APP(2,"len = %02X ",len);
			
			memcpy1(&ZetaSendBuf.Buf[10], &SetGpsAck.PationBuf[0], len); 
			
			ZetaSendBuf.Buf[4] |= User.BatState;

			memset(SetGpsAck.PationBuf, 0, 11);
			
			DEBUG(2,"ZetaSendBuf: ");
			for(uint8_t i = 0; i < len; i++)
			DEBUG(2,"%02X ",ZetaSendBuf.Buf[10+i]);
			DEBUG(2,"\r\n");
			
			ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
			ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);
			
			ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC

			len ++;
				
			ZetaSendBuf.Buf[2] = 0x0A + len; /// +sensor_len
			ZetaSendBuf.Len = ZetaSendBuf.Buf[2];
			
			UserSend(&ZetaSendBuf);
			
			UpSeqCounter ++;
			
			LedSendSucess(8);   ///ÿ�����ݼ��4S
			
			/********************�������*******************/
			memset(&ZetaSendBuf.Buf[10], 0, ZetaSendBuf.Len);
		}
	}
}

/*UserSendTest������Zeta��������
*������			    ��
*����ֵ��       ��
*/
void UserSendTest(void)
{
	uint8_t len= 3;
		
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬
	
	/********************�豸ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
	
	ZetaSendBuf.Buf[9] = 0x11; //֡��
			
	uint8_t data[3] = {1,2,3};			
	memcpy1(&ZetaSendBuf.Buf[10], &data[0], len); 
	
	ZetaSendBuf.Buf[4] |= User.BatState;

	memset(SetGpsAck.PationBuf, 0, 11);
	
	DEBUG(2,"ZetaSendBuf: ");
	for(uint8_t i = 0; i < len; i++)
	DEBUG(2,"%02X ",ZetaSendBuf.Buf[10+i]);
	DEBUG(2,"\r\n");
	
	ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
	ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);
	
	ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC

	len ++;
		
	ZetaSendBuf.Buf[2] = 0x0A + len; /// +sensor_len
	ZetaSendBuf.Len = ZetaSendBuf.Buf[2];
	
	UserSend(&ZetaSendBuf);
	
	UpSeqCounter ++;
	
	LedSendSucess(8);   ///ÿ�����ݼ��4S
	
	/********************�������*******************/
	memset(&ZetaSendBuf.Buf[10], 0, ZetaSendBuf.Len);
}

/*UserDownCommand������Ӧ��
*������			   		 ��
*����ֵ��      		 ��
*/
void UserDownCommand(void)
{
	uint8_t DoneState = 0;
	uint8_t len= 0;
	
  DoneState = ZetaHandle.DownCommand(ZetaRecviceBuf.RevBuf);
		
	DEBUG_APP(2,"DoneState = %d",DoneState);
		
	ZetaSendBuf.Buf[0] = 0xff;
	ZetaSendBuf.Buf[1] = 0x00;
	
	ZetaSendBuf.Buf[3] = 0x02;
	
	ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬

	/********************�豸ID*****************/
	memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
	
	ZetaSendBuf.Buf[9] = 0x11;
			
	ZetaSendBuf.Buf[4] |= User.BatState;
	
	switch(DoneState)
	{
		case 0x01:
		ZetaSendBuf.Buf[10 + len++] = ACKCOM;
		ZetaSendBuf.Buf[10 + len++] = 0x01;
		
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);

		ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC
			
		len++;
		
		ZetaSendBuf.Len = 0x0A+len;
		
		ZetaSendBuf.Buf[2] = 0x0A+len;
		UserSend(&ZetaSendBuf);
				
		LedSendSucess(8);   ///ÿ�����ݼ��4S
		
		UpSeqCounter ++;
			
		break;
		
		case 0x03:   ///�ϱ�MAC��ַ
			
		UserCheckCmd(&UserZetaCheck[MAC]);

		ZetaSendBuf.Buf[0] = 0xff;
		ZetaSendBuf.Buf[1] = 0x00;
		
		ZetaSendBuf.Buf[3] = 0x02;
		
		ZetaSendBuf.Buf[4] = (VERSIOS << 4); ///|���״̬

		/********************�豸ID*****************/
		memcpy1(&ZetaSendBuf.Buf[5], &DeviceInfo[0], 4); 
		
		ZetaSendBuf.Buf[9] = 0x11;
				
		ZetaSendBuf.Buf[4] |= User.BatState;
		ZetaSendBuf.Buf[10 + len++] = ACKMAC;
					
		memcpy1(&ZetaSendBuf.Buf[10 + len], ZetaRecviceBuf.RevBuf, 4);
				
		len += 4;
		
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);

		ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC
		
		len++;	
		
		ZetaSendBuf.Len = 0x0A+len;
		ZetaSendBuf.Buf[2] = 0x0A+len;
				
		UserSend(&ZetaSendBuf);
		
		LedSendSucess(8);   ///ÿ�����ݼ��4S
				
		UpSeqCounter ++;
			
		break;
		
		case 0xFC:
			
		ZetaSendBuf.Buf[10 + len++] = ACKMAC;
		ZetaSendBuf.Buf[10 + len++] = 0x02;
		
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);

		ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC
		
		len++;	
		ZetaSendBuf.Len = 0x0A+len;
		ZetaSendBuf.Buf[2] = 0x0A+len;
		UserSend(&ZetaSendBuf);
		
		LedSendSucess(8);   ///ÿ�����ݼ��4S
				
		UpSeqCounter ++;
			
		break;
		
		default:
			
		ZetaSendBuf.Buf[10 + len++] = ACKCOM;
		ZetaSendBuf.Buf[10 + len++] = 0x02;
		
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff00)<<8; ///Seq
		ZetaSendBuf.Buf[10 + len++] = (UpSeqCounter&0xff);

		ZetaSendBuf.Buf[10 + len] = ZetaHandle.CRC8(&ZetaSendBuf.Buf[10],len); ///CRC
		
		len++;	
		ZetaSendBuf.Len = 0x0A+len;
		ZetaSendBuf.Buf[2] = 0x0A+len;
		UserSend(&ZetaSendBuf);
		
		LedSendSucess(8);   ///ÿ�����ݼ��4S
				
		UpSeqCounter ++;
			
		break;
		
	}
	
	memset(&ZetaSendBuf.Buf[10], 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.RevBuf, 0, sizeof(ZetaRecviceBuf.RevBuf)/sizeof(ZetaRecviceBuf.RevBuf[0])); 	
}

/*UserCheckCmd���û���ѯZeta����������ѯ�·�
*������					UserZetaCheckCmd����ѯ����
*����ֵ��   		��
*/
void UserCheckCmd(UserZeta_t *UserZetaCheckCmd)
{	
	uint8_t temp[3] = {0xff, 0x00, 0x04};
	uint8_t ApplyCounter = 0;
		
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
				ApplyCounter ++;
				DEBUG(2,"---Writing registered---\r\n");
				LedSendFail(5);    ///����ʧ����˸6S
				i = 0;
				
				if(ApplyCounter == 10)  ///1min��ʱ����
				{
					ApplyCounter = 0;
					
					ZetaHandle.PowerOff(  );
					SetRtcAlarm(60); 
					UserIntoLowPower(  );
				}
			}
			else
			{
				HAL_Delay(300);
			}
		}
	}
	
	memset(ZetaSendBuf.Buf, 0, ZetaSendBuf.Len);
	memset(ZetaRecviceBuf.Buf, 0, ZetaRecviceBuf.Len);
}

/*UserSetHeart���û�����Zeta����
*������					mode: 0--����ģʽ6Hһ��������1--����ģʽ10Sһ������
*����ֵ��   		��
*/
void UserSetHeart(uint8_t mode)
{	
	uint8_t temp[4] = {0xff, 0x00, 0x05, 0x22};
		
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
}

/*UserSetTimer������Zeta��ʱ��ʱ��
*������					Timer�������жϿ�ʼʱ�䡢���ʱ�䡢�жϺ�
*����ֵ��   		��
*/
void UserSetTimer(ZetaTimer_t Timer)
{
	uint8_t temp[4] = {0xff, 0x00, 0x0f, 0x20};
	
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
}

/*UserCloseTimer���ر�Zeta��ʱ��
*������						Timer���жϺ�
*����ֵ��   			��
*/
void UserCloseTimer(ZetaTimer_t Timer)
{
	uint8_t temp[4] = {0xff, 0x00, 0x05, 0x21};
	
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
}

/*
 *	UserIntoLowPower:	����͹���ģʽ��ͣ��
 *	����ֵ: 					��
 */
void UserIntoLowPower(void)
{	  
		DEBUG_APP(2,"Goto Sleep Mode");
	
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

	FlashRead16More(DEV_ADDR,(uint16_t*)String_Buffer,DEV_ADDR_SIZE/2);         ////DEV
	
	String_Conversion(String_Buffer, DevTemp, DEV_ADDR_SIZE);  

	///09 07 18 30 0000 0001 ///0730 0001  30: Zeta  31:Zeta+GPS	
	DeviceInfo[0] = DevTemp[1];
	DeviceInfo[1] = DevTemp[3];
	
	DeviceInfo[2] = DevTemp[6];
	DeviceInfo[3] = DevTemp[7];
	
	for(uint8_t i = 0; i < 8; i++)
	DEBUG(2,"%02x ",DevTemp[i]);
	DEBUG(2,"\r\n");

	
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

void UserReadFlash(void)
{	
	 if(FlashRead32(SLEEP_ADDR)==0||FlashRead32(SLEEP_ADDR)==0xffffffff)
	{
			uint32_t time = 5;//Ĭ��5min
			FlashWrite32(SLEEP_ADDR,&time,1);			
	 }
	
	 if(FlashRead32(MAXLEN_ADDR)==0||FlashRead32(MAXLEN_ADDR)==0xffffffff)
	 {
			ZetaSendBuf.MaxLen = 38;
	 }
	 else
	 {
			ZetaSendBuf.MaxLen = FlashRead16(MAXLEN_ADDR);
		 	DEBUG_APP(2,"ZetaSendBuf.MaxLen = %d",ZetaSendBuf.MaxLen);
		 
			char String_Buffer[2] = {0};
			
			FlashRead16More(MAXLEN_ADDR, (uint16_t *)String_Buffer,	MAXLEN_ADDR_SIZE/2);           
			String_Conversion(String_Buffer, &ZetaSendBuf.MaxLen, MAXLEN_ADDR_SIZE);

		  ZetaSendBuf.MaxLen -= FIXLEN;
		 
			DEBUG_APP(2,"ZetaSendBuf.MaxLen = %d",ZetaSendBuf.MaxLen);
	 }
	 
	User.SleepTime =	FlashRead32(SLEEP_ADDR);
		 
	DEBUG_APP(2,"User.SleepTime = %d \r\n",User.SleepTime);

}
