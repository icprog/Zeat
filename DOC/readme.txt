
	
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



1��ʵ���·����Ĳ������ܣ��߱�����MCU

2�������ϱ�����

3����һ���ϵ��ȡ�Ƿ�ע�᣺0xff 0x00 0x04 0x43��ûע��

.\Objects\watcherPRO_V2.2: Error: L6200E: Symbol UserZetaData multiply defined (by power.o and usart.o).


ff 00 0e 02 68 65 6c 6c 6f 77 6f 72 6c 64





�𾴵Ĺ���Ա��
??? ���ã�
??? ��¼��Ϣ�Ѿ�������£�
??? �û�����nbcx
??? ���룺ca722db37e1a10f6

??? �����ڵ�¼����ҳ������Ͻǽ��������޸�,Ϊ�������˺Ű�ȫ,����ʹ�ù��ڼ򵥵����롣

??? Ϊ�������õ�ʹ�����飬�����Ƽ���ʹ��Google Chrome�����

??? ��Ϣ����ϵͳ��ַ:?http://www.zeta-alliance.com:25450/teamcms/homePage



unsigned char crc_high_first(unsigned char *ptr, unsigned char len)
{
    unsigned char i; 
    unsigned char crc=0x00�� /* ����ĳ�ʼcrcֵ */ 

    while(len--)
    {
        crc ^= *ptr++;  /* ÿ��������Ҫ������������,������ָ����һ���� */  
        for (i=8; i>0; --i)   /* ������μ�����������һ���ֽ�crcһ�� */  
        { 
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }

    return (crc); 
}


Program Size: Code=70778 RO-data=3770 RW-data=2640 ZI-data=6984  

SaveRs485s[index].MainBox.ExpendBox[ExpId].ExpenCheck

	Sensors.MaBoxData 			= SensorMaBoxData;
