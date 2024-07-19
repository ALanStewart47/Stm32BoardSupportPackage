/*
*********************************************************************************************************
*
*	ģ������ : ���������IC-����ģ��
*	�ļ����� : bsp_aip650.h
*	��    �� : V1.0
*	˵    �� : Aip650�ĵײ��������룬ʹ��ģ��IIC������������Hal��
*
*	�޸ļ�¼ :
*		�汾��  ����       		 ����     ˵��
*		V1.0    2024-06-05 		 alan  		
*
*********************************************************************************************************
*/
#ifndef __BSP_AIP650_H
#define __BSP_AIP650_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

/**
*	�ڴ��޸�AIP650��CLK��DAT���ŵ�PORT��PIN
**/
#define AIP650_PIN_CLK		GPIO_PIN_13
#define AIP650_PIN_DAT		GPIO_PIN_14
#define AIP650_PORT_DAT		GPIOB
#define AIP650_PORT_CLK		GPIOB

/**
*	��λ���������оƬλѡ��Ӧ�ĸ߰�λ����
**/
#define CMD_DIG0	0x68			//�����	��һλ	- ͨ����1\2\3...������ģʽ��H��
#define CMD_DIG1	0x6a			//�����	�ڶ�λ	-	����ֵ��λ
#define CMD_DIG2	0x6c			//�����	����λ	-	����ֵʮλ
#define CMD_DIG3	0x6e			//�����	����λ	-	����ֵ��λ


//�����������ʾ��������
extern const uint8_t DISPLAY_NUM[16];

//void AIP650_Wr_RAM(uint8_t Address, uint8_t Data);
void Tube_DisNum(uint8_t *Dis);
void bsp_InitAip650(void);
void bsp_DisplayErrAip650(uint8_t Err_code);
void bsp_DisplayON(void);


#endif 

