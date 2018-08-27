
	
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
【0】：ZETA-V0.9
优化功能：
【1】：增加Zeta入网超时1min进入休眠机制，防止长期入网操作

【0】：ZETA-V0.8
优化功能：
【1】：解决TIMER2定时检查GPS定位信息异常问题：设置TIMER中断优先级>GPS串口处理中断优先级
【2】：解决接收下行数据写FLASH出现擦除失败原因：开启TIMER2造成，解决方法：第一次擦除失败后延时50ms，再次擦除则成功

【0】：ZETA-V0.7
优化功能：
【1】：增加GPS定位
【2】：增加Zeta下行更改上报周期跟GPS重新定位命令

【0】：ZETA-V0.6
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
【11】：解决Rs485打开扩展盒第一口时接收到多一个0x00数据，Sensors.QueryPinStaus中调用Rs485s.GetData，同时Sensors.Handle中开启12V调用Rs485s.GetData，可以解决串口异常
	   




strlen与sizeof区别
sizeof是求数据类型所占的空间大小,而strlen是求字符串的长度，字符串以/0结尾



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



尊敬的管理员：
??? 您好！
??? 登录信息已经变更如下：
??? 用户名：nbcx
??? 密码：ca722db37e1a10f6

??? 您可在登录后，于页面的右上角进行密码修改,为了您的账号安全,请勿使用过于简单的密码。

??? 为了您更好的使用体验，我们推荐您使用Google Chrome浏览器

??? 信息管理系统地址:?http://www.zeta-alliance.com:25450/teamcms/homePage