#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "lcddemo.h"
#include "string.h"
#include "rtc.h"
#include "adc.h"

//数据命令选择线 pb4
extern char keyFlag;
#define X_MAX_PIXEL 128
#define Y_MAX_PIXEL 128

#define CMDDATAPORT GPIOB
#define CMDDATAPIN GPIO_PIN_4

//片选线 pa15
#define SSPORT GPIOA
#define SSPIN GPIO_PIN_15

//背光 pb6
#define BACKLIGHTPORT GPIOB
#define BACKLIGHTPIN GPIO_PIN_6

//数据命令选择线输出低
#define CMDDATALOW HAL_GPIO_WritePin(CMDDATAPORT,CMDDATAPIN,GPIO_PIN_RESET)
//数据命令选择线输出高
#define CMDDATAHIGH HAL_GPIO_WritePin(CMDDATAPORT,CMDDATAPIN,GPIO_PIN_SET)

//片选线输出低
#define SSLOW HAL_GPIO_WritePin(SSPORT,SSPIN,GPIO_PIN_RESET)
//片选线输出高
#define SSHIGH HAL_GPIO_WritePin(SSPORT,SSPIN,GPIO_PIN_SET)


//发送命令
static void sendCmd(unsigned char cmd)
{
	SSLOW;  //片选低
	CMDDATALOW; // 数据命令选择线低
	HAL_SPI_Transmit(&hspi1,&cmd,1,0xffff);// spi 总线发命令cmd
	SSHIGH;	//片选高
}
static void send8Dat(unsigned char dat)
{
	SSLOW;//片选低
	CMDDATAHIGH;// 数据命令选择线高
	HAL_SPI_Transmit(&hspi1,&dat,1,0xffff);// spi 总线发8bit 数据dat
	SSHIGH;	//片选高
}
static void send16Dat(unsigned short data)
{
	unsigned char tmp;
	SSLOW;//片选低
	CMDDATAHIGH;// 数据命令选择线高
	tmp = data >> 8;
	HAL_SPI_Transmit(&hspi1,&tmp,1,0xffff);// spi 总线发高8bit 数据dat
	tmp = data;
	HAL_SPI_Transmit(&hspi1,&tmp,1,0xffff);// spi 总线发低8bit 数据dat
	SSHIGH;		//片选高
}

void lcdInit()
{
	HAL_GPIO_WritePin(BACKLIGHTPORT,BACKLIGHTPIN,GPIO_PIN_SET); //点亮液晶屏背光
	
	
	        sendCmd(0x01);//Sleep exit  发命令 0x1，  lcd 进行复位
        HAL_Delay(120);
  
	sendCmd(0x11);//Sleep exit 
	
        HAL_Delay(120);
 
	sendCmd(0x36); //MX, MY, RGB mode 
	send8Dat(0xC8); 
	
	sendCmd(0x3A); //像素是16位
	send8Dat(0x05); 
	
	sendCmd(0x29);//显示开关打开	
}
//设置显示区域的左 右  上  下 
static void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	sendCmd(0x2a); //设置显示区域 的 x    
	
	send8Dat(0); // 显示区域x 左侧的坐标
	send8Dat(x_start+2);
	
	send8Dat(0); 
	send8Dat(x_end+2);   // 显示区域x 右侧的坐标
	
	
		sendCmd(0x2b); //设置显示区域 的 y   
	
	send8Dat(0); // 显示区域y 上侧的坐标
	send8Dat(y_start+3);
	
	send8Dat(0); 
	send8Dat(y_end+3);   // 显示区域y 下侧的坐标
	
	
}

void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	//在lcd 的点 x，y 地方输出数据
	Lcd_SetRegion(x,y,x+1,y+1);//设置显示区域
	sendCmd(0x2C);//发送命令
	send16Dat(Data);//发送显示的数据给lcd 驱动ic

}


void Lcd_Clear(uint16_t Color)      // 用颜色Color 填充屏幕         
{	
   unsigned int i;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);// 设置显示 区域(整个屏幕)
	sendCmd(0x2C);//发送命令
	
	for(i = 0; i < 128*128; i++)
	{
		send16Dat(Color);
	}
	

}


void Lcd_DispBmp(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char  *p)
{	
   unsigned int data;
	int i;
   Lcd_SetRegion(x,y,x+w-1,y+h-1);
	sendCmd(0x2C);
	
	for(i = 0; i < w*h*2; i+=2)
	{
		data = (p[i+1] << 8) | p[i];
		send16Dat(data);
	}
	
}

//在坐标x，y 处 显示字符 chr，  背景色（填充颜色）bc， 前景色（字体颜色）fc
//void  dispAscii(uint16_t x, uint16_t y, char  chr, uint16_t bc, uint16_t fc)
//{
//	int i;
//	int pos = 0;
//	unsigned char byte;
//	pos = chr;
//	pos = pos * 16;
//	Lcd_SetRegion(x,y,x+8,y+16);
//	sendCmd(0x2C);
//	for(i = 0;i<16; i++)
//	{
//		byte =  Zk_ASCII8X16[pos + i];
//		if(byte == 0)
//		{
//			send16Dat(chr);
//		}
//		else if(byte == 1)
//		{
//			send16Dat(fc);
//		}
//	}

//}

void  dispAscii(uint16_t x, uint16_t y, char chr, uint16_t bc, uint16_t fc)
{
	int i;
	int pos = 0;
	unsigned char byte;
	
	pos = (int)chr * 16;
	Lcd_SetRegion(x,y,x+7,y+15);
	sendCmd(0x2C);
	for(i = 0;i<16; i++)
	{
		byte =  Zk_ASCII8X16[pos + i];
		for (int j = 0; j < 8; j++) 
		{
            // 如果位为1，则绘制前景色，否则绘制背景色
           
            
            if (byte & (1 << (7 - j))) { // 位操作需要从最高位开始
                send16Dat(fc);
            } else {
               send16Dat(bc);
            }
        
		
	}
	}
}

//字符串
void lcdDispStr(uint16_t x, uint16_t y,char *str,uint16_t bc, uint16_t fc)
{
	int len=strlen(str);
	int i;
	for(i = 0; i < len; i++)
	{
		if((x+8*i) > 127)  return;
		dispAscii(x+8*i,y,str[i],bc,fc);
	}
}



void lcdyemian()
{
	
	RTC_TimeTypeDef ctime;
	RTC_DateTypeDef cdate;
	char time_date[2][17]={0};
	HAL_RTC_GetTime(&hrtc,&ctime,RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc,&cdate,RTC_FORMAT_BCD);
	sprintf(time_date[0],"%02x-%02x-%02x",cdate.Year,cdate.Month,cdate.Date);
	sprintf(time_date[1],"%02x-%02x-%02x",ctime.Hours,ctime.Minutes,ctime.Seconds);
//	lcdInit();
//	Lcd_Clear(0xffff);
	lcdDispStr(30,30,time_date[0],0xffff,0);
	lcdDispStr(30,80,time_date[1],0xffff,0);
	
	
}

int tempValue = 50;
int flameValue = 200;
int smokeValue = 100;
int currentLine = 0; // 当前光标所在行
char arr[4][17] = {
	"    setting   ",
	"temp th:  xxxx ",
	"flame th: xxxx ",
	"smoke th: xxxx ",
	};
void lcdcaidan()
{
//	char arr[4][17] = {
//	"    setting   ",
//	"temp th:  xxxx ",
//	"flame th: xxxx ",
//	"smoke th: xxxx ",
//	};
	sprintf(arr[1]+10, "%04d", tempValue);
	sprintf(arr[2]+10, "%04d", flameValue);
	sprintf(arr[3]+10, "%04d", flameValue);
//	lcdInit();
	Lcd_Clear(0xffff);
	lcdDispStr(5,30,arr[0],0xffff,0);
	lcdDispStr(5,50,arr[1],0xffff,0);
	lcdDispStr(5,70,arr[2],0xffff,0);
	lcdDispStr(5,90,arr[3],0xffff,0);
}


int getKey(unsigned int val)
{
	if(val < 1000)
		return KEY_DOWN;
	else if(val < 1800)
		return KEY_LEFT;
	if(val < 2300)
		return KEY_UP;
	if(val < 2700)
		return KEY_PUSH;
	else
		return KEY_RIGHT;
	

}
extern int keycode;
extern unsigned int adcValue;

void menu()
{
	lcdcaidan();
    int currentLine = 1; 
	lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
    while(1)
    {
				
        //if(key==0)
        if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_SET)
        {
            HAL_Delay(10);
            if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8) == GPIO_PIN_SET)
            {
							
												//HAL_ADCEx_Calibration_Start(&hadc1);
                        HAL_ADC_Start(&hadc1); 
                        HAL_ADC_PollForConversion(&hadc1, 0xffff); 
                        adcValue = HAL_ADC_GetValue(&hadc1); 
                        HAL_ADC_Stop(&hadc1);
                        //printf("adcValue = %d\r\n", adcValue);
                        keycode = getKey(adcValue);
										
                        switch(keycode)
                        {    
                        case KEY_UP:
                                currentLine--;
                                if(currentLine<1)
                                {
                                    currentLine++;
                                }
                                lcdcaidan();
                                lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
																
                                break;
                        case KEY_DOWN:
                                currentLine++;
                                if(currentLine>3)
                                {
                                    currentLine--;
                                }
                               lcdcaidan();
                                lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                
																break;
                        case KEY_LEFT :
                                if(currentLine==1)
                                {
                                    tempValue=tempValue-10;
                                    sprintf(arr[1]+10, "%04d", tempValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
                                if(currentLine==2)
                                {
                                    flameValue=flameValue-10;
                                    sprintf(arr[2]+10, "%04d", flameValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
                                if(currentLine==3)
                                {
                                    smokeValue=smokeValue-10;
                                    sprintf(arr[3]+10, "%04d", smokeValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
																
                                break;
                        case KEY_RIGHT:
                                if(currentLine==1)
                                {
                                    tempValue=tempValue+10;
                                    sprintf(arr[1]+10, "%04d", tempValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
                                if(currentLine==2)
                                {
                                    flameValue=flameValue+10;
                                    sprintf(arr[2]+10, "%04d", flameValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
                                if(currentLine==3)
                                {
                                    smokeValue=smokeValue+10;
                                    sprintf(arr[3]+10, "%04d", smokeValue);
                                    lcdDispStr(5,30+currentLine*20,arr[currentLine],RED,00000);
                                }
																
                                break;
                        case KEY_PUSH:
								Lcd_Clear(0xffff);
								break;       
					}
				if(KEY_PUSH==keycode)	
				{
					return ;
				}									
              }
			
        }
    }
}
