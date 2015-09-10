
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


#ifndef		__CONFIG_H
#define		__CONFIG_H


/*********************************************************/

//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟


/*********************************************************/

#include	"STC15Fxxxx.H"


#define YES 1
#define NO 0

#define OFF 0
#define ON 1


#define BIT(n) (1<<n)
/*****************系统引脚定义******************/
sbit EARPHONE=P1^1;			//断电器
sbit AUDIO_POWER=P3^7;          //放大电路电源
sbit LED_PWM=P1^0;			//LED负极
sbit BLUE=P1^3;				
sbit GREEN=P1^5;
sbit RED=P1^4;
sbit SIN_IN=P3^6;				//信号输入接口检测
sbit SIN_OUT=P3^3;			//耳机接口检测
sbit KEY=P3^2;					//开关按键检测
sbit KEY_LED=P5^5;			//按键LED

#define CCP_S0 0x10
#define CCP_S1 0X20


//类型定义
typedef unsigned char uint8;
typedef char sint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned long uint32;
typedef long sint32;

#define T1MS (65536UL - (MAIN_Fosc / 1000))

#define NULL ((void *)0)

#endif
