
	
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
4.实现下发数据更改上报命令最大长度功能。
4.实现双色温指示灯功能：发送定位信息时绿灯点亮，数据接收成功黄灯点亮，申请入网及数据发送或接受失败红灯点亮。





尊敬的管理员：
??? 您好！
??? 登录信息已经变更如下：
??? 用户名：nbcx
??? 密码：ca722db37e1a10f6

??? 您可在登录后，于页面的右上角进行密码修改,为了您的账号安全,请勿使用过于简单的密码。

??? 为了您更好的使用体验，我们推荐您使用Google Chrome浏览器

??? 信息管理系统地址:?http://www.zeta-alliance.com:25450/teamcms/homePage


.\Objects\Zetav-plus.axf: Error: L6218E: Undefined symbol SetLedStates (referred from gps.o).
.\Objects\Zetav-plus.axf: Error: L6218E: Undefined symbol LedSendFail (referred from user-app.o).
.\Objects\Zetav-plus.axf: Error: L6218E: Undefined symbol LedSendSucess (referred from user-app.o).
.\Objects\Zetav-plus.axf: Error: L6218E: Undefined symbol SetLedStates (referred from gps.o).

