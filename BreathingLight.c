/*
* �ļ����ܣ�������
* ����ʱ�䣺2015-09-06
* ���ߣ�hoeoi
*/
#include "config.h"
#include "BreathingLight.h"



static void Timer0Config();
static char s_speed = 1;
static char isIncrease = 0;
static unsigned char pwmValue = 0x7F;
static char isStart = NO;
static void PWM_ON_OFF(bit byte);


//�ڲ���������
void BreathingLightTimer();
void Timer0Config();

void SetSpeed(char speed)
{
	s_speed = speed;
}
char isBreathingLightStart()
{
	return isStart;
}

void StartBreathingLight()
{
	pwmValue = 0x7F;
	isIncrease = 0;
	PWM_ON_OFF(YES);
	isStart = YES;
	Timer0Config();
}

void Timer0Config()
{
	ET0 = 1;	//�����ж�
	PT0 = 0;	//�����ȼ��ж�
	TMOD = (TMOD & ~0x03) ;	//����ģʽ: 16λ�Զ���װ
	AUXR |=  0x80;	//1T
	INT_CLKO &= ~0x01;	//�����ʱ��
	
	TH0 = (u8)(T1MS >> 8);
	TL0 = (u8)T1MS;
	TR0 = 1;
}

void StopBreathingLight()
{
	PWM_ON_OFF(NO);
	isStart = NO;
	TR0 = 0;
	LED_PWM = 1;
}

static void PWM_ON_OFF(bit byte)
{
	if(byte==1)
	{
		CMOD|=0x08;
		CR=1;
		CCAPM1|=0x02;
	}else{
		CR=0;
		CCAPM1=0x00;
	}
}




/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
   static unsigned int num = 0;
	 num ++;
	 if(0 == num%20 ) //20ms
	 {
			num = 0;
		  if(isIncrease)
			{
				 pwmValue ++;
				 if(pwmValue == 0x7F)
				 {
					 isIncrease = NO;
				 }
			}
		 else
		 {
				pwmValue--;
				if(pwmValue == 0)
				{
					isIncrease = YES;
				}
		 }
	 
			//CCAP1H = pwmValue; // ����PWMֵ
		 	 PCA_PWM1 = 0x40;
			 CCAP1H = CCAP1L = pwmValue;
			 CCAPM1 = 0x42;
			 CR = 1;
	 }
	 
}

