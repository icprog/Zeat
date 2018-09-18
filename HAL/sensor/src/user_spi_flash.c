/**
  ******************************************************************************
  * �ļ�����:
  * ��    ��: ũ��Ƕ��ʽ�����Ŷ�(LWY)
  * ��    ��: V1.0
  * ��д����:
  * ��    ��:
  ******************************************************************************
  * ��飺
  *
  *
  ******************************************************************************
  * �汾�޸ļ�¼��
  * v1.0    (1)
  *
  * v1.1    (1)
  *         (2)
  *
  * v1.2    (1)
  *
  * ��Ȩ��ũ��Ƕ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "user_spi_flash.h"
#include "spi.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/*��ֵ����*/
#define WIP_Flag                      0x01    /* Write In Progress (WIP) flag */
#define Dummy_Byte                    0xFF    //������������

/*Flash����ָ��*/
#define MX25_WriteEnable		    0x06    //дʹ��
#define MX25_WriteDisable		    0x04    //дʧ��
#define MX25_WriteStatusReg		    0x01    //д״̬�Ĵ���
#define MX25_ReadStatusReg		    0x05    //��״̬�Ĵ���
#define MX25_ReadData               0x03    //������
#define MX25_FastReadData           0x0B    //���ٶ�����
#define MX25_ReadSFDP               0x5A
#define MX25_DeviceID               0x9F    //��ȡ�豸IDָ��      //RDID      //0xc22013
#define MX25_ElectronicID           0xAB    //���豸����ID        //RES       //0x12
#define MX25_ManufactDeviceID       0x90    //�������̺��豸ID     //REMS      //0xc212����0x12c2
#define MX25_FastReadDual           0x3B    //˫�ض�����
#define MX25_SectorErase            0x20    //ҳ����
#define MX25_BlockErase             0xD8    //�����
//#define MX25_BlockErase             0x52    //�����
#define MX25_ChipErase              0xC7    //оƬ����
//#define MX25_ChipErase              0x60    //оƬ����
#define MX25_PageProgram            0x02    //��¼ѡ���˵�ҳ
#define MX25_PowerDown              0xB9    //�����������
#define MX25_ReleasePowerDown	    0xAB    //�˳��������



/* ˽�б��� ------------------------------------------------------------------*/
/* ˽�к��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/

/**
  * ��������: ��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SpiFlashInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE(  );
	
	GPIO_InitStruct.Pin = OUT_SPI_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OUT_SPI_CE_GPIO_Port, &GPIO_InitStruct);

	SPI1_Init(  );
	
	SpiFlashPowerDown(  );	 
}
/**
  * ��������: ��������
  * �������: SectorAddr��������������ַ��Ҫ��Ϊ4096����
  * �� �� ֵ: ��
  * ˵    ��������Flash��С�������СΪ4KB(4096�ֽ�)����һ��������С��Ҫ���������
  *           Ϊ4096��������������FlashоƬд������֮ǰҪ���Ȳ����ռ䡣
  */
void SpiFlashSectorErase(uint32_t SectorAddr)
{		
	/* ����FLASHдʹ������ */
  SpiFlashWriteEnable();
		
	/* �ȴ��������*/
  SpiFlashWaitForNoBusy();
	
  /* �������� */
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* ������������ָ��*/
  SpiFlashSendByte(MX25_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  SpiFlashSendByte((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  SpiFlashSendByte((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  SpiFlashSendByte(SectorAddr & 0xFF);
  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
	
	/* �ȴ��������*/
  SpiFlashWaitForNoBusy();
}

/**
  * ��������: ������Ƭ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������������Flash��Ƭ�ռ�
  */
void SpiFlashBulkErase(void)
{	
 /* ����FLASHдʹ������ */
  SpiFlashWriteEnable();

	/* �ȴ��������*/
  SpiFlashWaitForNoBusy();
	
  /* ��Ƭ���� Erase */
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* ������Ƭ����ָ��*/
  SpiFlashSendByte(MX25_ChipErase);
  /* ���ô���FLASH: CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
	
	/* �ȴ��������*/
  SpiFlashWaitForNoBusy();

}

/**
  * ��������: ������FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PERWRITEPAGESIZE
  * �� �� ֵ: ��
  * ˵    ��������Flashÿҳ��СΪ256���ֽ�
  */
void SpiFlashPageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{	
  /* ����FLASHдʹ������ */
  SpiFlashWriteEnable();
	
	/* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();

   /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();
  /* д��дָ��*/
  SpiFlashSendByte(MX25_PageProgram);
  /*����д��ַ�ĸ�λ*/
  SpiFlashSendByte((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  SpiFlashSendByte((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  SpiFlashSendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PERWRITEPAGESIZE)
  {
     NumByteToWrite = SPI_FLASH_PERWRITEPAGESIZE;
     //printf("Err: SpiFlashPageWrite too large!\n");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
     /* ���͵�ǰҪд����ֽ����� */
    SpiFlashSendByte(*pBuffer);
     /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
		  
	/* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();
}

/**
  * ��������: ������FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void SpiFlashBufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PAGESIZE;
  count = SPI_FLASH_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PAGESIZE;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PAGESIZE;
  
//  printf("Addr=%d,count=%d,NumOfPage=%d,NumOfSingle=%d\r\n",Addr,count,NumOfPage,NumOfSingle);

  if (Addr == 0) /* ����ַ�� SPI_FLASH_PAGESIZE ����  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGESIZE */
    {
      SpiFlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        SpiFlashPageWrite(pBuffer, WriteAddr, SPI_FLASH_PAGESIZE);
        WriteAddr +=  SPI_FLASH_PAGESIZE;
        pBuffer += SPI_FLASH_PAGESIZE;
      }
      SpiFlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* ����ַ�� SPI_FLASH_PAGESIZE ������ */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;
        SpiFlashPageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
		
        SpiFlashPageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SpiFlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PAGESIZE;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PAGESIZE;

      SpiFlashPageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SpiFlashPageWrite(pBuffer, WriteAddr, SPI_FLASH_PAGESIZE);
        WriteAddr +=  SPI_FLASH_PAGESIZE;
        pBuffer += SPI_FLASH_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        SpiFlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

uint8_t W25QXX_BUFFER[4096];	

/**
  * ��������: ������FLASHд�����ݣ��Դ���������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void SpiFlashEraseWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
  
	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SpiFlashBufferRead(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SpiFlashSectorErase(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			SpiFlashNoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else SpiFlashNoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		  pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		  NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}

/**
  * ��������: ������FLASHд������У��
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void SpiFlashNoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SpiFlashPageWrite(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}  
}

/**
  * ��������: �Ӵ���Flash��ȡ����
  * �������: pBuffer����Ŷ�ȡ�����ݵ�ָ��
  *           ReadAddr����ȡ����Ŀ���ַ
  *           NumByteToRead����ȡ���ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú����������������ȡ���ݳ���
  */
void SpiFlashBufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{	
	/* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();
	
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* ���� �� ָ�� */
  SpiFlashSendByte(MX25_ReadData);

  /* ���� �� ��ַ��λ */
  SpiFlashSendByte((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  SpiFlashSendByte((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  SpiFlashSendByte(ReadAddr & 0xFF);

	HAL_SPI_Receive(&hspi1, pBuffer, NumByteToRead, 0xFFFFFFFF);

  /* ���ô���FLASH: CS �ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
		  
	/* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();
}

/**
  * ��������: ��ȡ����Flash�豸ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���豸ID
  * ˵    ����
  */
uint32_t SpiFlashReadDeviceID(void)
{
  uint8_t Temp = MX25_DeviceID;
	uint8_t pData[3] = {0};
	uint32_t ID = 0;

	/*************SPIæ�ȴ�*****************/
	SpiFlashWaitForNoBusy(  );
	
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* ���������ȡоƬ�豸ID * */	
	HAL_SPI_Transmit(&hspi1, &Temp, 1, 0xFFFF);	
	HAL_SPI_Receive(&hspi1, &pData[0], 3, 0xFFFF);
	
	ID |= (pData[0] << 16);
	ID |= (pData[1] << 8);
	ID |= (pData[2] << 0);	
	
  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();

  printf("ID=0x%04x\r\n", ID);
  return ID;
}

/**
  * ��������: ����������ȡ���ݴ�
  * �������: ReadAddr����ȡ��ַ
  * �� �� ֵ: ��
  * ˵    ����Initiates a read data byte (READ) sequence from the Flash.
  *           This is done by driving the /CS line low to select the device,
  *           then the READ instruction is transmitted followed by 3 bytes
  *           address. This function exit and keep the /CS line low, so the
  *           Flash still being selected. With this technique the whole
  *           content of the Flash is read with a single READ instruction.
  */
void SpiFlashStartReadSequence(uint32_t ReadAddr)
{
  /* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();
	
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read from Memory " instruction */
  SpiFlashSendByte(MX25_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SpiFlashSendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SpiFlashSendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SpiFlashSendByte(ReadAddr & 0xFF);
			  
	/* �ȴ�д�����*/
  SpiFlashWaitForNoBusy();
}

/**
  * ��������: �Ӵ���Flash��ȡһ���ֽ�����
  * �������: ��
  * �� �� ֵ: uint8_t����ȡ��������
  * ˵    ����This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
uint8_t SpiFlashReadByte(void)
{
  uint8_t d_read,d_send=Dummy_Byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read;    
}

void SpiFlashSendByte(uint8_t byte)
{
	uint8_t d_send=byte;
	if(HAL_SPI_Transmit(&hspi1, &d_send, 1, 0xFFFFFF)!=HAL_OK)
	{
		printf("write flash error\r\n");
	}
}

/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
uint8_t SpiFlashSendBytes(uint8_t byte)
{
  uint8_t d_read[3],d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,d_read,3,0xFFFFFF)!=HAL_OK)
  {
    printf("read spi fail\r\n");
  }
  
  printf("DerveID: ");
  for(uint8_t i = 0; i < 3; i++)
  {
    printf("0x%02x ",d_read[i]);
  }
  printf("\r\n");
  
  return *d_read; 
}

/**
  * ��������: ʹ�ܴ���Flashд����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SpiFlashWriteEnable(void)
{
  /* ѡ����FLASH: CS�͵�ƽ */
  FLASH_SPI_CS_ENABLE();

  /* �������дʹ�� */
  SpiFlashSendByte(MX25_WriteEnable);

  /* ���ô���Flash��CS�ߵ�ƽ */
  FLASH_SPI_CS_DISABLE();
}

void SpiFlashWaitForNoBusy(void)
{
  uint8_t d_send = MX25_ReadStatusReg;
	uint16_t Getdata = 0;

	FLASH_SPI_CS_ENABLE();
	
	HAL_SPI_Transmit(&hspi1, &d_send, 1, 0xFFFF);	
	
  /* Send "Read Status Register" instruction */
	do
	{
		uint8_t d_read[2] = {0};
		uint16_t Getdata = 0;

		/* Select the FLASH: Chip Select low */				
		HAL_SPI_Receive(&hspi1, &d_read[0], 1, 0xFFFF);
		
		HAL_Delay(30);
		
		/* Deselect the FLASH: Chip Select high */		
		Getdata |= d_read[0];
//		Getdata |= d_read[1];
//		for(uint8_t i = 0; i < 2; ++i)	
//		DEBUG("data[%d] = %02x Getdata = %d\r\n",0, d_read[0],Getdata);
	}while (Getdata != RESET); /* Write in progress */
	
	FLASH_SPI_CS_DISABLE();
}

/**
  * ��������: �������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SpiFlashPowerDown(void)   
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SpiFlashSendByte(MX25_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}   

/**
  * ��������: ���Ѵ���Flash
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SpiFlashWAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SpiFlashSendByte(MX25_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE(); 
}  

/************************ (C) COPYRIGHT ũ��Ƕ��ʽ�����Ŷ� *****END OF FILE****/
