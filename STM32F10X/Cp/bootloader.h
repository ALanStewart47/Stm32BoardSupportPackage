/*
*********************************************************************************************************
*
*	ģ������ : ������������(For STM32F1)
*	�ļ����� : bootloader.h
*	��    �� : V1.0
*	˵    �� : �����Զ�Э��Ĵ�����������
*	�޸ļ�¼ :
*		�汾��  ����         ����       ˵��
*		V1.0    2024-06-03  	Alan  	��ʽ����
*
*********************************************************************************************************
*/
#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stm32f1xx_hal.h"

/*	---�����޸ĵ�ַ	*/
#define NVIC_VectTab_FLASH        ((uint32_t)0x08000000)		//Bootloader��ַ
#define APPLICATION_POSADDR_A 	  (0x00003800) 							//APPƫ���� 
#define BOOT_FLAG_ADDR  					(0x8003400)								//Ҫ�����ĵ�ַ

#define DATA_SAVE_ADDR								((uint32_t)(0x800F000))		//���ݱ����ַ		//2K
#define BACK_DATA_SAVE_ADDR						((uint32_t)(0x800F800))		//���ݱ����ַ		//2K
#define BAUD_DATA_SAVE_ADDR						((uint32_t)(0x800FE00))		//���ݱ����ַ		//2K

#define	DataSucFlag										0x66					//����д��ɹ���־λ
#define	BackDataSucFlag								0x88					//������д��ɹ���־λ

#define	BaudDataSucFlag								0x99					//������-д��ɹ���־λ


/*	��������ƫ��	*/
void NVIC_SetVectorTable(uint32_t base, uint32_t offset);
/*	д��������־λ	*/
void set_BootLoader_flag(void);

uint8_t bsp_CheckFlashSucess(void);

/*	�����Ҫ����	*/
void bsp_SaveFlashData(void);
/*	��ȡ����	*/
void bsp_ReadFlashData(void);

void bsp_SaveFlashBackupData(void);

void bsp_SaveBaudData(void);

uint32_t bsp_CheckBaud(void);

#endif
/***************************** (END OF FILE) *********************************/
