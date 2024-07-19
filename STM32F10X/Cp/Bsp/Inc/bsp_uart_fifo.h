/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_UART_FIFO_H_
#define _BSP_UART_FIFO_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "usart.h"

/*
	串口分配：
	【串口1】 RS232
		PA9/USART1_TX	  
		P10/USART1_RX

	【串口2】 被占用	触发输入; PA3-ADC引脚
		PA2/USART2_TX/
		PA3/USART2_RX	

	【串口3】 网口 通信
		PB10/USART3_TX
		PB11/USART3_RX

	【串口4】 被占用	且暂无使用
	【串口5】 被占用	且暂无使用
*/
/*	
*	Uart使能	
*	根据实际情况打开或关闭uart
*	1	=	使能 ，0	=	失能	；
*	c8t6	-	仅有三个USART
*	rct6	-	有三个USART(1\2\3),两个UART
*	vet6	-	有三个USART(1\2\3),两个UART
*	zet6	-	同上
*/
#define	UART1_FIFO_EN		1
#define	UART2_FIFO_EN		0
#define	UART3_FIFO_EN		1
#define	UART4_FIFO_EN		0
#define	UART5_FIFO_EN		0


/* 定义端口号 */
typedef enum
{
	COM1 = 0,		/* USART1 */
	COM2 = 1,		/* USART2 */
	COM3 = 2,		/* USART3 */
	COM4 = 3,		/* UART4 */
	COM5 = 4		/* UART5 */
}COM_PORT_E;



/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define	UART1_BAUD			19200	
	#define UART1_TX_BUF_SIZE	100
	#define UART1_RX_BUF_SIZE	100		
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	10
	#define UART2_RX_BUF_SIZE	2*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			19200
	#define UART3_TX_BUF_SIZE	100
	#define UART3_RX_BUF_SIZE	100
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			115200
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif




/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t Sending;			/* 正在发送中 */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void bsp_DeInitHardUart(USART_TypeDef *Instance);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
//void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);

void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate);
void bsp_SetUartParam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

//void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
//void RS485_SendStr(char *_pBuf);
//void RS485_SetBaud(uint32_t _baud);
uint8_t UartTxEmpty(COM_PORT_E _ucPort);
void comDmaSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);

#endif

/***************************** (END OF FILE) *********************************/
