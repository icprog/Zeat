
	
1��Ӳ��IO�ӿ�ͼ��

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
��Դ����ʹ��  -------- PB9	|					  		|
							|					  		|
							|					  		|
							|					  		|
							|_ _ _ _ _ _ _ _ _ _ _ _ _ _|	



�汾˵����
��0����ZETA-V0.8
�Ż����ܣ�
��1�������TIMER2��ʱ���GPS��λ��Ϣ�쳣���⣺����TIMER�ж����ȼ�>GPS���ڴ����ж����ȼ�
��2�������������������дFLASH���ֲ���ʧ��ԭ�򣺿���TIMER2��ɣ������������һ�β���ʧ�ܺ���ʱ50ms���ٴβ�����ɹ�

��0����ZETA-V0.7
�Ż����ܣ�
��1��������GPS��λ
��2��������Zeta���и����ϱ����ڸ�GPS���¶�λ����

��0����ZETA-V0.6
�Ż����ܣ�
��1����ʵ�ֻ���ͨѶ
��2��������Zeta�����������崥��
��3���������������߻���
��4��������Zeta��������INT����MCU
��5��������RTC���߻���
��6��������Rs485��ȡ����
��7�������Rs485����չ�е�һ��ʱ���յ���һ��0x00���ݣ�Sensors.QueryPinStaus�е���Rs485s.GetData��
	   ���´��ڽ��յ���һ��0x00
��8��������WWDG���ڿ��Ź�
��9��������Timer
��10�������Ӵ������쳣���˻��ƣ�ʹ��SensorToLen�Ƿ�Ϊ0�жϴ������Ƿ��쳣
��11�������Rs485����չ�е�һ��ʱ���յ���һ��0x00���ݣ�Sensors.QueryPinStaus�е���Rs485s.GetData��ͬʱSensors.Handle�п���12V����Rs485s.GetData�����Խ�������쳣
	   




strlen��sizeof����
sizeof��������������ռ�Ŀռ��С,��strlen�����ַ����ĳ��ȣ��ַ�����/0��β



Counter:0,GLL--$GPGLL,2233.0257,N,11356.5984,E,033621.000,A,A*5C
$GPGLL,2233.0257,N,11356.5984,E,033621.000,A,A*5C
App:..\HAL\sensor\src\gps.c,line : 134,GetPation = 1 Posfix = 1
22---22.55042 N, 113.94331 E
App:..\HAL\sensor\src\gps.c,line : 150,len = 6, N = 4e, C = N
App:..\HAL\sensor\src\gps.c,line : 150,len = 11, N = 45, C = E
App:..\USER\user-app.c,line : 185,len = 0B 
ZetaSendBuf: FD 00 22 68 C2 4E 00 AD DD 1B 45 
start send data
FF 00 18 02 13 07 31 00 02 11 FD 00 22 68 C2 4E 00 AD DD 1B 45 00 02 F9 
ff 00 04 01 
