/*
*********************************************************************************************************
*
*	模块名称 : 数码管驱动IC-驱动模块
*	文件名称 : bsp_aip650.h
*	版    本 : V1.0
*	说    明 : Aip650的底层驱动代码，使用模拟IIC来驱动，基于Hal库
*
*	修改记录 :
*		版本号  日期       		 作者     说明
*		V1.0    2024-06-05 		 alan  		
*
*********************************************************************************************************
*/
#ifndef __BSP_AIP650_H
#define __BSP_AIP650_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

/**
*	在此修改AIP650的CLK和DAT引脚的PORT和PIN
**/
#define AIP650_PIN_CLK		GPIO_PIN_13
#define AIP650_PIN_DAT		GPIO_PIN_14
#define AIP650_PORT_DAT		GPIOB
#define AIP650_PORT_CLK		GPIOB

/**
*	四位数码管驱动芯片位选对应的高八位命令
**/
#define CMD_DIG0	0x68			//数码管	第一位	- 通道（1\2\3...）或者模式（H）
#define CMD_DIG1	0x6a			//数码管	第二位	-	亮度值百位
#define CMD_DIG2	0x6c			//数码管	第三位	-	亮度值十位
#define CMD_DIG3	0x6e			//数码管	第四位	-	亮度值个位


//声明数码管显示内容数组
extern const uint8_t DISPLAY_NUM[16];

//void AIP650_Wr_RAM(uint8_t Address, uint8_t Data);
void Tube_DisNum(uint8_t *Dis);
void bsp_InitAip650(void);
void bsp_DisplayErrAip650(uint8_t Err_code);
void bsp_DisplayON(void);


#endif 

