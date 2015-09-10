/*
* ����ʱ�䣺2015-09-06
* ���ܣ�������
* ���ߣ�hoeoi
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
	
	EX0 = 1;	//�����ж�
	PX0 = 1;	//�����ȼ��ж�
	IT0 = 1;	//�½����ж�
	EA = 1;   //�����ж�
}


//������
void main()
{
	int waitingCnt = 0;
	int j;
	//�ж�����
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


//�ж�INT0���
void Ext_INT0 (void) interrupt INT0_VECTOR		//���ж�ʱ�Ѿ������־
{
	
	EX0 = 0;
	delay_ms(5);
	if(KEY == 0)//��������
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
		if(!isBreathingLightStart())	//�򿪺�����
		{
			StartBreathingLight();
		}
		
		if((SIN_IN==0)&&(SIN_OUT==0))//��������Ƶ���붼�Ӻ�
		{
			systemStatus = SystemStatusTypeWorkNormal;
		}
}


void PowerWorkNormal()
{
	  EARPHONE = ON;
		AUDIO_POWER = ON;
		if(isBreathingLightStart())//�رպ�����
		{
			 StopBreathingLight();
		}
		
		if((SIN_IN==1)||(SIN_OUT==1))//��������Ƶ����γ�
		{
			systemStatus = SystemStatusTypeWaiting;
		}
}


/**�ر�����LED**/
void LedOff()
{
	GREEN = 1;
	BLUE = 1;
  RED = 1;
	LED_PWM = 0;
	KEY_LED = 1;
}

/**ȡADC��ѹ���**/
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

	if(channel > ADC_CH7)	return	1024;	//����,����1024,���õĳ����ж�	
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	NOP(4);			//��ADC_CONTR������Ҫ4T֮����ܷ���

	for(i=0; i<250; i++)		//��ʱ
	{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			adc = (u16)ADC_RES;
			adc = (adc << 2) | (ADC_RESL & 3);
			return	adc;
		}
	}
	return	1024;	//����,����1024,���õĳ����ж�
}
/**��ѹ�ɼ���ƹ�ѡ��**/
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

/**ADC��ʼ��**/
void InitADC()
{
	P1ASF |= BIT(2);  // P1.2 ��ΪADC
	ADC_CONTR = (ADC_CONTR & ~ADC_90T) | ADC_90T;
	ADC_CONTR |= 0x80;
  PCON2 &= ~(1<<5);	//10λAD����ĸ�8λ��ADC_RES����2λ��ADC_RESL�ĵ�2λ��
	EADC = 0;  //�������ж�
	PADC = 0;	//���ȼ�����	PolityLow		
	ADC_POWER_ON();
}

/**��ѹֵ����**/
//int count(char dat)
//{
//	int velue;
//	velue=(int)(dat*33*3)/256;
//	return velue;
//}


void Timer2Config()
{
	AUXR &= ~(1<<4);	//ֹͣ����
	IE2  |=  (1<<2);	//�����ж�
	AUXR &= ~(1<<2);	//12T
	INT_CLKO &= ~0x04;	//�����ʱ��
	TH2 = (u8)(T20MS >> 8);
	TL2 = (u8)T20MS;
	AUXR |=  (1<<4);	//��ʼ����
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
			LED_Selet(v/3);		//ȡ����ƽ��ֵ ����300ms����һ��led״̬
			v = 0;
			addCnt = 0;
		}
//		v = Get_ADC10bitResult(2);
//		LED_Selet(v);
		
		tickCnt = 0;
	}

}
