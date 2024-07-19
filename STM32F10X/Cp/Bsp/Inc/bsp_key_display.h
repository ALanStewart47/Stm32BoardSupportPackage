/*
*********************************************************************************************************
*
*	模块名称 : 按键显示模块
*	文件名称 : bsp_key_display.h
*	版    本 : V1.0
*	说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问
*	修改记录 :
*		版本号  日期       	作者    说明
*		V1.0    2024-05-28 	Alans  	正式发布
*
*********************************************************************************************************
*/
#ifndef __BSP_KEY_DISPLAY_H
#define __BSP_KEY_DISPLAY_H

#include "bsp_light_ctrl.h"
#include "bsp_aip650.h"
#include "bsp_key.h"
#include <stdint.h>

/*	可显示最大页面数，除了通道显示还包括模式显示，故+1	*/
/*	此宏定义不用修改，后续增加通道仅需修改最大通道数		*/
#define PAGE_MAX	LIGHT_CHANNEL_NUM+1		


//显示页面变量，尽量不要直接修改变量，请通过函数修改
extern uint8_t	g_DisPlayPage;

extern uint8_t	g_test_DisPlayPage;

/*	数码管显示的核心函数	*/
void bsp_ChangeDisPlayPage(uint16_t	_usChannel);
/*	数码管显示的核心函数	*/
void bsp_DisplayHandler(void);
/*	按键处理函数	*/
void app_Key_Handler(void);
/*  数码管显示错误状态函数  */
uint8_t bsp_ErrorDisplay(void);

#endif
