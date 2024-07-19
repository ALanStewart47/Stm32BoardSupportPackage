/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32F1)
*	�ļ����� : bsp.h
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ���������е���������ģ�顣
*			   bsp = Borad surport packet �弶֧�ְ�
*	�޸ļ�¼ :
*		�汾��  ����         ����       ˵��
*		V1.0    2024-05-20  	Alan  	��ʽ����
*
*
*********************************************************************************************************
*/
#ifndef _BSP_H_
#define _BSP_H


/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

/* ���������ڵ��Խ׶��Ŵ� */
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

/*	����ģʽ	*/
#ifndef DEBUGMODE
	#define	DEBUGMODE	0
#endif

/*	���°汾�ţ���Ҫ�޸ĸú궨��	*/
#define	MAJOR_VERSION		1		//���汾��
#define	MINOR_VERSION		0		//�ΰ汾��
 
/* ��ԴӲ��ͨ������������Ŀʵ������ */	
#define LIGHT_CHANNEL_HARD_NUM		3			
#define LIGHT_CHANNEL_NUM		LIGHT_CHANNEL_HARD_NUM

//����ʱ��ѯ����λms	
#define	ADC_FILTER_TIME					5					//ADC��ʱ�˲�
#define	IWDG_TIME								50				//���Ź���ʱ50ms����λʱ��Ϊ798.4ms
#define	FLASH_BACKUP_TIME				1200000		//������ˢдʱ�䣬20����
#define CHECK_POWER_TIME				6					//����Դ

/*	���Ź���ʱ�궨��	*/
#define IWDG_TIME_800ms		500
#define IWDG_TIME_1S			625
#define IWDG_TIME_2S			1250
#define IWDG_TIME_3S			1875
#define IWDG_TIME_4S			2500
#define	IWDG_TIME_5S			3125


/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
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

/*	---OTA����汾	*/
extern	uint8_t g_OtaVersion[2];
/*	---���ڲ�����		*/
extern uint32_t	g_Uart1_Baud;

 
/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);

void bsp_RunPer10ms(void);
void bsp_RunPer1ms(void);

void bsp_feedDog(void);

//void Error_Handler(char *file, uint32_t line);

#endif

/***************************** (END OF FILE) *********************************/
