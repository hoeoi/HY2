/*
* 创建时间：2015-09-06
* 功能：主函数
* 作者：hoeoi
*/
#include	"config.h"
#include	"Exti.h"
#include  "delay.h"
#include  "BreathingLight.h"
#include "ADC.h"
#include	"soft_uart.h"
#define T20MS (65536UL - (MAIN_Fosc / (50*12)))
void LedOff();
void InitADC();
void LED_Selet(uint16 v);
uint8 GetADCResult(uint8 ch);
void PowerOff();
void PowerWaiting();
void PowerWorkNormal();
int count(char dat);
void Timer2Config();

typedef enum 
{
	SystemStatusTypePowerOff,
	SystemStatusTypeWaiting,
	SystemStatusTypeWorkNormal
}SystemStatusType;

SystemStatusType systemStatus = SystemStatusTypePowerOff;

void	INT0Config(void)
{
	
	EX0 = 1;	//允许中断
	PX0 = 1;	//高优先级中断
	IT0 = 1;	//下降沿中断
	EA = 1;   //开总中断
}


//主函数
void main()
{
	int waitingCnt = 0;
	int j;
	//中断配置
	INT0Config();
	Timer2Config();
	InitADC();
  SIN_IN = 1;
	SIN_OUT = 1;
	while(1)
	{
		switch(systemStatus)
		{
			case SystemStatusTypePowerOff:
			{
				PowerOff();
				waitingCnt = 0;
				break;
			}
			case SystemStatusTypeWaiting:
			{
				PowerWaiting();
				
				waitingCnt ++ ;
				if(waitingCnt > 6000)
				{
					systemStatus = SystemStatusTypePowerOff;
				}
				delay_ms(10);
				break;
			}
			case SystemStatusTypeWorkNormal:
			{
				PowerWorkNormal();
				waitingCnt = 0;
				break;
			}
			default:
				break;
		}
		
	}
}


//中断INT0入口
void Ext_INT0 (void) interrupt INT0_VECTOR		//进中断时已经清除标志
{
	
	EX0 = 0;
	delay_ms(5);
	if(KEY == 0)//按键按下
	{
		KEY_LED = 0;
		if(systemStatus == SystemStatusTypePowerOff)
		{
			systemStatus = SystemStatusTypeWaiting;
		}
		else
		{
			systemStatus = SystemStatusTypePowerOff;
		}
		while(KEY == 0);
		KEY_LED = 1;
	}
	EX0 = 1;
}

void PowerOff()
{
	LedOff();
	EARPHONE = OFF;
	ADC_POWER_OFF();
	AUDIO_POWER = OFF;
	if(isBreathingLightStart())
	{
		StopBreathingLight();
	}
	PCON |= BIT(1);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}


void PowerWaiting()
{
		EARPHONE = OFF;
	  AUDIO_POWER = OFF;
		InitADC();
		if(!isBreathingLightStart())	//打开呼吸灯
		{
			StartBreathingLight();
		}
		
		if((SIN_IN==0)&&(SIN_OUT==0))//耳机和音频输入都接好
		{
			systemStatus = SystemStatusTypeWorkNormal;
		}
}


void PowerWorkNormal()
{
	  EARPHONE = ON;
		AUDIO_POWER = ON;
		if(isBreathingLightStart())//关闭呼吸灯
		{
			 StopBreathingLight();
		}
		
		if((SIN_IN==1)||(SIN_OUT==1))//耳机和音频输入拔出
		{
			systemStatus = SystemStatusTypeWaiting;
		}
}


/**关闭所有LED**/
void LedOff()
{
	GREEN = 1;
	BLUE = 1;
  RED = 1;
	LED_PWM = 0;
	KEY_LED = 1;
}

/**取ADC电压结果**/
//uint8 GetADCResult(uint8 ch)
//{
//	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ch|ADC_START;
//	_nop_();
//	_nop_();
//	_nop_();
//	_nop_();
//	while(!(ADC_CONTR&ADC_FLAG));
//	ADC_CONTR&=~ADC_FLAG;
//	return ADC_RES;
//}

uint16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	u16	adc;
	u8	i;

	if(channel > ADC_CH7)	return	1024;	//错误,返回1024,调用的程序判断	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	NOP(4);			//对ADC_CONTR操作后要4T之后才能访问

	for(i=0; i<250; i++)		//超时
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			adc = (u16)ADC_RES;
			adc = (adc << 2) | (ADC_RESL & 3);
			return	adc;
		}
	}
	return	1024;	//错误,返回1024,调用的程序判断
}
/**电压采集后灯光选择**/
void LED_Selet(uint16 v)
{
		if(v>=414)
		{
			GREEN=0;
			BLUE=1;
			RED=1;
		}
		else if(v < 393)
		{
			GREEN=1;
			BLUE=1;
			RED=0;
		}
		else
		{
			GREEN=1;
			BLUE=0;
			RED=1;
		}

}

/**ADC初始化**/
void InitADC()
{
	P1ASF |= BIT(2);  // P1.2 作为ADC
	ADC_CONTR = (ADC_CONTR & ~ADC_90T) | ADC_90T;
	ADC_CONTR |= 0x80;
  PCON2 &= ~(1<<5);	//10位AD结果的高8位放ADC_RES，低2位在ADC_RESL的低2位。
	EADC = 0;  //不允许中断
	PADC = 0;	//优先级设置	PolityLow		
	ADC_POWER_ON();
}

/**电压值运算**/
//int count(char dat)
//{
//	int velue;
//	velue=(int)(dat*33*3)/256;
//	return velue;
//}


void Timer2Config()
{
	AUXR &= ~(1<<4);	//停止计数
	IE2  |=  (1<<2);	//允许中断
	AUXR &= ~(1<<2);	//12T
	INT_CLKO &= ~0x04;	//不输出时钟
	TH2 = (u8)(T20MS >> 8);
	TL2 = (u8)T20MS;
	AUXR |=  (1<<4);	//开始运行
}

void timer2_int (void) interrupt TIMER2_VECTOR
{
	static int tickCnt = 0;
	static uint8 addCnt = 0;
	static uint16 v = 0;
	tickCnt++;
	if(tickCnt == 5)		//100ms
	{
		addCnt++;
		v += Get_ADC10bitResult(2);
		if(addCnt == 3)
		{
			LED_Selet(v/3);		//取三次平均值 所以300ms更新一次led状态
			v = 0;
			addCnt = 0;
		}
//		v = Get_ADC10bitResult(2);
//		LED_Selet(v);
		
		tickCnt = 0;
	}

}
