#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stm32l0xx_hal.h"
										
#define STM32_FLASH_SIZE 							128	 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_BASE 							0x08000000 		//flash����ַ
									 							  
#define READ_FLASH(faddr)							(*(volatile uint16_t*)faddr) 	
#define STMFLASH_ReadWord(faddr)			(*(volatile uint32_t*)faddr) 	
					  
#if STM32_FLASH_SIZE<256							
#define STM_SECTOR_SIZE 							1024 			//�е�����flash���ͺ�ҳ���С��1024�ֽ�
#else												
#define STM_SECTOR_SIZE 							2048 			//������flash���ͺ�ҳ���С��2048�ֽ�
#endif												
															
#define UNLOCK_FLASH()								{FLASH->KEYR=0X45670123;FLASH->KEYR=0XCDEF89AB;}//FLASH����
#define LOCK_FLASH()									(FLASH->CR|=1<<7)								//FLASH����
#define FLASH_STATUS 									(FLASH->SR)										//FLASH״̬�Ĵ���
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

#define FLAG_ADD											0x0801fd10			//���ݼĴ����Ĵ���λ�ã����ڼ�¼���ֱ�ʶλ
#define	FLAG_ADD_SIZE									0x10


#define FLAG	FlashRead32(FLAG_ADD)		

typedef enum FLAGTYPE						//��־����,���ֵΪ31
{
	SOFT_RESET=0x00,						//�����λ��־
	UPDATE_RESET,							//���¹̼���־
	DOWNLINK_CONFIRM,						//����ȷ�ϱ�־,���ڻظ������������п�������
	CLEAN_FLASH,							//��λ��������Flash�е�����
	GET_LOCATION,							//��ȡλ����Ϣ
	UPDATE_SUCCEED,
	UPDATE_FAIL,
  CLOSE_IWDG,                             //�رն������Ź���Ҳ�ǽ������ߵı�־λ
	END_OF_FLAGTYPE
}FlagType;

/*
 *	FlashReadPage:		��ȡ1ҳ����
 *	����PageAddr��		ҳ��ַ
 *	����pBuffer��		�����ȡ�������ݵ�ָ��
 *	����ֵ��			1�ɹ� 0ʧ��	
 */
uint8_t FlashReadPage(uint32_t PageAddr, uint32_t *pBuffer);
/*
 *	FlashWritePage:		д1ҳ����
 *	����PageAddr��		ҳ��ַ
 *	����pBuffer��		����д��Flash�е�����ָ��
 *	����ֵ��			1�ɹ� 0ʧ��	
 */
uint8_t FlashWritePage( uint32_t PageAddr, uint32_t *pPageBuffer);

/*
 *	FlashWrite32:		д4�ֽ�(32λ)����
 *	����WriteAddr��		��������Flash�еĵ�ַ
 *	pBuffer:			����д��Flash�е�����ָ��
 *	NumToWrite			���ݳ���(С��ҳ��С/4)
 *	����ֵ��			1�ɹ� 0ʧ��			
 */
uint8_t FlashWrite32( uint32_t WriteAddr, uint32_t * pBuffer, uint16_t NumToWrite );
/*
 *	FlashWrite16:		д2�ֽ�(16λ)����
 *	����WriteAddr��		��������Flash�еĵ�ַ
 *	pBuffer:			����д��Flash�е�����ָ��
 *	NumToWrite			���ݳ���(С��ҳ��С/2)
 *	����ֵ��			1�ɹ� 0ʧ��			
 */
uint8_t FlashWrite16( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );

/*
 *	FlashRead32:		��ȡ4�ֽ�(32λ)����
 *	����ReadAddr��		��������Flash�еĵ�ַ
 *	����ֵ��			���ض�ȡ��������		
 */
uint32_t FlashRead32(uint32_t ReadAddr );
/*
 *	FlashRead16:		��ȡ2�ֽ�(16λ)����
 *	����ReadAddr��		��������Flash�еĵ�ַ
 *	����ֵ��			���ض�ȡ��������		
 */
uint16_t FlashRead16(uint32_t ReadAddr );

/*
 *	FlashRead8:		��ȡ1�ֽ�(8λ)����
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����ֵ��		���ض�ȡ��������		
 */
uint8_t FlashRead8(uint32_t ReadAddr );
/*
 *	FlashReadChar:	��ȡ�ֽ�(8λ)����
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����pBuffer��	�����ȡ�������ݵ�ָ��
 *	����NumToRead��	��ȡ�ĳ���
 *	����ֵ��		ʵ�ʶ�ȡ�����ַ�����,��ȡ���󷵻�0
 */
uint16_t FlashReadChar(uint32_t ReadAddr,char* pBuffer,uint16_t NumToRead);


void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   	

/*
 *	FlashRead16More:	��ȡ���2�ֽ�(16λ)����
 *	����ReadAddr��	��������Flash�еĵ�ַ
 *	����pBuffer��	�����ȡ�������ݵ�ָ��
 *	����NumToRead��	��ȡ�ĳ���
 *	����ֵ��		1�ɹ� 0ʧ��
 */
uint16_t FlashRead16More(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);

/*
 *	ReadSerialNumber:	��flash��ȡ���к�
 * 	device_id:			�豸ID
 *	stream:				������
 *	server_add:			������IP��ַ
 *	server_port:		�������˿�
 *	����ֵ��				��ȡ�ɹ��򷵻�0�����򷵻�1
 */
uint8_t ReadSerialNumber(char *device_id,char *stream,char *server_addr,char *server_port);

/*
 *	SetFlag:			�޸�FLAG_ADD��ַ��ֵ��������ر�־λ
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			��	
 */
void SetFlag(FlagType flag);

/*
 *	CleanFlag:			�޸�FLAG_ADD��ַ��ֵ�������ر�־λ
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			��	
 */
void CleanFlag(FlagType flag);

/*
 *	CheckFlag:			��ѯ��ر�־λ�Ƿ񱻱�־
 *	����flag��			ֻ��ΪFlagType����
 *	����ֵ��			����־����>0��ֵ��δ��־����0
 */
uint8_t CheckFlag(FlagType flag);
 

#endif	

