/*
* 文件功能：呼吸灯
* 创建时间：2015-09-06
* 作者：hoeoi
*/
#include "config.h"
#include "BreathingLight.h"



static void Timer0Config();
static char s_speed = 1;
static char isIncrease = 0;
static unsigned char pwmValue = 0x7F;
static char isStart = NO;
static void PWM_ON_OFF(bit byte);


//内部函数定义
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
	ET0 = 1;	//允许中断
	PT0 = 0;	//低优先级中断
	TMOD = (TMOD & ~0x03) ;	//工作模式: 16位自动重装
	AUXR |=  0x80;	//1T
	INT_CLKO &= ~0x01;	//不输出时钟
	
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




/********************* Timer0中断函数************************/
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
	 
			//CCAP1H = pwmValue; // 更新PWM值
		 	 PCA_PWM1 = 0x40;
			 CCAP1H = CCAP1L = pwmValue;
			 CCAPM1 = 0x42;
			 CR = 1;
	 }
	 
}

