
	
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



1：实现下发更改参数功能：具备唤醒MCU

2：周期上报数据

3：第一次上电获取是否注册：0xff 0x00 0x04 0x43：没注册

.\Objects\watcherPRO_V2.2: Error: L6200E: Symbol UserZetaData multiply defined (by power.o and usart.o).


ff 00 0e 02 68 65 6c 6c 6f 77 6f 72 6c 64





尊敬的管理员：
??? 您好！
??? 登录信息已经变更如下：
??? 用户名：nbcx
??? 密码：ca722db37e1a10f6

??? 您可在登录后，于页面的右上角进行密码修改,为了您的账号安全,请勿使用过于简单的密码。

??? 为了您更好的使用体验，我们推荐您使用Google Chrome浏览器

??? 信息管理系统地址:?http://www.zeta-alliance.com:25450/teamcms/homePage



unsigned char crc_high_first(unsigned char *ptr, unsigned char len)
{
    unsigned char i; 
    unsigned char crc=0x00； /* 计算的初始crc值 */ 

    while(len--)
    {
        crc ^= *ptr++;  /* 每次先与需要计算的数据异或,计算完指向下一数据 */  
        for (i=8; i>0; --i)   /* 下面这段计算过程与计算一个字节crc一样 */  
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
