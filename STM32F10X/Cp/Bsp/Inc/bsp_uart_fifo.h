/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	˵    �� : ͷ�ļ�
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
	���ڷ��䣺
	������1�� RS232
		PA9/USART1_TX	  
		P10/USART1_RX

	������2�� ��ռ��	��������; PA3-ADC����
		PA2/USART2_TX/
		PA3/USART2_RX	

	������3�� ���� ͨ��
		PB10/USART3_TX
		PB11/USART3_RX

	������4�� ��ռ��	������ʹ��
	������5�� ��ռ��	������ʹ��
*/
/*	
*	Uartʹ��	
*	����ʵ������򿪻�ر�uart
*	1	=	ʹ�� ��0	=	ʧ��	��
*	c8t6	-	��������USART
*	rct6	-	������USART(1\2\3),����UART
*	vet6	-	������USART(1\2\3),����UART
*	zet6	-	ͬ��
*/
#define	UART1_FIFO_EN		1
#define	UART2_FIFO_EN		0
#define	UART3_FIFO_EN		1
#define	UART4_FIFO_EN		0
#define	UART5_FIFO_EN		0


/* ����˿ں� */
typedef enum
{
	COM1 = 0,		/* USART1 */
	COM2 = 1,		/* USART2 */
	COM3 = 2,		/* USART3 */
	COM4 = 3,		/* UART4 */
	COM5 = 4		/* UART5 */
}COM_PORT_E;



/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
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




/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
	uint8_t Sending;			/* ���ڷ����� */
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
