
	
1：硬件IO接口图：

								stm32L072CBT6
SX1278						 _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
SPI     NSS	  --------	PA4 |					  		|
		SCK	  --------	PA5 |    				  		|
		MISO  --------	PA6 |					 		|
		MOSI  --------	PA7 |					  		|
						    |					  		|
EXti	DIO0  --------	PB1 |                    		|
		DIO1  --------	PB2 |					  		|
		DIO2  --------	PB10|					  		|
		DIO3  --------	PB11|					 		|
		DIO4  --------	NC	|					  		|
		DIO5  --------	NC	|					 		|
							|					  		|
CPIO	RESET --------	PB0 |					  		|
		LoRa_Power ---  PB12|					 		|
							|					 		|
							|					 		|
GPS	(UART2)					|					 		|	
		TX	  --------  PA2	|					  		|	
		RX	  --------  PA3	|					  		|	
GPS_Power_ON  --------  PB7	|					  		|									
							|					  		|
485	(UART5)					|					 		|	
		485_TX	------	PB3	|					  		|	
		485_RX	------	PB4	|					  		|	
		485_DE	------	PB5	|					  		|
		12V_ON	------  PA8	|					  		|	
							|					  		|
							|					 		|	
DEBUG(UART1)				|					  		|
		TX   ---------	PA9	|					  		|
		RX	 ---------  PA10|					  		|
							|					  		|
I2C							|					  		|
		I2C2_SDA ----- PB14	|					  		|
		I2C2_SCL ----- PB13	|					  		|
							|					  		|
电源管理使能  -------- PB9	|					  		|
							|					  		|
							|					  		|
							|					  		|
							|_ _ _ _ _ _ _ _ _ _ _ _ _ _|	



版本说明：
【1】：ZETA-V0.7
优化功能：
【1】：增加GPS定位
【2】：增加Zeta下行更改上报周期跟GPS重新定位命令

【1】：ZETA-V0.6

优化功能：
【1】：实现基本通讯
【2】：增加Zeta下行数据脉冲触发
【3】：增加主控休眠机制
【4】：增加Zeta下行数据INT触发MCU
【5】：增加RTC休眠唤醒
【6】：增加Rs485读取数据
【7】：解决Rs485打开扩展盒第一口时接收到多一个0x00数据，Sensors.QueryPinStaus中调用Rs485s.GetData：
	   导致串口接收到多一个0x00
【8】：增加WWDG窗口看门狗
【9】：增加Timer
【10】：增加传感器异常过滤机制，使用SensorToLen是否为0判断传感器是否异常


