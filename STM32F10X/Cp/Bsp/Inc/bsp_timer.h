/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

//#include "main.h"

#include "stm32f1xx_hal.h"

#include "bsp.h"

//#define ENABLE_INT() __set_PRIMASK(0) /* ʹ��ȫ���ж� */
//#define DISABLE_INT() __set_PRIMASK(1) /* ��ֹȫ���ж� */

/*
	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	5		/* �����ʱ���ĸ��� ����ʱ��ID��Χ 0 - 3) */

enum
{
	ADC_FILTER_TMR 			= 0,		/*	ADC�˲���ʱ��	*/
	IWDG_TMR				=	1,		/*	�������Ź���ʱ��	*/
	FLASH_BACKUP_TMR		=	2,		/*	���ݱ��ݶ�ʱ��	*/
	CHECK_POWER_TMR			=	3		/*  ��Դ��ⶨʱ��	*/
};


/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;		/* ������ģʽ��1���� */
	volatile uint8_t Flag;		/* ��ʱ�����־  */
	volatile uint32_t Count;	/* ������ */
	volatile uint32_t PreLoad;	/* ������Ԥװֵ */
}SOFT_TMR;

/* �ṩ������C�ļ����õĺ��� */
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
