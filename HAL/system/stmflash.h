#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stm32l0xx_hal.h"
										
#define STM32_FLASH_SIZE 							128	 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_BASE 							0x08000000 		//flash基地址
									 							  
#define READ_FLASH(faddr)							(*(volatile uint16_t*)faddr) 	
#define STMFLASH_ReadWord(faddr)			(*(volatile uint32_t*)faddr) 	
					  
#if STM32_FLASH_SIZE<256							
#define STM_SECTOR_SIZE 							1024 			//中等容量flash的型号页面大小是1024字节
#else												
#define STM_SECTOR_SIZE 							2048 			//大容量flash的型号页面大小是2048字节
#endif												
															
#define UNLOCK_FLASH()								{FLASH->KEYR=0X45670123;FLASH->KEYR=0XCDEF89AB;}//FLASH解锁
#define LOCK_FLASH()									(FLASH->CR|=1<<7)								//FLASH上锁
#define FLASH_STATUS 									(FLASH->SR)										//FLASH状态寄存器
#define FLASH_DONE										0
#define FLASH_BUSY										1
#define FLASH_PROGRAM_ERROR						1<<2
#define FLASH_WRITE_ERROR							1<<4
#define FLASH_TIMEOUT									0xee
#define FLASH_ERROR										0xff	

#define DEV_ADDR                      0x0801F3E8        ////UID 0x0801F3E8 ---- 0x0801F3FA
#define DEV_ADDR_SIZE                 0x12

#define SLEEP_ADDR										0x0801F3D4
#define SLEEP_ADDR_SIZE               0x01

#define FLAG_ADD											0x0801fd10			//备份寄存器的代替位置，用于记录各种标识位
#define	FLAG_ADD_SIZE									0x10


#define FLAG	FlashRead32(FLAG_ADD)		

typedef enum FLAGTYPE						//标志类型,最大值为31
{
	SOFT_RESET=0x00,						//软件复位标志
	UPDATE_RESET,							//更新固件标志
	DOWNLINK_CONFIRM,						//下行确认标志,用于回复服务器的下行控制命令
	CLEAN_FLASH,							//复位所有数据Flash中的数据
	GET_LOCATION,							//获取位置信息
	UPDATE_SUCCEED,
	UPDATE_FAIL,
  CLOSE_IWDG,                             //关闭独立看门狗，也是进入休眠的标志位
	END_OF_FLAGTYPE
}FlagType;

/*
 *	FlashReadPage:		读取1页数据
 *	参数PageAddr：		页地址
 *	参数pBuffer：		保存读取到的数据的指针
 *	返回值：			1成功 0失败	
 */
uint8_t FlashReadPage(uint32_t PageAddr, uint32_t *pBuffer);
/*
 *	FlashWritePage:		写1页数据
 *	参数PageAddr：		页地址
 *	参数pBuffer：		用于写入Flash中的数据指针
 *	返回值：			1成功 0失败	
 */
uint8_t FlashWritePage( uint32_t PageAddr, uint32_t *pPageBuffer);

/*
 *	FlashWrite32:		写4字节(32位)数据
 *	参数WriteAddr：		该数据在Flash中的地址
 *	pBuffer:			用于写入Flash中的数据指针
 *	NumToWrite			数据长度(小于页大小/4)
 *	返回值：			1成功 0失败			
 */
uint8_t FlashWrite32( uint32_t WriteAddr, uint32_t * pBuffer, uint16_t NumToWrite );
/*
 *	FlashWrite16:		写2字节(16位)数据
 *	参数WriteAddr：		该数据在Flash中的地址
 *	pBuffer:			用于写入Flash中的数据指针
 *	NumToWrite			数据长度(小于页大小/2)
 *	返回值：			1成功 0失败			
 */
uint8_t FlashWrite16( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );

/*
 *	FlashRead32:		读取4字节(32位)数据
 *	参数ReadAddr：		该数据在Flash中的地址
 *	返回值：			返回读取到的数据		
 */
uint32_t FlashRead32(uint32_t ReadAddr );
/*
 *	FlashRead16:		读取2字节(16位)数据
 *	参数ReadAddr：		该数据在Flash中的地址
 *	返回值：			返回读取到的数据		
 */
uint16_t FlashRead16(uint32_t ReadAddr );

/*
 *	FlashRead8:		读取1字节(8位)数据
 *	参数ReadAddr：	该数据在Flash中的地址
 *	返回值：		返回读取到的数据		
 */
uint8_t FlashRead8(uint32_t ReadAddr );
/*
 *	FlashReadChar:	读取字节(8位)数据
 *	参数ReadAddr：	该数据在Flash中的地址
 *	参数pBuffer：	保存读取到的数据的指针
 *	参数NumToRead：	读取的长度
 *	返回值：		实际读取到的字符长度,读取错误返回0
 */
uint16_t FlashReadChar(uint32_t ReadAddr,char* pBuffer,uint16_t NumToRead);


void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   	

/*
 *	FlashRead16More:	读取多个2字节(16位)数据
 *	参数ReadAddr：	该数据在Flash中的地址
 *	参数pBuffer：	保存读取到的数据的指针
 *	参数NumToRead：	读取的长度
 *	返回值：		1成功 0失败
 */
uint16_t FlashRead16More(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);

/*
 *	ReadSerialNumber:	从flash读取序列号
 * 	device_id:			设备ID
 *	stream:				数据流
 *	server_add:			服务器IP地址
 *	server_port:		服务器端口
 *	返回值：				读取成功则返回0，否则返回1
 */
uint8_t ReadSerialNumber(char *device_id,char *stream,char *server_addr,char *server_port);

/*
 *	SetFlag:			修改FLAG_ADD地址的值，设置相关标志位
 *	参数flag：			只能为FlagType类型
 *	返回值：			无	
 */
void SetFlag(FlagType flag);

/*
 *	CleanFlag:			修改FLAG_ADD地址的值，清除相关标志位
 *	参数flag：			只能为FlagType类型
 *	返回值：			无	
 */
void CleanFlag(FlagType flag);

/*
 *	CheckFlag:			查询相关标志位是否被标志
 *	参数flag：			只能为FlagType类型
 *	返回值：			被标志返回>0的值，未标志返回0
 */
uint8_t CheckFlag(FlagType flag);
 

#endif	

