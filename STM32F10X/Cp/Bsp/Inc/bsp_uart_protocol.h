/*
*********************************************************************************************************
*
*	模块名称 : 串口通讯协议处理模块
*	文件名称 : bsp_uart_protocol.h
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_UART_PROTOCOL_H_
#define _BSP_UART_PROTOCOL_H_



/*	用于OTA版本回复的版本宏定义	*/
#define	OTA_MAJOR_VERSION		1		/*	主版本	*/
#define	OTA_MINOR_VERSION		0		/*	次版本	*/

/*	控制指令	Sx0xxx#	*/
#define	SX0XXX	1
/*	查询指令	Sx#	*/
#define	SX			1

#define	TX			1
#define	T				1


void uart_protocol_handle(void);

void net_protocol_handle(void);

#endif

/***************************** (END OF FILE) *********************************/

