/*
*********************************************************************************************************
*
*	ģ������ : ����ͨѶЭ�鴦��ģ��
*	�ļ����� : bsp_uart_protocol.h
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_UART_PROTOCOL_H_
#define _BSP_UART_PROTOCOL_H_



/*	����OTA�汾�ظ��İ汾�궨��	*/
#define	OTA_MAJOR_VERSION		1		/*	���汾	*/
#define	OTA_MINOR_VERSION		0		/*	�ΰ汾	*/

/*	����ָ��	Sx0xxx#	*/
#define	SX0XXX	1
/*	��ѯָ��	Sx#	*/
#define	SX			1

#define	TX			1
#define	T				1


void uart_protocol_handle(void);

void net_protocol_handle(void);

#endif

/***************************** (END OF FILE) *********************************/

