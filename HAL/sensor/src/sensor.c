/*
**************************************************************************************************************
*	@file			sensor.c
*	@author 	Jason
*	@version 	V0.1
*	@date     2018/07/13
*	@brief		�������ɼ�
***************************************************************************************************************
*/
#include "sensor.h"
#include "user-app.h"

#include "stm32l0xx_hal.h"
#include "rs485.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	


CheckRs485_t CheckRs485s[] = {
///Addr  Identifier	RegAddress  RegDatalen  SendDataLen	RevDataLen	SensorToLen  		TimeOut  SendBuff		revBuff			name
	{0x00, 0x00,				0x0000,     0x0000,         7,          9,    RS485_IDE_LEN+4,    200*1, "EXPEND",  	"",    		"EXPEND"},	///��չ��
	{0x02, 0x02,				0x0000, 		0x0002,  				6,					9, 		RS485_IDE_LEN+4,		200*1,		"" ,			"" ,		"SWR-100W"},  ///������ʪ��
	{0x05, 0x05,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*1,	  "" ,			"" ,		"ST_PH"		},  ///������ˮPH  1000*30
	{0x06, 0x06,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		500*1,		"" ,			"" ,		"ST_GH"		},	///�����Ч
	{0x07, 0x07,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*1,		"" ,			"" ,		"ST_Y/MW"	},  ///Ҷ���¶�
	{0x08, 0x08,				0x0001, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*1,		"" ,			"" ,		"ST_YMS"	},  ///Ҷ��ʪ��
	{0x0C, 0x03,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		200*1,		"" ,			"" ,		"ST-TW"		},  ///�����¶�
	{0x0D, 0x0D,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*1,		"" ,			"" ,		"ST-EC"		},  ///EC
	{0x0E, 0x0E,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*10,	"" ,			"" ,		"ST_CO2"	},	///CO2				
//	{0x0F, 0x24,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		1000*1,		"" ,			"" ,		"ST_AP"		},///����վ������ѹ				
	{0x11, 0x11,				0x0000, 		0x0001,  				6,					9, 		RS485_IDE_LEN+4,		200*1,	  "" ,			"" ,		"OXY"			},  ///ˮ���� 1000*120
	{0x12, 0x12,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		200*1,		"" ,			"" ,		"andan"		},  ///����
	{0x13, 0x13,				0x0000, 		0x0002,  				6,					9, 		RS485_IDE_LEN+4,		200*1,		"" ,			"" ,		"Water-EC"},  ///ˮEC
	{0x14, 0x14,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE_LEN+2,		200*1,		"" ,			"" ,		"Water-T" },  ///ˮ��
	{0x15, 0x15,				0x0000, 		0x0002,  				6,					7, 		RS485_IDE_LEN+2,		200*1,		"" ,			"" ,		"Water-K+"},	///ˮ��+
	{0xFD, 0x18,				0x0000, 		0x0004,  				6,				 13, 		RS485_IDE_LEN+8,		500*1,	  "" ,			"" ,		"Air-Ill" },	///������ʪ�ȡ�����
};

SaveRs485_t  SaveRs485s[3];

SendBuf_t		 SendBufs[10] = {{{0}, {0}, 0}};

Sensor_t     Sensors;
	

volatile 	uint8_t SendBufsCounter = 0; ///��¼���ݻ������������

uint8_t OpenExpendBoxbuff[10] = {0x00,0x05,0x00,0x01,0x00,0x00,0x00};
uint8_t ExpendBoxbuff[9] = {0xFE,0x03,0x04,0x00,0x00,0x00,0x00,0x00,0x00};


/*
 *	SensorsInit:		��������ʼ��
 *	������			  	��
 *	����ֵ��				��
 */
void SensorsInit(void)
{
	Sensors.Handle		 			= SensorHandle;
	Sensors.DataProces			=	SensorDataProces;
	Sensors.QueryPinStaus 	= SensorQueryPinStaus;
	Sensors.QueryType 			= SensorQueryType;
	Sensors.GetData 				= SensorGetData;
	Sensors.GetRs485Type 		= GetRs485Type;
	Sensors.ExpendBoxLive 	= SensorExpendBoxLive;
	Sensors.MaBoxData 			= SensorMaBoxData;
	Sensors.ExpBoxAddr 			= SensorExpBoxAddr;
	Sensors.ExpenData 			= SensorExpenData;

	Rs485Init(  );
	Rs485s.PowerOn(  );
}

/*
 *	GetRs485Type:		485�ӿ�����
 *	������			  	����Rs485�ӿ�ID
 *	����ֵ��				Rs485���ͣ�RS485_NONE/RS485_EXPAND_BOX/RS485_SIGNAL
 */
Rstype_t GetRs485Type(int index)
{	
	return SaveRs485s[index].Type;
}

/*
 *	SensorGetData:		��ȡRs185����������
 *	������			  		Rs485����ӿ�
 *	����ֵ��					��ȡ�ɹ�/ʧ��
 */
HAL_StatusTypeDef SensorGetData(int id)
{	
	HAL_StatusTypeDef status;
	
	if(Sensors.GetRs485Type(id) == RS485_SIGNAL)
	{
		status = Sensors.MaBoxData( id );
	}
	else if(Sensors.GetRs485Type(id) == RS485_EXPAND_BOX)
	{
		status = Sensors.ExpenData( id );
	}
	
	return status;
}

/*
 *	SensorHandle:	  ����������
 *	������			  	��
 *	����ֵ��				��
 */
void SensorHandle(void)
{			
	Rs485s.PowerOn(  );
	
	for(uint8_t id = 0; id < NBI_RS485_PIN_COUNT; id++)
	{
		if(Sensors.GetRs485Type( id ) != RS485_NONE)
		{
			id++;
			DEBUG_APP(2,"Start get sensor data main box id = %d",id);
			id--;

			Rs485s.OpenPin(id);
			HAL_Delay(1000);
			
			Sensors.GetData(id);			
		}
	}
	Rs485s.ClosePin(  );	
	
	Rs485s.PowerOff(  );
	
	SendBufsCounter = 0;
	
	Sensors.DataProces(  );
}

/*
 *	SensorDataProces:	���������ݴ���
 *	������			  		��
 *	����ֵ��					��
 */
void SensorDataProces(void) 
{
	/**********���㳤��**************/
	uint8_t Len = 0;
	
	/**********����������BUF�±�**************/
	uint8_t Index = 0;
	
	/***********��չ��ID*************/
	uint8_t ExId = 0;
	
	/***********����ID*************/
	uint8_t PortId = 0;
	
	/**********ʵ�������ܳ���**************/
	uint8_t Length = 0; 
	
	/***********������������*************/
	uint8_t Temp[50] = {0};
	
	/***********���������ݳ���*************/
	uint8_t TempIndex = 0;
	
	/***********ʣ�����ȡ����������*************/
	uint8_t GetSensorCounter = 0;
	
	WdgTime = 0;

#if 1

	DEBUG_APP(2, "-----Start get data Counter : %d----",Sensors.Counter);
	for(PortId = 0; PortId < NBI_RS485_PIN_COUNT; PortId++)
	{
		if(SaveRs485s[PortId].Type == RS485_SIGNAL && GetSensorCounter < Sensors.Counter)
		{
			Len += SaveRs485s[PortId].MainBox.SensorToLen;
			
			if(Len <= ZETAMAXLEN) ///��Ϊ��ȡ�������������쳣��־
			{
					DEBUG_APP(2, "-----SensorToLen : %d----",SaveRs485s[PortId].MainBox.SensorToLen);

				if(SaveRs485s[PortId].MainBox.SensorToLen != 0)
				{
					SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.Index; //�ӿ�ID
					SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.Identifier; //�����ʶ
					
					memcpy1(&Temp[TempIndex], &SaveRs485s[PortId].MainBox.SensorBuff[0], SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE_LEN);
					
					memset(SaveRs485s[PortId].MainBox.SensorBuff, 0, SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE_LEN);

					TempIndex += SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE_LEN ;
					Length += SaveRs485s[PortId].MainBox.SensorToLen;
									
					DEBUG_APP(3,"11SendBufsCounter = %d, Len = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",\
										SendBufsCounter,Len, PortId, GetSensorCounter, Sensors.Counter);
				}
				GetSensorCounter++;
			}
			else 
			{
				/********************�ְ��ڼ�֡*****************/
				SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
				
				/********************����*****************/
				SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
				
				/********************����������*****************/
				SendBufs[SendBufsCounter].Buf[2] = Index/2;

				uint8_t BufLen = 3;
				
				memset(&SendBufs[SendBufsCounter].Buf[BufLen], 0, 50);
				
				memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index);
				
				BufLen += Index;
				memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex);
				
				Length += 3;
				
				SendBufs[SendBufsCounter].Len = Length;
								
				DEBUG_APP(2,"22SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",\
									SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
				for(uint8_t i = 0; i< Length; i++)
				DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
				DEBUG(2,"\r\n");
	
				memset(Temp, 0, TempIndex);
				memset(SendBufs[SendBufsCounter].Port, 0, Index);
				
				PortId --; //�±����	
				SendBufsCounter ++; 
				Len = 0;
				Length = 0;
				TempIndex = 0;	
				Index = 0;
			}				
		}
		else if(SaveRs485s[PortId].Type == RS485_EXPAND_BOX && GetSensorCounter < Sensors.Counter)
		{
			for( ExId = 0 ; ExId < 5; ExId++)
			{
				if(SaveRs485s[PortId].MainBox.ExpendBox[ExId].ExpenCheck) ///�ӿڴ���
				{
					Len += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen;
			
					DEBUG_APP(3,"PortId = %d, ExId = %d Len = %d SensorToLen = %d",PortId,ExId,Len, \
					SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen);
					if(Len <= ZETAMAXLEN)
					{
						if(SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen != 0)
						{
							SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.ExpendBox[ExId].Index; //�ӿ�ID
							SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.ExpendBox[ExId].Identifier; //�����ʶ
							
							memcpy1(&Temp[TempIndex], &SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorBuff[0], \
											SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen-RS485_IDE_LEN);
							
							memset(SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorBuff, 0, \
							SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen-RS485_IDE_LEN);

							TempIndex += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen-RS485_IDE_LEN;
							Length += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen;
							
							DEBUG_APP(3,"33SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",\
												SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
						}
						GetSensorCounter++;

					}
					else
					{					
						/********************�ְ��ڼ�֡*****************/
						SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
						
						/********************����*****************/
						SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
						
						/********************����������*****************/
						SendBufs[SendBufsCounter].Buf[2] = Index/2;

						
						uint8_t BufLen = 3;
						
						memset(&SendBufs[SendBufsCounter].Buf[BufLen], 0, 50);
				
						memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index);
				
						BufLen += Index;
						memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex);
						
						Length += 3;

						SendBufs[SendBufsCounter].Len = Length;
						
						DEBUG_APP(2,"44SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",\
											SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
						for(uint8_t i = 0; i<Length; i++)
						DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
						DEBUG(2,"\r\n");
				
						memset(Temp, 0, TempIndex);
						memset(SendBufs[SendBufsCounter].Port, 0, Index);
											
						ExId --;
						SendBufsCounter ++;
						Len = 0;
						Length = 0;
						TempIndex = 0;	
						Index = 0;
					}
				}
			}
		}					
	}
	
	if(PortId == NBI_RS485_PIN_COUNT && ExId == 5)
	{
		if(1 == GetSensorCounter - Sensors.Counter)
			GetSensorCounter --;
	}
		
	if(GetSensorCounter == Sensors.Counter)  ///<38B
	{			
		/********************�ְ��ڼ�֡*****************/
		SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
		
		/********************����*****************/
		SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
		
		/********************����������*****************/
		SendBufs[SendBufsCounter].Buf[2] = Index/2;

		uint8_t BufLen = 3;
		
		memset(&SendBufs[SendBufsCounter].Buf[BufLen], 0, 50);

		memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index); ///��������ʶ
		DEBUG_APP(3, "444PortId : %d counter = %d\r\n",PortId, GetSensorCounter);
		
		BufLen += Index;
		
		memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex); ///����������
		
		Length += 3;

		SendBufs[SendBufsCounter].Len = Length;
		
		DEBUG(2,"55SendBufsCounter = %d, Length = %d,TempIndex = %d, Length = %d\r\n",SendBufsCounter,Length,TempIndex,Length);
		for(uint8_t i = 0; i< Length; i++)
		DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
		DEBUG(2,"\r\n");

		memset(Temp, 0, TempIndex);
		memset(SendBufs[SendBufsCounter].Port, 0, Index);
		
		SendBufsCounter ++;
		Len = 0;
		Length = 0;
		TempIndex = 0;	
		Index = 0;
	}
	
	Sensors.Counter = 0;
	GetSensorCounter = 0;
	
	if(WdgTime>=70) ///4S���WWDG
	WdgTime = 0;
	
#endif	
}

/*
 *	SensorQueryPinStaus:		�㲥��ѯ485����ȡ����������
 *	������			  					��
 *	����ֵ��								��ѯ״̬���ɹ�/ʧ��/��ʱ
 */
HAL_StatusTypeDef SensorQueryPinStaus(void)
{	
	HAL_StatusTypeDef Status = HAL_TIMEOUT;
	
	RS485CmdPackage(NBI_RS485_SEARCH_CODE);///��ȡԤ��485�����
	
	for(int id = 0; id < NBI_RS485_PIN_COUNT ; id++)
	{	
		WdgTime = 0;
		
		//����plus��6���ڣ��ֱ���ʲô
		//��io��	
		Rs485s.OpenPin(id);
		HAL_Delay(1000);
		//�ж���ʲô�ڣ����һ�ȡ���д������ĵ�ַ
		if(Sensors.QueryType(id) != RS485_NONE) 
		{			
			DEBUG_APP(3,"pin %02x: find device\r\n",id);
			
			if(WdgTime>=70) ///4S���WWDG
				WdgTime = 0;		
		}		
		else 
		{
			id++;
			DEBUG_WARNING(2,"pin %d:not find device\r\n",id);
			id--;
		}
	}
	Rs485s.ClosePin(  );				
	return Status;
}

/*
 *	SensorQueryType:		�㲥��ѯ����485����
 *	������			  			Rs485�ӿ�
 *	����ֵ��						Rs485���ͣ�RS485_NONE/RS485_EXPAND_BOX/RS485_SIGNAL
 */
Rstype_t SensorQueryType(int PortId)
{
	uint8_t repbuff[20] = {0};	
	uint8_t expend_sensor = 0;
	
	uint8_t id = PortId;
	id++;

	SaveRs485s[PortId].Type = RS485_NONE;
	
	int len = Rs485s.Cmd(ExpendBoxbuff,7, NODEBUG,1000);   // ��ַ�㲥��get expend return data 	
	
	memcpy1(repbuff,Rs485s.Revbuff,len);
		
	Rs485s.Print(repbuff, len, 3);
  ///�жϹ㲥�ظ���ַ����չ�е�ַӦ��
	
	if(len == CheckRs485s[EXPENDBOX].RevDataLen && repbuff[3] == 0) //��չ�еĵ�ַ Ϊ0
	{
		//���ص����ݳ��ȴ���0����Ϊ���ⲿ����
		DEBUG_APP(3,"expend box is ok");
		expend_sensor = repbuff[4];  	//��չ����Щ�ڽ��봫����
		
		Sensors.ExpendBoxLive(expend_sensor,PortId); ///��չ����Ӧ�ӿ�״̬��־
        
		SaveRs485s[PortId].Type = RS485_EXPAND_BOX;
		
		Sensors.ExpBoxAddr( PortId );
          
		DEBUG_APP(2,"sensor get expen box address success PortId : %d",id);	
	}
	else if(len >5) ///����չ�е�ַӦ��
	{				
		//û���ҵ���չ�е��ǵ��������˻ظ�
		DEBUG_APP(3,"siganal ok and has rpy addr = %x",repbuff[3]);		
		int i = 0;

		////�жϽӿڶ�Ӧ����485��������ַ����¼
		for(i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
		{
			if(CheckRs485s[i].Addr == repbuff[3]) 
			{					
				SaveRs485s[PortId].Type = RS485_SIGNAL;
				
				SaveRs485s[PortId].MainBox.Index = id;  
				SaveRs485s[PortId].MainBox.CheckIndex = i; ///��¼��ѯ�±�
				SaveRs485s[PortId].MainBox.Identifier = CheckRs485s[i].Identifier; ///�����ʶ
//				SaveRs485s[PortId].MainBox.SensorToLen = CheckRs485s[i].SensorToLen; ///�������ܳ���		
						
				DEBUG_APP(2,"CheckRs485s is ok index = %d",i);		
				break;
			}
		}
		if(i == ARRAY(CheckRs485s))
		{
				//û���ҵ�
				SaveRs485s[PortId].Type = RS485_NONE;				
				DEBUG_ERROR(2,"address = %d not in array",repbuff[3]);		
		}
		else
		{
				SaveRs485s[PortId].Type = RS485_SIGNAL;
		}
	}
	else
	{
		//û���ҵ���չ�У�������Ҳû�лظ�����ʼ�������е��ⲿ�豸������
		DEBUG_WARNING(3,"siganal ok and foreach");		
		uint32_t startTime = HAL_GetTick();

//		for(int i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
		for(int i = 4 ; i < 6 ; i ++) ///ֻ��ѯҶ�洫����
		{			
			len = Rs485s.Cmd(CheckRs485s[i].SendBuff, CheckRs485s[i].SendDataLen, NODEBUG, CheckRs485s[i].TimeOut);
			while(HAL_GetTick() - startTime < CheckRs485s[i].TimeOut && len != CheckRs485s[i].RevDataLen)
			{
				len = Rs485s.Cmd(CheckRs485s[i].SendBuff,CheckRs485s[i].SendDataLen, NODEBUG, CheckRs485s[i].TimeOut);				
				HAL_Delay(500);
			}
			if(len == CheckRs485s[i].RevDataLen)
			{
				//�ҵ���
				DEBUG_APP(2,"device had find:%02x",CheckRs485s[i].Addr);
												
				SaveRs485s[PortId].MainBox.Index = id;
				SaveRs485s[PortId].MainBox.CheckIndex = i; ///��¼��ѯ�±�
				SaveRs485s[PortId].MainBox.Identifier = CheckRs485s[i].Identifier; ///�����ʶ
//				SaveRs485s[PortId].MainBox.SensorToLen = CheckRs485s[i].SensorToLen; ///�������ܳ���		

				SaveRs485s[PortId].Type = RS485_SIGNAL;
				break;
			}
			else
			{
        DEBUG_WARNING(2,"device had not find g_rs485index:%02x",id);
				SaveRs485s[PortId].Type = RS485_NONE;
			}
		}		
	}
	return SaveRs485s[PortId].Type;
}

/*
 *	SensorExpendBoxLive:		�ж���չ����Щ�ڽ��봫����
 *	������			  					��չ�н����ʶ������485ID
 *	����ֵ��								��
 */
void SensorExpendBoxLive(int expend_sensor,int index)
{
	//0x4f
	for(int i = 0 ; i < 5 ; i ++)
	{
		if(i == 4)
		{
			if(expend_sensor & (0x01 << 6))
			{
				SaveRs485s[index].MainBox.ExpendBox[i].ExpendBoxLive = true;
			}
			break;
		}
		if(expend_sensor & (0x01 << i))
		{
			SaveRs485s[index].MainBox.ExpendBox[i].ExpendBoxLive = true;
		}       
	}	
}

/*
 *	SensorExpBoxAddr:		��ѯ��չ���봫������ַ
 *	������			  			����485ID
 *	����ֵ��						��ѯ״̬���ɹ�/ʧ��
 */
HAL_StatusTypeDef SensorExpBoxAddr(int index)
{
	uint8_t len = 0;
	uint8_t repbuff[20] = {0};	
	
	uint8_t ExpId = 0;
	
	uint8_t MainIndex = index;
	
	uint8_t ExpIndex = ExpId;
	
	MainIndex++;
	
	for(ExpId = 0 ; ExpId < 5 ; ExpId ++)
	{
		ExpIndex++;

		SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = false;
		
		if( SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive )
		{			
			DEBUG_APP(2,"main port=%02x ExpId = %d",MainIndex, ExpIndex);

			if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(ExpId))
			{
				HAL_Delay(100);
				DEBUG_ERROR(2,"main00 port = %02x, exbox port=%d sensor open faile",MainIndex,ExpIndex);				
				
				if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(ExpId))
				{
					DEBUG_ERROR(2,"main port = %02x, exbox port=%d sensor open faile",MainIndex,ExpIndex);				
					continue;
				}
			}
			else
			{
				DEBUG_APP(3,"exbox port=%d sensor open success",ExpIndex);
			}
			
		  len = Rs485s.Cmd(ExpendBoxbuff,7, NODEBUG,1000);   // ��ַ�㲥��get expend return data 	
	
			memcpy1(repbuff,Rs485s.Revbuff,len);
	
			DEBUG_APP(3,"rs485 ExpendBox get data = ");			
			Rs485s.Print(repbuff,len, 3);
			
			if(len >5) ///�㲥Ӧ��
			{				
				DEBUG_APP(2,"siganal ok and has rpy addr = %x",repbuff[3]);		
				int i = 0;

				////�жϽӿڶ�Ӧ����485��������ַ����¼
				for(i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
				{
					if(CheckRs485s[i].Addr == repbuff[3]) 
					{											
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = true;
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Index = (MainIndex<<4)|ExpId;   
						SaveRs485s[index].MainBox.ExpendBox[ExpId].CheckIndex = i; ///��¼��ѯ�±�
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Identifier = CheckRs485s[i].Identifier; ///�����ʶ
//						SaveRs485s[index].MainBox.ExpendBox[ExpId].SensorToLen = CheckRs485s[i].SensorToLen; ///�������ܳ���		
									
						DEBUG_APP(3,"add device ok id = %d",i);		
						break;
					}
				}
				if(i == ARRAY(CheckRs485s))
				{
						//û���ҵ�
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive = false;				
						DEBUG_WARNING(2,"ExpendBox address = %d not in array",repbuff[3]);		
				}
			}
			else
			{
				//�㲥��Ϣû�з��أ���Ϊ���������������FE��ѯ
        //����
        DEBUG_ERROR(2,"port=%d sensor don`t return FE statr foreach",ExpIndex);			
				uint32_t startTime = HAL_GetTick();
				int i;
				
//				for( i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
				for(int i = 4 ; i < 6 ; i ++) ///ֻ��ѯҶ�洫����
				{
					len = Rs485s.Cmd(CheckRs485s[i].SendBuff,CheckRs485s[i].SendDataLen, NODEBUG, CheckRs485s[i].TimeOut);
				
					if(len == CheckRs485s[i].RevDataLen)
					{
						//�ҵ���
						DEBUG_APP(2,"device had find:%02x",CheckRs485s[i].Addr);
						
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = true;
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Index = (MainIndex<<4)|ExpId;   
						SaveRs485s[index].MainBox.ExpendBox[ExpId].CheckIndex = i; ///��¼��ѯ�±�
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Identifier = CheckRs485s[i].Identifier; ///�����ʶ
//						SaveRs485s[index].MainBox.ExpendBox[ExpId].SensorToLen = CheckRs485s[i].SensorToLen; ///�������ܳ���
						break;
					}
				}
				if(i == ARRAY(CheckRs485s))
				{
					//û���ҵ�
					SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive = false;				
					DEBUG_ERROR(2,"ExpendBox address = %d not in array",repbuff[3]);	
				}
			}			
		}
	}	
		
	//�ر� ��չ��
	OpenExpendBoxbuff[5] = 0x00;
	len = Rs485s.Cmd(OpenExpendBoxbuff,7, NODEBUG, 200);		
	if(len == 0)
	{
		DEBUG_ERROR(2,"close expend box falie");		
		return HAL_ERROR;		
	}
	else
	{
		DEBUG_APP(3,"close expend box");
		return HAL_OK;
	}
}

/*
 *	SensorMaBoxData:		��ȡ����Rs485����������
 *	������			  			����485ID
 *	����ֵ��						��ѯ״̬���ɹ�/ʧ��
 */
HAL_StatusTypeDef SensorMaBoxData(uint8_t id)
{
	uint8_t len = 0;	
	uint32_t startTime = 0;
	uint32_t getDataTimeOut = 4000;
	
	len = Rs485s.Cmd(CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendDataLen,\
								   NODEBUG, CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].TimeOut);
	//�ȴ��������,				
	startTime = HAL_GetTick();
	while(HAL_GetTick() - startTime < getDataTimeOut && len != CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].RevDataLen)
	{
		len = Rs485s.Cmd(CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendDataLen,\
								     NODEBUG, CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].TimeOut);
		HAL_Delay(500);
	}
	if(len != CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].RevDataLen)
	{
		DEBUG_ERROR(2,"Get MainBox : %02x data error",SaveRs485s[id].MainBox.Index);
		
		///��ȡ����ʱĳ���ӿڴ������쳣���������ܳ������㣬��Ϊʶ���ʶ
		SaveRs485s[id].MainBox.SensorToLen = 0; ///�������ܳ���		
		Sensors.Counter ++;
	
//		SaveRs485s[index].MainBox.ExpendBox[Exid].Index += 1;   ///��ȡ����ʱĳ���ӿڴ������쳣
		return HAL_ERROR;
	}
	
	SaveRs485s[id].MainBox.SensorToLen = CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SensorToLen; ///�������ܳ���		
	Sensors.Counter ++;
	memset(SaveRs485s[id].MainBox.DataBuff,0,len);
	memcpy1(SaveRs485s[id].MainBox.DataBuff,Rs485s.Revbuff,len);
	id ++;
	DEBUG_APP(2,"main_box = %d, sensor Rs485 revData : ",id);
	id--;
	Rs485s.Print(SaveRs485s[id].MainBox.DataBuff,len, 2);				

	//get data
	for(int i = 0,j = 3 ; j < len-2 ; j ++)
	{
		SaveRs485s[id].MainBox.SensorBuff[i] = SaveRs485s[id].MainBox.DataBuff[j];
		DEBUG_APP(3,"get data[%d] = 0x%02x\r\n",j,SaveRs485s[id].MainBox.SensorBuff[i]);	
		i++;
	}
	return HAL_OK;
}

/*
 *	SensorExpenData:		��ȡ��չ��Rs485����������
 *	������			  			����485ID
 *	����ֵ��						��ѯ״̬���ɹ�/ʧ��
 */
HAL_StatusTypeDef SensorExpenData(uint8_t index)
{
	uint8_t len = 0;
	
	uint32_t startTime = 0;
	uint32_t getDataTimeOut = 4000;
	
	int Exid = 0;
	
	uint8_t main_box = index;
	uint8_t ex_box   = Exid;
	
	main_box ++;

	for(Exid = 0; Exid < 5; Exid++)
	{
		ex_box ++;
		/***********************�ж϶�Ӧ485�ӿ��Ƿ���봫����***********************************/
		if( SaveRs485s[index].MainBox.ExpendBox[Exid].ExpenCheck ) 
		{			
			/***********************����չ�нӿ�״̬***********************************/
			if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(Exid))
			{
				HAL_Delay(100);		

				if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(Exid))
				{
					DEBUG_ERROR(2,"main port = %d, exbox port=%d sensor open faile",main_box,ex_box);			
					continue;
				}
			}

			len = Rs485s.Cmd(CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].SendBuff,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].SendDataLen,\
											 NODEBUG,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].TimeOut);      
			//�ȴ��������,				
			startTime = HAL_GetTick();
			while(HAL_GetTick() - startTime < getDataTimeOut && len != CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].RevDataLen)
			{
				len = Rs485s.Cmd(CheckRs485s[SaveRs485s[index].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[index].MainBox.CheckIndex].SendDataLen,\
												 NODEBUG,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].TimeOut);                                       
				HAL_Delay(500);
			}
			if(len != CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].RevDataLen)
			{
				DEBUG_ERROR(2,"Get Exid : %02x data error",SaveRs485s[index].MainBox.ExpendBox[Exid].Index);	
				
				Sensors.Counter ++;
				
				SaveRs485s[index].MainBox.ExpendBox[Exid].SensorToLen = 0;
				
				continue;
			}
			else
			{
				Sensors.Counter ++;
				
				SaveRs485s[index].MainBox.ExpendBox[Exid].SensorToLen = CheckRs485s[SaveRs485s[index].MainBox.CheckIndex].SensorToLen;
			
				memset(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,0,len);
				memcpy1(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,Rs485s.Revbuff,len);
				
				DEBUG_APP(2,"main_box = %d, exbox port=%d Index = 0x%02x sensor Rs485 revData : ",main_box,ex_box,SaveRs485s[index].MainBox.ExpendBox[Exid].Index);
				Rs485s.Print(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,len, 2);				

				//get data
				for(int i = 0,j = 3 ; j < len - 2; j ++)
				{
					SaveRs485s[index].MainBox.ExpendBox[Exid].SensorBuff[i] = SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff[j];
					DEBUG_APP(3,"get data[%d] = 0x%02x\r\n",j,SaveRs485s[index].MainBox.ExpendBox[Exid].SensorBuff[i]);	
					i++;
				}	
			}			
		}
	}
	
	//�ر� ��չ��
	OpenExpendBoxbuff[5] = 0x00;
	len = Rs485s.Cmd(OpenExpendBoxbuff,7, NODEBUG, 100);		
	if(len == 0)
	{
		DEBUG_ERROR(2,"close expend box falie");		
		return HAL_ERROR;		
	}
	else
	{
		DEBUG_APP(3,"close expend box ok");
		return HAL_OK;
	}
}

/*
*OpenExpenBox������չ�нӿ�
*ExpId��			 ��չ��ID
*���أ�				 ��չ�з������ݳ���
*/
uint8_t OpenExpenBox(uint8_t ExpId)
{
	int temp = 0;
	uint8_t len = 0;
	
	if(ExpId == 4)
	{
		temp = 6;
	}
	else
	{
		temp = ExpId;				
	}
	OpenExpendBoxbuff[5] = 0x01 << temp;
	len = Rs485s.Cmd(OpenExpendBoxbuff,7, NODEBUG, 100);

	return len;
}

/*
*RS485CmdPackage��Rs485��ѯ���������
*mode��			 			Rs485������
*���أ�				    ��
*/
void RS485CmdPackage(char mode)
{
	for(int index = 0 ; index < ARRAY(CheckRs485s); index ++)
	{
		DEBUG_APP(3,"index = %d,addr = %02X\r\n",index,CheckRs485s[index].Addr);
		memset(CheckRs485s[index].SendBuff,0,sizeof(CheckRs485s[index].SendBuff));
		CheckRs485s[index].SendBuff[0] = CheckRs485s[index].Addr;
		CheckRs485s[index].SendBuff[1] = mode;
		CheckRs485s[index].SendBuff[2] = (CheckRs485s[index].RegAddress & 0xFF00)  >> 8;		
		CheckRs485s[index].SendBuff[3] = (CheckRs485s[index].RegAddress & 0x00FF);
		CheckRs485s[index].SendBuff[4] = (CheckRs485s[index].RegDatalen & 0xFF00)  >> 8;				
		CheckRs485s[index].SendBuff[5] = (CheckRs485s[index].RegDatalen & 0x00FF);
		
//		for(uint8_t i = 0; i < 6; i ++)
//	  DEBUG(3,"%02X ",CheckRs485s[index].SendBuff[i]);
//		DEBUG(3,"\r\n");
	}
}

/*
*memcpy1��	���ݿ���
*dst��			����Ŀ��
*src:				����Դ��ַ
*size:			���ݴ�С
*���أ�			��
*/
void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
	while( size-- )
	{
		*dst++ = *src++;
	}
}
