/*
*********************************************************************************************************
*
*	模块名称 : 定时器模块
*	文件名称 : bsp_timer.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

//#include "main.h"

#include "stm32f1xx_hal.h"

#include "bsp.h"

//#define ENABLE_INT() __set_PRIMASK(0) /* 使能全局中断 */
//#define DISABLE_INT() __set_PRIMASK(1) /* 禁止全局中断 */

/*
	在此定义若干个软件定时器全局变量
	注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TMR_COUNT	5		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

enum
{
	ADC_FILTER_TMR 			= 0,		/*	ADC滤波定时器	*/
	IWDG_TMR				=	1,		/*	独立开门狗定时器	*/
	FLASH_BACKUP_TMR		=	2,		/*	数据备份定时器	*/
	CHECK_POWER_TMR			=	3		/*  电源检测定时器	*/
};


/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t Mode;		/* 计数器模式，1次性 */
	volatile uint8_t Flag;		/* 定时到达标志  */
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
}SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
void SysTick_Handler(void);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);

//void bsp_InitHardTimer(void);
//void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
