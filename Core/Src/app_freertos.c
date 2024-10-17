/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcddemo.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern char keyFlag;
unsigned int adcValue = 0;
char ledchange = 0;
int fputc(int c, FILE *f)
{
	unsigned char tmp = c;
	HAL_UART_Transmit(&huart1,&tmp,1,0xffff);
	
	return c;
}
//void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)// 外中断8 的回调函数
//{
//	if(GPIO_Pin == GPIO_PIN_8) //判断是否是 8 号外中断
//	{
//		keyFlag = 1;
//	}
//}
//extern char keyFlag;

#define KEY_UP 100
#define KEY_DOWN 101
#define KEY_LEFT 102
#define KEY_RIGHT 103
#define KEY_PUSH 104

int keycode= 0;


//int getKey(unsigned int val)
//{
//	if(val < 1000)
//		return KEY_DOWN;
//	else if(val < 1800)
//		return KEY_LEFT;
//	if(val < 2300)
//		return KEY_UP;
//	if(val < 2700)
//		return KEY_PUSH;
//	else
//		return KEY_RIGHT;
//	

//}


unsigned int dmaBuf[] = {0}; //保存通道0  通道4 的转换结果
volatile char adcDmaEndFlag = 0; // 1------ adc的dma方式结束了
void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef *hadc)
{
	adcDmaEndFlag = 1;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osTimerId myTimer01Handle;
osSemaphoreId myBinarySem01Handle;
osSemaphoreId myBinarySem02Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* definition and creation of myBinarySem02 */
  osSemaphoreDef(myBinarySem02);
  myBinarySem02Handle = osSemaphoreCreate(osSemaphore(myBinarySem02), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityRealtime, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
   //osTimerStart(myTimer01Handle,1000);
	for(;;)
  {
	  //lcdyemian();
	  
	  osSemaphoreWait(myBinarySem01Handle,0xffff);
//	  lcdyemian();
//	  osDelay(1000);
//	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_SET)
//	  {
//		osDelay(10);
//		  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_SET)
//		  {
//			  
//			          HAL_ADC_Start(&hadc1); 
//                        HAL_ADC_PollForConversion(&hadc1, 0xffff); 
//                        adcValue = HAL_ADC_GetValue(&hadc1); 
//                        HAL_ADC_Stop(&hadc1);
//			  adcDmaEndFlag = 0;
//			HAL_ADC_Start_DMA(&hadc1,dmaBuf,2);
//				while(adcDmaEndFlag == 0)   ; // 直至adcDmaEndFlag 为1 
//				
//			//dmaBuf[0] 就是五向键的adc
//				adcValue = dmaBuf[0];
//			HAL_ADC_Stop_DMA(&hadc1); //
//			  while(1)
//			  {
//				  HAL_ADC_Start(&hadc1); 
//                        HAL_ADC_PollForConversion(&hadc1, 0xffff); 
//                        adcValue = HAL_ADC_GetValue(&hadc1); 
//                        HAL_ADC_Stop(&hadc1);
//				  lcdcaidan();
//				  osDelay(10000);
//				  adcDmaEndFlag = 0;
//			HAL_ADC_Start_DMA(&hadc1,dmaBuf,2);
//				while(adcDmaEndFlag == 0)   ; // 直至adcDmaEndFlag 为1 
//				
//			//dmaBuf[0] 就是五向键的adc
//				adcValue = dmaBuf[0];
//			HAL_ADC_Stop_DMA(&hadc1); //
//				keycode = getKey(adcValue); //根据每个键的adc的不同， 区分是哪个键按下
//				if(KEY_PUSH == keycode)
//				{
//					lcdcaidan();
//				}
//				else if(KEY_UP == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_RESET);
//				}
//				else if(KEY_DOWN == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_SET);
//				}
//				else if(KEY_LEFT == keycode)
//				{
//					ledchange = 1;
//				}
//				else if(KEY_RIGHT == keycode)
//				{
//					ledchange = 0;
//				}
//				else if(KEY_PUSH == keycode)
//				{
//					break;
//				}
//			  }
//			adcDmaEndFlag = 0;
//			HAL_ADC_Start_DMA(&hadc1,dmaBuf,2);
//				while(adcDmaEndFlag == 0)   ; // 直至adcDmaEndFlag 为1 
//				
//			//dmaBuf[0] 就是五向键的adc
//				adcValue = dmaBuf[0];
//			HAL_ADC_Stop_DMA(&hadc1); //				
//				
//			keycode = getKey(adcValue); //根据每个键的adc的不同， 区分是哪个键按下
//				if(KEY_PUSH == keycode)
//				{
//					lcdcaidan();
//				}
//				else if(KEY_UP == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_RESET);
//				}
//				else if(KEY_DOWN == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_SET);
//				}
//				else if(KEY_LEFT == keycode)
//				{
//					ledchange = 1;
//				}
//				else if(KEY_RIGHT == keycode)
//				{
//					ledchange = 0;
//				}
//			
//		  }
//	  }
	while(1)
	{
		lcdyemian();
	  osDelay(1000);
		if(keyFlag == 1)
		{
			HAL_Delay(10); //  延时消抖
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_SET)
			{
				HAL_ADCEx_Calibration_Start(&hadc1);
                            HAL_ADC_Start(&hadc1); 
              HAL_ADC_PollForConversion(&hadc1, 0xffff); 
              adcValue = HAL_ADC_GetValue(&hadc1); 
              HAL_ADC_Stop(&hadc1);
                   keycode = getKey(adcValue);
//				adcDmaEndFlag = 0;
//			HAL_ADC_Start_DMA(&hadc1,dmaBuf,1);
//				while(adcDmaEndFlag == 0)   ; // 直至adcDmaEndFlag 为1 
//				
//			//dmaBuf[0] 就是五向键的adc
//				adcValue = dmaBuf[0];
//			HAL_ADC_Stop_DMA(&hadc1); //				
//				
//			keycode = getKey(adcValue); //根据每个键的adc的不同， 区分是哪个键按下
				if(KEY_PUSH == keycode)
				{
					menu();
					lcdyemian();
				}
				else if(KEY_UP == keycode)
				{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_RESET);
				}
				else if(KEY_DOWN == keycode)
				{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_SET);
				}
				else if(KEY_LEFT == keycode)
				{
					ledchange = 1;
				}
				else if(KEY_RIGHT == keycode)
				{
					ledchange = 0;
				}
			}
//			
//			keyFlag = 0;
		}
	}
	osSemaphoreRelease(myBinarySem01Handle);	
    osDelay(1);
 }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		//osSemaphoreWait(myBinarySem01Handle,0xffff);
//		adcDmaEndFlag = 0;
//		HAL_ADC_Start_DMA(&hadc1,dmaBuf,2);
//		while(adcDmaEndFlag == 0) ; // 直至adcDmaEndFlag 为1 
//				
//			//dmaBuf[0] 就是五向键的adc
//		adcValue = dmaBuf[0];
//		HAL_ADC_Stop_DMA(&hadc1); //
//	   HAL_ADC_Start(&hadc1); 
//       HAL_ADC_PollForConversion(&hadc1, 0xffff); 
//        adcValue = HAL_ADC_GetValue(&hadc1); 
//                        HAL_ADC_Stop(&hadc1);
//	  keycode = getKey(adcValue); //根据每个键的adc的不同， 区分是哪个键按下
//				if(KEY_PUSH == keycode)
//				{
//					lcdcaidan();
//				}
//				else if(KEY_UP == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_RESET);
//				}
//				else if(KEY_DOWN == keycode)
//				{
//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2,GPIO_PIN_SET);
//				}
//				else if(KEY_LEFT == keycode)
//				{
//					ledchange = 1;
//				}
//				else if(KEY_RIGHT == keycode)
//				{
//					ledchange = 0;
//				}
	  if(ledchange == 1)
	{
		for(int i; i<100 ;i++)
		{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,i*2);
			osDelay(30);
		}
		
		for(int i = 100;i>0;i--)
		{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,i*2);
			osDelay(30);
		}
	  
	  }
//	if(ledchange == 0)
//	{
//		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
//	}
	//osSemaphoreRelease(myBinarySem01Handle);
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
	  
	dht11get();
	  
	osDelay(1000);
	  osDelay(1);
    
  }
  /* USER CODE END StartTask03 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */
	//printf("123\r\n");
  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

