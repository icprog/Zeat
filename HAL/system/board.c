/*
**************************************************************************************************************
*	@file		 ard.c
*	@author  Jason 
*	@version V0.1
*	@date    2018/07/09
*	@brief	 底层IO初始化
* @conn		 Jason_531@163.com
***************************************************************************************************************
*/
#include "board.h"

/*!
 * Flag to indicate if the MCU is Initialized
 */
bool McuInitialized = false;

void BoardInitMcu( void )
{
	if( McuInitialized == false )
	{
		HAL_Init(  );
	
    /***************时钟初始化********************/
		SystemClockConfig(  );
        
		/* Enable Power Control clock */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE(); ///开启时钟
		
		RTC_Init(  );
												
		McuInitialized = true;
		
	} 
	else
	{
		SystemClockReConfig(  );
		
		/* Enable Power Control clock */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE(); ///开启时钟							
	}
		
	/***************串口初始化********************/
	MX_USART1_UART_Init(  );  
							
	/****************ADC初始化*******************/
	MX_ADC_Init(  );
	
	SPI1_Init(  );
	
	/*****************电源管理********************/
	BatteryInit(  );
		
	/*******************开启RTC中断*******************/
	HAL_NVIC_SetPriority(RTC_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(RTC_IRQn);
	
	LedInit(  );
	
	SpiFlashInit(  );
	
//	MX_WWDG_Init(  );
//	WWDG_NVIC_Init(  );
					
	SensorsInit(  );
	
	ZetaHandle.Init(  );
	
	ZetaHandle.PowerOn(  );
}

/*
 *	BoardDeInitMcu:	进入低功耗模式：停机，需要设置相应IO模式
 *	返回值: 				无
 */
void BoardDeInitMcu( void )
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Enable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
	SpiFlashPowerDown( );
	
	/****************************************/
    /* Disable the Peripheral */	
	HAL_ADC_MspDeInit(&hadc);  ///OK
	hadc.State = HAL_ADC_STATE_RESET;

	HAL_UART_DeInit(&hlpuart1);
	hlpuart1.gState = HAL_UART_STATE_RESET;
	
	///关闭UART1时钟
  HAL_UART_DeInit(&huart1);
	huart1.gState = HAL_UART_STATE_RESET;
	
	///关闭UART2时钟
	HAL_UART_DeInit(&huart2);
	huart2.gState = HAL_UART_STATE_RESET;
	
	///关闭UART5时钟
	HAL_UART_DeInit(&huart5);
	huart5.gState = HAL_UART_STATE_RESET;
	
	HAL_TIM_Base_MspDeInit(&htim2);
	htim2.State = HAL_TIM_STATE_RESET;
	
	///关闭SPI时钟：
	HAL_SPI_DeInit(&hspi1);
	hspi1.State = HAL_SPI_STATE_RESET;
	
	/*******************关闭SPI*********************/
		
	GPIO_InitStructure.Pin = GPIO_PIN_All;   ///GPIO_PIN_ll 0xF7FF
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG; ///low_power,其它较高
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.Pin = GPIO_PIN_All;   ///GPIO_PIN_All
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG; ///low_power,其它较高
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
					
  GPIO_InitStructure.Pin = GPIO_PIN_All;  /// PB9：CH_CE(ok) 0xDFFF
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_LOW;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
                
  HAL_SuspendTick();
	
	/* Disable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
    
}

void SystemClockReConfig( void )
{
    __HAL_RCC_PWR_CLK_ENABLE( );
    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );

    /* Enable HSE */
    __HAL_RCC_HSE_CONFIG( RCC_HSE_ON );

    /* Wait till HSE is ready */
    while( __HAL_RCC_GET_FLAG( RCC_FLAG_HSERDY ) == RESET )
    {
    }

    /* Enable PLL */
    __HAL_RCC_PLL_ENABLE( );

    /* Wait till PLL is ready */
    while( __HAL_RCC_GET_FLAG( RCC_FLAG_PLLRDY ) == RESET )
    {
    }

    /* Select PLL as system clock source */
    __HAL_RCC_SYSCLK_CONFIG ( RCC_SYSCLKSOURCE_PLLCLK );

    /* Wait till PLL is used as system clock source */
    while( __HAL_RCC_GET_SYSCLK_SOURCE( ) != RCC_SYSCLKSOURCE_STATUS_PLLCLK )
    {
    }
    /* Resume Tick interrupt if disabled prior to sleep mode entry*/
    HAL_ResumeTick();
}

uint8_t BoardGetBatteryLevel( void )
{
    uint8_t batteryLevel = 0;

    batteryLevel =  100;
    return (batteryLevel);
}

/**
  * @brief 系统时钟初始化
  * @param 外部时钟16MHZ
  * @retval None
  */
void SystemClockConfig( void )
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */	
  /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
	
    /**Configure the Systick interrupt time 1ms
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
