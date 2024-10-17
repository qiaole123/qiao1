#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "stdio.h"
#include "string.h"


//单总线 是 pb8 
#define DATAPORT GPIOB
#define  DATAPIN   GPIO_PIN_8 

void setOutput()
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pins : PB0 PB1 PB2 PB8 */
  GPIO_InitStruct.Pin = DATAPIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);

}

void setInput()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = DATAPIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);	
	
}

void delay_uss(char numbr)
{
	htim16.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim16);
	while(htim16.Instance->CNT < numbr) ;
	HAL_TIM_Base_Stop(&htim16);
	return;
}

int shakehand()
{
	setOutput();
	HAL_GPIO_WritePin(DATAPORT,DATAPIN,GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(DATAPORT,DATAPIN,GPIO_PIN_SET);
	delay_uss(30);
	
	setInput();
	while(HAL_GPIO_ReadPin(DATAPORT,DATAPIN) == 1);// 等单总线  变低电平
	while(HAL_GPIO_ReadPin(DATAPORT,DATAPIN) == 0);// 等单总线  变高电平
	
	return 1;
}

char readByte()
{
	char  data=0;
	int i=0;
	
	// 10 01 0011 
	//010010011

	for(i = 0; i < 8; i++)
	{
		while(HAL_GPIO_ReadPin(DATAPORT,DATAPIN) == 1);// 等单总线  变低电平
		while(HAL_GPIO_ReadPin(DATAPORT,DATAPIN) == 0) {;}   
			// 等单总线  变高电平
			delay_uss(40);// 对二进制的0 来说， 40us后，总线上的电平是低   对1 来说， 总线上的电平是高

		data = data << 1;		
		if(HAL_GPIO_ReadPin(DATAPORT,DATAPIN) == GPIO_PIN_SET)
		{
			data |= 1;
		}
	}
	return data;
}

void dht11get()
{
	char h0,h1,t0,t1,crccode;
	unsigned char tmp;
	shakehand();
	h0 = readByte();
	h1 = readByte();
	t0 = readByte();
	t1 = readByte();
	crccode = readByte();
	tmp = h0+h1+t0+t1;
	if(tmp == crccode)
	{
		if(h0 > 60)
		{
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2| GPIO_PIN_1);
		}
		printf("ok, humi= %d.%d,   temp=%d.%d\r\n",h0,h1,t0,t1);
	}
	
}

