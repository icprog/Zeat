/*
**************************************************************************************************************
*	@file			sensor.c
*	@author 	Jason
*	@version 	V0.1
*	@date     2018/07/13
*	@brief		传感器采集
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
	{0x00, 0x00,				0x0000,     0x0000,         7,          9,    RS485_IDE+4,    200*1, "EXPEND",  	"",    		"EXPEND"},	///扩展盒
	{0x02, 0x02,				0x0000, 		0x0002,  				6,					9, 		RS485_IDE+4,		200*1,		"" ,			"" ,		"SWR-100W"},  ///土壤温湿度
	{0x05, 0x05,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		1000*1,		"" ,			"" ,		"ST_PH"		},  ///土壤、水PH
	{0x06, 0x06,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"ST_GH"		},	///光合有效
	{0x07, 0x07,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		1000*1,		"" ,			"" ,		"ST_Y/MW"	},  ///叶面温度
	{0x08, 0x08,				0x0001, 		0x0001,  				6,					7, 		RS485_IDE+2,		1000*1,		"" ,			"" ,		"ST_YMS"	},  ///叶面湿度
	{0x0C, 0x03,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"ST-TW"		},  ///土壤温度
	{0x0D, 0x0D,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"ST-EC"		},  ///EC
	{0x0E, 0x0E,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		1000*10,	"" ,			"" ,		"ST_CO2"	},	///CO2				
//	{0x0F, 0x24,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		1000*1,		"" ,			"" ,		"ST_AP"		},///气象站：大气压				
	{0x11, 0x11,				0x0000, 		0x0001,  				6,					9, 		RS485_IDE+4,		200*1,		"" ,			"" ,		"OXY"			},  ///水溶氧
	{0x12, 0x12,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"andan"		},  ///氨氮
	{0x13, 0x13,				0x0000, 		0x0002,  				6,					9, 		RS485_IDE+4,		200*1,		"" ,			"" ,		"Water-EC"},  ///水EC
	{0x14, 0x14,				0x0000, 		0x0001,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"Water-T" },  ///水温
	{0x15, 0x15,				0x0000, 		0x0002,  				6,					7, 		RS485_IDE+2,		200*1,		"" ,			"" ,		"Water-K+"},	///水钾+
	{0xFD, 0x18,				0x0000, 		0x0002,  				6,				 13, 		RS485_IDE+8,		1000*1,		"" ,			"" ,		"Air-Ill" },	///空气温湿度、光照
};

SaveRs485_t  SaveRs485s[6];

SendBuf_t		 SendBufs[10];

Sensor_t     Sensors;

volatile 	uint8_t SendBufsCounter = 0; ///记录数据缓存区数组大小

uint8_t OpenExpendBoxbuff[10] = {0x00,0x05,0x00,0x01,0x00,0x00,0x00};
uint8_t ExpendBoxbuff[9] = {0xFE,0x03,0x04,0x00,0x00,0x00,0x00,0x00,0x00};


/*
 *	SensorsInit:		传感器初始化
 *	参数：			  	无
 *	返回值：				无
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
	
	RS485CmdPackage(NBI_RS485_SEARCH_CODE);///获取预存485命令缓存
	
	Sensors.Counter = 0;
		
	Sensors.QueryPinStaus(  );
	
	Rs485s.PowerOff(  );
}

/*
 *	SensorHandle:	  传感器处理
 *	参数：			  	无
 *	返回值：				无
 */
void SensorHandle(void)
{			
	Rs485s.PowerOn(  );
	
	for(uint8_t id = 0 ; id < NBI_RS485_PIN_COUNT; id++)
	{
		DEBUG_APP(2,"Start get sensor data main box id = %d",++id);
		id--;
		if(Sensors.GetRs485Type( id ) != RS485_NONE)
		{
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
 *	SensorDataProces:	传感器数据处理
 *	参数：			  		无
 *	返回值：					无
 */
void SensorDataProces(void) 
{
	/**********运算长度**************/
	uint8_t Len = 0;
	
	/**********传感器类型BUF下标**************/
	uint8_t Index = 0;
	
	/**********实际数据总长度**************/
	uint8_t Length = 0; 
	
	/***********传感器缓存区*************/
	uint8_t Temp[50] = {0};
	
	/***********传感器数据长度*************/
	uint8_t TempIndex = 0;
	
	/***********剩余待获取传感器个数*************/
	uint8_t GetSensorCounter = 0;
	
	uint8_t ExId = 0;
	
	uint8_t PortId = 0;

#if 1

	DEBUG_APP(2, "-----Start get data Counter : %d----",Sensors.Counter);
	for(PortId = 0 ; PortId < NBI_RS485_PIN_COUNT; PortId++)
	{
		if(SaveRs485s[PortId].Type == RS485_SIGNAL && GetSensorCounter < Sensors.Counter)
		{
			Len += SaveRs485s[PortId].MainBox.SensorToLen;
			DEBUG_APP(2, "222PortId : %d Len = %d\r\n",PortId, Len);
			
			if(Len <= ZETAMAX)
			{
				SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.Index; //接口ID
				SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.Identifier; //解码标识
				
				DEBUG_APP(2,"TempIndex = %d",TempIndex);
				memcpy1(&Temp[TempIndex], &SaveRs485s[PortId].MainBox.SensorBuff[0], SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE);
				for(uint8_t i = 0; i< SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE; i++)
				{
					DEBUG(2,"%02x ",Temp[TempIndex + i]);
				}
				DEBUG(2,"\r\n");
				TempIndex += SaveRs485s[PortId].MainBox.SensorToLen-RS485_IDE ;
				Length += SaveRs485s[PortId].MainBox.SensorToLen;
				GetSensorCounter++;
								
				DEBUG_APP(2,"11SendBufsCounter = %d, Len = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",SendBufsCounter,Len, PortId, GetSensorCounter, Sensors.Counter);
			}
			else
			{
				/********************分包第几帧*****************/
				SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
				
				/********************电量*****************/
				SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
				
				/********************传感器个数*****************/
				SendBufs[SendBufsCounter].Buf[2] = Index/2;

				uint8_t BufLen = 3;
				
				memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index);
				
				BufLen += Index;
				memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex);
				
				Length += 3;
				
				SendBufs[SendBufsCounter].Len = Length;
								
				DEBUG_APP(2,"22SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
				for(uint8_t i = 0; i< Length; i++)
				DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
				DEBUG(2,"\r\n");
	
				memset(Temp, 0, TempIndex);
				
				PortId --; //下标回退	
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
				if(SaveRs485s[PortId].MainBox.ExpendBox[ExId].ExpenCheck) ///接口存在
				{
					Len += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen;
			
					DEBUG_APP(2,"PortId = %d, ExId = %d Len = %d SensorToLen = %d",PortId,ExId,Len, SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen);
					if(Len <= ZETAMAX)
					{
						SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.ExpendBox[ExId].Index; //接口ID
						SendBufs[SendBufsCounter].Port[Index++] = SaveRs485s[PortId].MainBox.ExpendBox[ExId].Identifier; //解码标识
						
						memcpy1(&Temp[TempIndex], &SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorBuff[0], SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen-RS485_IDE);

						TempIndex += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen-RS485_IDE;
						Length += SaveRs485s[PortId].MainBox.ExpendBox[ExId].SensorToLen;
						GetSensorCounter++;
						
						DEBUG_APP(2,"33SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
					}
					else
					{					
						/********************分包第几帧*****************/
						SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
						
						/********************电量*****************/
						SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
						
						/********************传感器个数*****************/
						SendBufs[SendBufsCounter].Buf[2] = Index/2;

						
						uint8_t BufLen = 3;
				
						memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index);
				
						BufLen += Index;
						memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex);
						
						Length += 3;

						SendBufs[SendBufsCounter].Len = Length;
						
						DEBUG_APP(2,"44SendBufsCounter = %d, Length = %d, PortId = %d,GetSensorCounter = %d, Sensors.Counter = %d",SendBufsCounter,Length, PortId, GetSensorCounter, Sensors.Counter);
						for(uint8_t i = 0; i<Length; i++)
						DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
						DEBUG(2,"\r\n");
				
						memset(Temp, 0, TempIndex);
						
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
		/********************分包第几帧*****************/
		SendBufs[SendBufsCounter].Buf[0] = SendBufsCounter;
		
		/********************电量*****************/
		SendBufs[SendBufsCounter].Buf[1] = CheckBattery(  );
		
		/********************传感器个数*****************/
		SendBufs[SendBufsCounter].Buf[2] = Index/2;

		uint8_t BufLen = 3;

		memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], SendBufs[SendBufsCounter].Port, Index); ///传感器标识
		DEBUG_APP(2, "444PortId : %d counter = %d\r\n",PortId, GetSensorCounter);
		
		DEBUG_APP(3,"Index = %d, \r\n",Index);

		BufLen += Index;
		
		DEBUG(2,"BufLen = %d, \r\n",BufLen);
		memcpy1(&SendBufs[SendBufsCounter].Buf[BufLen], Temp, TempIndex); ///传感器数据
		
		Length += 3;

		SendBufs[SendBufsCounter].Len = Length;
		
		DEBUG(2,"55SendBufsCounter = %d, Length = %d,TempIndex = %d, Length = %d\r\n",SendBufsCounter,Length,TempIndex,Length);
		for(uint8_t i = 0; i< Length; i++)
		DEBUG(2,"%02X ",SendBufs[SendBufsCounter].Buf[i]);
		DEBUG(2,"\r\n");

		memset(Temp, 0, TempIndex);
		
		SendBufsCounter ++;
		Len = 0;
		Length = 0;
		TempIndex = 0;	
		Index = 0;
	}
	
	Sensors.Counter = 0;
	GetSensorCounter = 0;
#endif	
}

/*
 *	GetRs485Type:		485接口类型
 *	参数：			  	主板Rs485接口ID
 *	返回值：				Rs485类型：RS485_NONE/RS485_EXPAND_BOX/RS485_SIGNAL
 */
Rstype_t GetRs485Type(int index)
{	
	return SaveRs485s[index].Type;
}

/*
 *	SensorQueryPinStaus:		广播查询485、读取传感器数据
 *	参数：			  					无
 *	返回值：								查询状态：成功/失败/超时
 */
HAL_StatusTypeDef SensorQueryPinStaus(void)
{	
	HAL_StatusTypeDef Status = HAL_TIMEOUT;
	
	Rs485s.GetData(Rs485s.Revbuff);
	for(int id = 0 ; id < NBI_RS485_PIN_COUNT ; id++)
	{
		//遍历plus的6个口，分别是什么
		//打开io口		
		Rs485s.OpenPin(id);
		HAL_Delay(1000);
		//判断是什么口，并且获取其中传感器的地址
		if(Sensors.QueryType(id) != RS485_NONE) 
		{			
			DEBUG(2,"pin %02x: find device\r\n,",id);
		}		
		else 
		{
			DEBUG(2,"pin %d:not find device\r\n,",id);
		}
		
		DEBUG(2,"g_index = %d\r\n",id);
	}
	Rs485s.ClosePin(  );				
	return Status;
}



/*
 *	SensorQueryType:		广播查询主板485类型
 *	参数：			  			Rs485接口
 *	返回值：						Rs485类型：RS485_NONE/RS485_EXPAND_BOX/RS485_SIGNAL
 */
Rstype_t SensorQueryType(int PortId)
{
	uint8_t repbuff[20] = {0};	
	uint8_t expend_sensor = 0;
	
	SaveRs485s[PortId].Type = RS485_NONE;
	
	int len = Rs485s.Cmd(ExpendBoxbuff,7, 1000);   // 地址广播：get expend return data 	
	
	memcpy(repbuff,Rs485s.Revbuff,len);
		
	Rs485s.Print(repbuff , len);
  ///判断广播回复地址：扩展盒地址应答
	
	if(len == CheckRs485s[EXPENDBOX].RevDataLen && repbuff[3] == 0) //扩展盒的地址 为0
	{
		//返回的数据长度大于0，认为是外部盒子
		DEBUG_APP(2,"expend box is ok");
		expend_sensor = repbuff[4];  	//扩展盒哪些口接入传感器
		
		Sensors.ExpendBoxLive(expend_sensor,PortId); ///扩展盒相应接口状态标志
        
		SaveRs485s[PortId].Type = RS485_EXPAND_BOX;
		
		Sensors.ExpBoxAddr( PortId );
          
		DEBUG_APP(2,"sensor get expen box address success PortId : %d",++PortId);	
		PortId--;
	}
	else if(len >5) ///非扩展盒地址应答
	{				
		//没有找到扩展盒但是单个口有了回复
		DEBUG_APP(2,"siganal ok and has rpy addr = %x",repbuff[3]);		
		int i = 0;

		////判断接口对应接入485传感器地址，记录
		for(i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
		{
			if(CheckRs485s[i].Addr == repbuff[3]) 
			{					
				SaveRs485s[PortId].Type = RS485_SIGNAL;
				
				SaveRs485s[PortId].MainBox.Index = PortId;  
				SaveRs485s[PortId].MainBox.CheckIndex = i; ///记录查询下标
				SaveRs485s[PortId].MainBox.Identifier = CheckRs485s[i].Identifier; ///解码标识
				SaveRs485s[PortId].MainBox.SensorToLen = CheckRs485s[i].SensorToLen; ///传感器总长度		
							
				DEBUG_APP(2,"add device ok id = %d",i);		
				break;
			}
		}
		if(i == ARRAY(CheckRs485s))
		{
				//没有找到
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
		//没有找到扩展盒，单个口也没有回复，开始遍历所有的外部设备的数据
		DEBUG_WARNING(2,"siganal ok and foreach");		
		uint32_t startTime = HAL_GetTick();

//		for(int i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
		for(int i = 4 ; i < 6 ; i ++) ///只查询叶面传感器
		{			
			len = Rs485s.Cmd(CheckRs485s[i].SendBuff, CheckRs485s[i].SendDataLen, CheckRs485s[i].TimeOut);
			while(HAL_GetTick() - startTime < CheckRs485s[i].TimeOut && len != CheckRs485s[i].RevDataLen)
			{
				len = Rs485s.Cmd(CheckRs485s[i].SendBuff,CheckRs485s[i].SendDataLen, CheckRs485s[i].TimeOut);				
				HAL_Delay(500);
			}
			if(len == CheckRs485s[i].RevDataLen)
			{
				//找到了
				DEBUG_APP(2,"device had find:%02x",CheckRs485s[i].Addr);
				
				SaveRs485s[PortId].MainBox.Index = PortId;
				SaveRs485s[PortId].MainBox.CheckIndex = i; ///记录查询下标
				SaveRs485s[PortId].MainBox.Identifier = CheckRs485s[i].Identifier; ///解码标识
				SaveRs485s[PortId].MainBox.SensorToLen = CheckRs485s[i].SensorToLen; ///传感器总长度		

				SaveRs485s[PortId].Type = RS485_SIGNAL;
				break;
			}
			else
			{
        DEBUG_APP(2,"device had not find g_rs485index:%02x",++PortId);
				PortId--;
				SaveRs485s[PortId].Type = RS485_NONE;
			}
		}		
	}
	return SaveRs485s[PortId].Type;
}

/*
 *	SensorExpendBoxLive:		判断扩展盒哪些口接入传感器
 *	参数：			  					扩展盒接入标识，主板485ID
 *	返回值：								无
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
        DEBUG_APP(2,"mainBox index : %02x",index);                
			}
			break;
		}
		if(expend_sensor & (0x01 << i))
		{
      DEBUG_APP(2,"mainBox index : %02x",index);             
			SaveRs485s[index].MainBox.ExpendBox[i].ExpendBoxLive = true;
		}       
	}	
}

/*
 *	SensorExpBoxAddr:		查询扩展接入传感器地址
 *	参数：			  			主板485ID
 *	返回值：						查询状态：成功/失败
 */
HAL_StatusTypeDef SensorExpBoxAddr(int index)
{
	uint8_t len = 0;
	uint8_t repbuff[20] = {0};	

	for(int ExpId = 0 ; ExpId < 5 ; ExpId ++)
	{
		SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = false;
		
		if( SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive )
		{			
			DEBUG_APP(2,"main port=%d ExpId = %d",index, ExpId);

			if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(ExpId))
			{
				DEBUG_ERROR(2,"main port = %d, exbox port=%d sensor open faile",index,ExpId);				
				continue;
			}
			else
			{
				DEBUG_APP(2,"exbox port=%d sensor open success",ExpId);
			}
			
		  len = Rs485s.Cmd(ExpendBoxbuff,7, 1000);   // 地址广播：get expend return data 	
	
			memcpy(repbuff,Rs485s.Revbuff,len);
	
			DEBUG_APP(2,"rs485 ExpendBox get data = ");			
			Rs485s.Print(repbuff,len);
			
			if(len >5) ///广播应答
			{				
				DEBUG_APP(2,"siganal ok and has rpy addr = %x",repbuff[3]);		
				int i = 0;

				////判断接口对应接入485传感器地址，记录
				for(i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
				{
					if(CheckRs485s[i].Addr == repbuff[3]) 
					{											
						uint8_t ExpIndex = index;
						ExpIndex++;
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = true;
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Index = (ExpIndex<<4)|ExpId;   
						SaveRs485s[index].MainBox.ExpendBox[ExpId].CheckIndex = i; ///记录查询下标
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Identifier = CheckRs485s[i].Identifier; ///解码标识
						SaveRs485s[index].MainBox.ExpendBox[ExpId].SensorToLen = CheckRs485s[i].SensorToLen; ///传感器总长度		
									
						DEBUG_APP(2,"add device ok id = %d",i);		
						break;
					}
				}
				if(i == ARRAY(CheckRs485s))
				{
						//没有找到
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive = false;				
						DEBUG_ERROR(2,"ExpendBox address = %d not in array",repbuff[3]);		
				}
			}
			else
			{
				//广播信息没有返回，认为这个传感器不接受FE查询
        //遍历
        DEBUG_ERROR(2,"port=%d sensor don`t return FE statr foreach",ExpId);			
				uint32_t startTime = HAL_GetTick();
				int i;
				
//				for( i = 0 ; i < ARRAY(CheckRs485s) ; i ++)
				for(int i = 4 ; i < 6 ; i ++) ///只查询叶面传感器
				{
					len = Rs485s.Cmd(CheckRs485s[i].SendBuff,CheckRs485s[i].SendDataLen, CheckRs485s[i].TimeOut);
				
					if(len == CheckRs485s[i].RevDataLen)
					{
						//找到了
						DEBUG_APP(2,"device had find:%02x",CheckRs485s[i].Addr);
						uint8_t ExpIndex = index;
						ExpIndex++;
						
						SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck = true;
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Index = (ExpIndex<<4)|ExpId;   
						SaveRs485s[index].MainBox.ExpendBox[ExpId].CheckIndex = i; ///记录查询下标
						SaveRs485s[index].MainBox.ExpendBox[ExpId].Identifier = CheckRs485s[i].Identifier; ///解码标识
						SaveRs485s[index].MainBox.ExpendBox[ExpId].SensorToLen = CheckRs485s[i].SensorToLen; ///传感器总长度
						break;
					}
				}
				if(i == ARRAY(CheckRs485s))
				{
					//没有找到
					SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpendBoxLive = false;				
					DEBUG_ERROR(2,"ExpendBox address = %d not in array",repbuff[3]);	
				}
			}			
		}
	}	
		
	//关闭 扩展盒
	OpenExpendBoxbuff[5] = 0x00;
	len = Rs485s.Cmd(OpenExpendBoxbuff,7, 200);		
	if(len == 0)
	{
			DEBUG_ERROR(2,"close expend box falie");		
			return HAL_ERROR;		
	}
	else
	{
			DEBUG_APP(2,"close expend box");
			return HAL_OK;
	}
}

/*
 *	SensorMaBoxData:		读取主板Rs485传感器数据
 *	参数：			  			主板485ID
 *	返回值：						查询状态：成功/失败
 */
HAL_StatusTypeDef SensorMaBoxData(uint8_t id)
{
	uint8_t len = 0;	
	uint32_t startTime = 0;
	uint32_t getDataTimeOut = 4000;
	
	len = Rs485s.Cmd(CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendDataLen, CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].TimeOut);
	//等待发送完成,				
	startTime = HAL_GetTick();
	while(HAL_GetTick() - startTime < getDataTimeOut && len != CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].RevDataLen)
	{
		len = Rs485s.Cmd(CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].SendDataLen, CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].TimeOut);
		HAL_Delay(500);
	}
	if(len != CheckRs485s[SaveRs485s[id].MainBox.CheckIndex].RevDataLen)
	{
		DEBUG_APP(2,"Get MainBox : %02x data error",SaveRs485s[id].MainBox.Index);
		return HAL_ERROR;
	}

	Sensors.Counter ++;
	memset(SaveRs485s[id].MainBox.DataBuff,0,len);
	memcpy(SaveRs485s[id].MainBox.DataBuff,Rs485s.Revbuff,len);
	DEBUG(2,"main_box = %d, sensor Rs485 revData : ",++id);
	id--;
	Rs485s.Print(SaveRs485s[id].MainBox.DataBuff,len);				

	//get data
	for(int i = 0,j = 3 ; j < len-2 ; j ++)
	{
		SaveRs485s[id].MainBox.SensorBuff[i] = SaveRs485s[id].MainBox.DataBuff[j];
		DEBUG_APP(2,"get data[%d] = 0x%02x\r\n",j,SaveRs485s[id].MainBox.SensorBuff[i]);	
		i++;
	}
	return HAL_OK;
}

/*
 *	SensorExpenData:		读取扩展盒Rs485传感器数据
 *	参数：			  			主板485ID
 *	返回值：						查询状态：成功/失败
 */
HAL_StatusTypeDef SensorExpenData(uint8_t index)
{
	uint8_t len = 0;
	
	uint32_t startTime = 0;
	uint32_t getDataTimeOut = 4000;
	
	for(int Exid = 0; Exid < 5; Exid++)
	{
		/***********************判断对应485接口是否接入传感器***********************************/
		if( SaveRs485s[index].MainBox.ExpendBox[Exid].ExpenCheck ) 
		{
			/***********************打开扩展盒接口状态***********************************/
			if(CheckRs485s[EXPENDBOX].RevDataLen != OpenExpenBox(Exid))
			{
				DEBUG_ERROR(2,"main port = %d, exbox port=%d sensor open faile",index,Exid);				
				continue;
			}
			
			len = Rs485s.Cmd(CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].SendBuff,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].SendDataLen,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].TimeOut);
			//等待发送完成,				
			startTime = HAL_GetTick();
			while(HAL_GetTick() - startTime < getDataTimeOut && len != CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].RevDataLen)
			{
				len = Rs485s.Cmd(CheckRs485s[SaveRs485s[index].MainBox.CheckIndex].SendBuff,CheckRs485s[SaveRs485s[index].MainBox.CheckIndex].SendDataLen,CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].TimeOut);
				HAL_Delay(500);
			}
			if(len != CheckRs485s[SaveRs485s[index].MainBox.ExpendBox[Exid].CheckIndex].RevDataLen)
			{
				DEBUG_ERROR(2,"Get Exid : %02x data error",SaveRs485s[index].MainBox.ExpendBox[Exid].Index);	
			}
			else
			{

				Sensors.Counter ++;
				
				memset(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,0,len);
				memcpy(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,Rs485s.Revbuff,len);

				DEBUG(2,"main_box = %d, exbox port=%d Index = 0x%02x sensor Rs485 revData : ",++index,++Exid,SaveRs485s[index].MainBox.ExpendBox[Exid].Index);
				index--;
				Exid--;
				Rs485s.Print(SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff,len);				

				//get data
				for(int i = 0,j = 3 ; j < len - 2; j ++)
				{
					SaveRs485s[index].MainBox.ExpendBox[Exid].SensorBuff[i] = SaveRs485s[index].MainBox.ExpendBox[Exid].DataBuff[j];
					DEBUG_APP(2,"get data[%d] = 0x%02x\r\n",j,SaveRs485s[index].MainBox.ExpendBox[Exid].SensorBuff[i]);	
					i++;
				}	
			}			
		}
	}
	return HAL_OK;
}

/*
 *	SensorGetData:		获取Rs185传感器数据
 *	参数：			  		Rs485主板接口
 *	返回值：					获取成功/失败
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
*OpenExpenBox：打开扩展盒接口
*ExpId：			 扩展口ID
*返回：				 扩展盒返回数据长度
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
	len = Rs485s.Cmd(OpenExpendBoxbuff,7, 200);

	DEBUG_APP(2,"Rs485s--len = %d",len);
	return len;
}

void RS485CmdPackage(char mode)
{
	for(int index = 0 ; index < ARRAY(CheckRs485s); index ++)
	{
//		DEBUG_APP(2,"index = %d,addr = %02X\r\n",index,CheckRs485s[index].Addr);
		memset(CheckRs485s[index].SendBuff,0,sizeof(CheckRs485s[index].SendBuff));
		CheckRs485s[index].SendBuff[0] = CheckRs485s[index].Addr;
		CheckRs485s[index].SendBuff[1] = mode;
		CheckRs485s[index].SendBuff[2] = (CheckRs485s[index].RegAddress & 0xFF00)  >> 8;		
		CheckRs485s[index].SendBuff[3] = (CheckRs485s[index].RegAddress & 0x00FF);
		CheckRs485s[index].SendBuff[4] = (CheckRs485s[index].RegDatalen & 0xFF00)  >> 8;				
		CheckRs485s[index].SendBuff[5] = (CheckRs485s[index].RegDatalen & 0x00FF);
	}
}

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}
