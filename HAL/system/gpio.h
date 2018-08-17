/*
**************************************************************************************************************
*	@file	gpio.c
*	@author Ysheng
*	@version 
*	@date    
*	@brief	GPIO
***************************************************************************************************************
*/
#ifndef __GPIO_H__
#define __GPIO_H__

#include "board.h"


/********************DEBUG UART******************/
#define  USART1_IO									GPIOA
#define  USART1_TX									GPIO_PIN_9
#define  USART1_RX									GPIO_PIN_10

/********************GPS UART******************/
#define  USART2_IO									GPIOA
#define  USART2_TX									GPIO_PIN_2
#define  USART2_RX									GPIO_PIN_3

/********************GPS Power******************/
#define  GPS_IO											GPIOB
#define  GPS_Power_ON     					GPIO_PIN_12

/********************485 UART******************/
#define  USART5_IO									GPIOB
#define  USART5_TX									GPIO_PIN_3
#define  USART5_RX									GPIO_PIN_4

/**********************12V��Դ���أ�485����****************************/
#define Out_12V_ON_Pin_GPIO_Port    GPIOB
#define Out_12V_ON_Pin_Pin          GPIO_PIN_5

#define	RS485PIN_0									GPIO_PIN_13
#define RS485PIN_1									GPIO_PIN_14
#define RS485PIN_2									GPIO_PIN_15


/***********************485IC��ԴIO***************************/
#define POWER_485IC_Pin             GPIO_PIN_12
#define POWER_485IC_Port            GPIOA

/**************485�շ��л�����**************/
#define Out_485_DE_Pin_Pin          GPIO_PIN_15
#define Out_485_DE_Pin_GPIO_Port    GPIOA


/***********************���IC IO����***************************/
#define OUT_CH_CE_GPIO_Port					GPIOB
#define	OUT_CH_CE_Pin								GPIO_PIN_9

#define IN_CH_STAT1_Pin							GPIO_PIN_7
#define IN_CH_STAT2_Pin							GPIO_PIN_6
#define IN_CH_PG_Pin								GPIO_PIN_8

/***********************FLASH IO����***************************/
#define FLASH_SPI_CS_PORT           GPIOA
#define FLASH_SPI_CS_PIN            GPIO_PIN_8

/*!
 * \brief GPIO��ʼ��
 * \param SX1276 RESET���ų�ʼ��---PB13
 * \retval None
 */
void SX1276GPIO_Init(void);

void SenSor_Close(void);

/**
  * @brief GPIO��ʼ��
  * @param  LoRa��Դ����  HOST_2_Lora_DFU_EN���ų�ʼ��---PB12 
  * @retval None
  */
void LoRaPower_Init(void);

/*!
 * \brief GPIO IRQ Initialization
 *
 */
void SX1276EXTI_Init(void);


/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 * \param [IN] value New GPIO output value
 */
void GpioWrite( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState );

/*!
 * \brief Toggle the value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 */
void GpioToggle( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin );

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO input value
 */
uint32_t GpioRead( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin );

#endif // __GPIO_H__
