/*
*********************************************************************************************************
*
*	ģ������ : ������ʾģ��
*	�ļ����� : bsp_key_display.h
*	��    �� : V1.0
*	˵    �� : ���ô����ж�+FIFOģʽʵ�ֶ�����ڵ�ͬʱ����
*	�޸ļ�¼ :
*		�汾��  ����       	����    ˵��
*		V1.0    2024-05-28 	Alans  	��ʽ����
*
*********************************************************************************************************
*/
#ifndef __BSP_KEY_DISPLAY_H
#define __BSP_KEY_DISPLAY_H

#include "bsp_light_ctrl.h"
#include "bsp_aip650.h"
#include "bsp_key.h"
#include <stdint.h>

/*	����ʾ���ҳ����������ͨ����ʾ������ģʽ��ʾ����+1	*/
/*	�˺궨�岻���޸ģ���������ͨ�������޸����ͨ����		*/
#define PAGE_MAX	LIGHT_CHANNEL_NUM+1		


//��ʾҳ�������������Ҫֱ���޸ı�������ͨ�������޸�
extern uint8_t	g_DisPlayPage;

extern uint8_t	g_test_DisPlayPage;

/*	�������ʾ�ĺ��ĺ���	*/
void bsp_ChangeDisPlayPage(uint16_t	_usChannel);
/*	�������ʾ�ĺ��ĺ���	*/
void bsp_DisplayHandler(void);
/*	����������	*/
void app_Key_Handler(void);
/*  �������ʾ����״̬����  */
uint8_t bsp_ErrorDisplay(void);

#endif
