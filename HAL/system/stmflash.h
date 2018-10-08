#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include "stm32l0xx_hal.h"

#define DEV_ADDR					0x0801ffe0			//�豸ID�洢��ַ
#define DEV_ADDR_SIZE			0x12				//�豸IDռ�ô洢�ռ�

#define SLEEP_ADDR				0x0801ffc0			//����ʱ��洢��ַ 
#define SLEEP_ADDR_SIZE		0x04				//����ʱ��ռ�ô洢�ռ�

#define MAXLEN_ADDR				0x0801ffbc			//Zeta���ݳ��ȴ洢��ַ
#define MAXLEN_ADDR_SIZE	0x01				//Zeta���ݳ���ռ�ô洢�ռ�

#define AQUATIC_MODE_ADDR 0x0801ffb8   //ˮ������ģʽѡ��
#define AQUATIC_MODE_SIZE	0x01				

#define FLAG_ADD					0x0801fd10			//���ݼĴ����Ĵ���λ�ã����ڼ�¼���ֱ�ʶλ
#define	FLAG_ADD_SIZE			0x10


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
  CLOSE_IWDG,                             //�رն������Ź�
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

