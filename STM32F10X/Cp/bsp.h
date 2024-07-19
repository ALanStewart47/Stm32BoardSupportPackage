/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F1)
*	文件名称 : bsp.h
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*			   bsp = Borad surport packet 板级支持包
*	修改记录 :
*		版本号  日期         作者       说明
*		V1.0    2024-05-20  	Alan  	正式发布
*
*
*********************************************************************************************************
*/
#ifndef _BSP_H_
#define _BSP_H


/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf		printf
//#define BSP_Printf(...)


#define ERROR_HANDLER()		Error_Handler(__FILE__, __LINE__);


#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/*	测试模式	*/
#ifndef DEBUGMODE
	#define	DEBUGMODE	0
#endif

/*	更新版本号，需要修改该宏定义	*/
#define	MAJOR_VERSION		1		//主版本号
#define	MINOR_VERSION		0		//次版本号
 
/* 光源硬件通道数，根据项目实际设置 */	
#define LIGHT_CHANNEL_HARD_NUM		3			
#define LIGHT_CHANNEL_NUM		LIGHT_CHANNEL_HARD_NUM

//任务定时轮询，单位ms	
#define	ADC_FILTER_TIME					5					//ADC定时滤波
#define	IWDG_TIME								50				//看门狗定时50ms，复位时间为798.4ms
#define	FLASH_BACKUP_TIME				1200000		//后备数据刷写时间，20分钟
#define CHECK_POWER_TIME				6					//检查电源

/*	看门狗定时宏定义	*/
#define IWDG_TIME_800ms		500
#define IWDG_TIME_1S			625
#define IWDG_TIME_2S			1250
#define IWDG_TIME_3S			1875
#define IWDG_TIME_4S			2500
#define	IWDG_TIME_5S			3125


/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_light_ctrl.h"
#include "bsp_aip650.h"
#include "bsp_key_display.h"
#include "bsp_uart_fifo.h"
#include "bsp_uart_protocol.h"
#include "bsp_trig.h"
#include "bootloader.h"
#include "bsp_mcp4922.h"
#include "bsp_adc_filter.h"

/*	---OTA软件版本	*/
extern	uint8_t g_OtaVersion[2];
/*	---串口波特率		*/
extern uint32_t	g_Uart1_Baud;

 
/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);

void bsp_RunPer10ms(void);
void bsp_RunPer1ms(void);

void bsp_feedDog(void);

//void Error_Handler(char *file, uint32_t line);

#endif

/***************************** (END OF FILE) *********************************/
