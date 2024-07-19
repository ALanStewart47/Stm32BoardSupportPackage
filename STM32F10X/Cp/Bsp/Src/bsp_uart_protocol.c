/*
*********************************************************************************************************
*
*	ģ������ : ����ͨѶЭ�鴦��ģ��
*	�ļ����� : bsp_uart_protocol.c
*	��    �� : V1.0
*	˵    �� : ������bsp_uart_fifo.h����������Э�飬������fifo���������
*	�޸ļ�¼ :
*		�汾��  ����       	����    ˵��
*		V1.0    2024-05-28 	Alans  	��ʽ����
*
*********************************************************************************************************
*/
#include "bsp.h"

static	void send_ModePara(COM_PORT_E _ucPort)
{
	uint8_t	_ucSendBuf[1]	=	{0};
	
	if(g_CtrlMode	==	MODE_H)
	{
		_ucSendBuf[0]	=	'h';
		comSendBuf(_ucPort,_ucSendBuf,1);
	}
	if(g_CtrlMode	==	MODE_L)
	{
		_ucSendBuf[0]	=	'l';
		comSendBuf(_ucPort,_ucSendBuf,1);
	}
}

/*
*********************************************************************************************************
*	�� �� ��:		send_ChannelBrightness
*	����˵��: 	�����β�ͨ��������ֵ
*	��    ��:  	_ucChannel	��	��Դͨ��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static	void send_ChannelBrightness(uint8_t	_ucChannel,COM_PORT_E _ucPort)
{
	uint8_t	_ucSendBuf[5]	=	{0};
	
	if(_ucChannel < LIGHT_CHANNEL_HARD_NUM )
	{
		_ucSendBuf[0]	=	_ucChannel	+	'a';
		_ucSendBuf[1]	=	'0';
		_ucSendBuf[2]	=	(g_CHx[_ucChannel].value	%1000	/	100)	+	'0'	;
		_ucSendBuf[3]	=	g_CHx[_ucChannel].value	%100 /	10	+	'0';
		_ucSendBuf[4]	=	g_CHx[_ucChannel].value	%10	+	'0';

		comSendBuf(_ucPort,_ucSendBuf,5);
		bsp_DacUpdata();
	}
}

/*
*********************************************************************************************************
*	�� �� ��:		Analyze_VER_Command
*	����˵��: 	�ظ��汾��
*	��    ��:  	��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static	void Analyze_VER_Command(COM_PORT_E _ucPort)
{
	uint8_t	_ucVerBuf[3]	=	{MAJOR_VERSION + '0', 0x2E ,MINOR_VERSION + '0'};

	comSendBuf(_ucPort,_ucVerBuf,3);
}


static	void Analyze_CST_Command(COM_PORT_E _ucPort)
{
	uint8_t	_ucCstBuf[3]	=	{'C','S','T'};
	
	comSendBuf(_ucPort,_ucCstBuf,3);
}

/*
*********************************************************************************************************
*	�� �� ��:		Analyze_Tx_Command
*	����˵��: 	����Tx���ݣ����ص�ǰģʽ
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static void Analyze_Tx_Command(uint8_t	_ucMode,COM_PORT_E _ucPort)
{
	uint8_t	_ucReBuf[1]	=	{0};
	
	/*	Ϊ����ָ��	*/
	if(_ucMode	==	MODE_H)
	{
		if(bsp_ModeCtrl(MODE_H))
		{
			_ucReBuf[0]	=	'h';
			/*	���� 'h' ��ʾ�л��ɹ�	*/
			comSendBuf(_ucPort,_ucReBuf,sizeof(_ucReBuf));
			
			bsp_ChangeDisPlayPage(0);
		}
	}
	/*	Ϊ����ָ��	*/
	else if(_ucMode	==	MODE_L)
	{
		if(bsp_ModeCtrl(MODE_L))
		{
			_ucReBuf[0]	=	'l';
			/*	���� 'l' ��ʾ�л��ɹ�	*/
			comSendBuf(_ucPort,_ucReBuf,sizeof(_ucReBuf));
			
			bsp_ChangeDisPlayPage(0);
		}
	}
	else{
	}
}


/*
*********************************************************************************************************
*	�� �� ��:		Analyze_S_Command
*	����˵��: 	��������S��ͷ�����ݣ���ִ�ж�Ӧָ��Ĺ��ܡ�
								ע�⣺��ȷ���������������ʽ����ȷ�ġ�
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static	void Analyze_T_Command( uint8_t	* _ucBuf	,	uint16_t	uslen,COM_PORT_E _ucPort)
{
	char *	p;

	/*	��ȡ�����ַ	*/
	p	=	(char *)_ucBuf;
	
	/*	��ȡ�����е�һ��T���ڵĵ�ַ���������ֱ�ӷ������ݰ�	*/
	p	=	strchr(p,'T');

	/*	ȷ����������#	*/
	if(strstr(p,"CS") == NULL)
	{
		if(	strchr(p,'#') != NULL	)
		{	
			switch(*(p+1))
			{
				case 'H' :
				#if DEBUGMODE
					printf("Mode H	\r\n");
				#endif
					/*	ģʽ��Ϊ����	*/
					Analyze_Tx_Command(MODE_H,_ucPort);
				break;
				
				case	'L':
					#if DEBUGMODE
						printf("Mode L	\r\n");
					#endif
					/*	ģʽ��Ϊ����	*/
					Analyze_Tx_Command(MODE_L,_ucPort);
				break;
				
				case	'#' :
					#if DEBUGMODE
						printf("T -HorL	\r\n");
					#endif
					/*	����ģʽ����	*/
					send_ModePara(_ucPort);
				break;
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��:		cmd_SxHandler
*	����˵��: 	��ѯ����ָ�SX#�������͵����ݷ����� eg:	SA#
								ע�⣺��ȷ���������������ʽ����ȷ�ġ�
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
void Analyze_Sx_Command(char * _ucPointer,COM_PORT_E _ucPort)
{
	char * p;
	uint8_t _ucChannel	=	0;
	
	p =	_ucPointer;	
	
		p++;
	
	/*	��Ҫ���ڵ���ͨ��A���Ž��з���	*/
	if('	*P	'	>= 'A' )
	{
		/*	ͨ����	*/
		_ucChannel	=	(uint16_t)(*p - 'A');
	
		if(_ucChannel	<=	LIGHT_CHANNEL_NUM)
		{
			#if DEBUGMODE
				//printf("Channel is %d \r\n",_ucChannel);
			
			#endif
		
			/*	��ӷ��ض�Ӧͨ������ֵ�ĺ���	*/
			send_ChannelBrightness(_ucChannel,_ucPort);
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��:		cmd_Sx0xxxHandler
*	����˵��: 	��������ָ�SX0XXX#�������͵����ݷ����� eg:	SA0255��
								ע�⣺��ȷ���������������ʽ����ȷ�ġ�
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	true of false
*********************************************************************************************************
*/
int8_t Analyze_Sx0xxx_Command(char *	_ucPointer,COM_PORT_E _ucPort)
{
	char * p;
	uint8_t _ucChannel	=	0;
	uint16_t _usBrightness	=	0;
	uint8_t	_ucReBuf[3]	=	{0};	
	
	p =	_ucPointer;	
	p++;
	
	/*	��Ҫ���ڵ���ͨ��A���Ž��з���	*/
	if('	*P	'	>= 'A' )
	{
		/*	ͨ����	*/
		_ucChannel	=	(uint16_t)(*p - 'A');
		if(_ucChannel	<	LIGHT_CHANNEL_NUM)
		{
			/*	����ֵ	*/
			_usBrightness	=	(*(p+2)-'0')*100	+	(*(p+3)-'0')*10	+(*(p+4)-'0');
			
			/*	���øı����ȵ�API	*/
			bsp_SetBrightness(_ucChannel,_usBrightness);
			/*	��ʾ��Ӧͨ����ҳ��*/
			bsp_ChangeDisPlayPage((_ucChannel+1));
			
			#if DEBUGMODE
				printf("Value is %d \r\n",_usBrightness);
			#endif
			_ucReBuf[0]	=	_ucChannel	+	'a';
			
			comSendBuf(_ucPort,_ucReBuf,1);
			return	TRUE;
		}
	}
	else
		return	FALSE;
}	


/*
*********************************************************************************************************
*	�� �� ��:		Analyze_S_Command
*	����˵��: 	��������S��ͷ�����ݣ���ִ�ж�Ӧָ��Ĺ��ܡ�
								ע�⣺��ȷ���������������ʽ����ȷ�ġ�
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static	void Analyze_S_Command( uint8_t	* _ucBuf	,	uint16_t	uslen,COM_PORT_E _ucPort)
{
	char *	p;
	char *	_ucTemp;
	
	/*	��ȡ�����ַ	*/
	p	=	(char *)_ucBuf;
	
	/*	��ȡ�����е�һ��S���ڵĵ�ַ���������ֱ�ӷ������ݰ�	*/
	p	=	strchr(p,'S');

	
	/*	ȷ����������#	*/
	if(	strchr(p,'#') != NULL	)
	{
		_ucTemp	=	p+2;
	/*	--------SX#����--��ѯ��Դ����----------	*/
		if(*_ucTemp	==	'#')		
		{
			Analyze_Sx_Command(p,_ucPort);
		}
		else if(*_ucTemp	==	'0')									
		{
			_ucTemp	= _ucTemp+4;
			/*	--------SX0XXX#����--���ù�Դ����---------	*/
			if(*_ucTemp	==	'#')
			{
				Analyze_Sx0xxx_Command(p,_ucPort);
			}
		}
		else{
			
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��:	Analyze_SetBps_Command
*	����˵��: �����ʿɵ����ܣ�0-Լ���������� ��1~8�������ʵ�λ�� �����ڴ���1�ɵ���
*	��    ��:  ����ָ��	-	_ucPointer
*	�� �� ֵ:  true of false
*********************************************************************************************************
*/
void bsp_SetBps_Command(char *	_ucPointer)
{
	char * p;
	uint8_t	i;
	uint32_t _usBaudRate	=	0;

	p =	_ucPointer;
	p	=	p +	4;
	i	=	*p	-	'0';
	
	switch(i)
	{
		case 0:
			printf("brc");
		break;
		
		case 1:
			printf("brc");
			g_Uart1_Baud	=	4800;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case 2:
			printf("brc");
			g_Uart1_Baud	=	9600;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
			
		case 3:
			printf("brc");
			g_Uart1_Baud	=	14400;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case	4 :
			printf("brc");
			g_Uart1_Baud	=	19200;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case	5 :
			printf("brc");
			g_Uart1_Baud	=	28800;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case	6 :
			printf("brc");
			g_Uart1_Baud	=	38400;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case	7 :
			printf("brc");
			g_Uart1_Baud	=	57600;			
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
		break;
		
		case	8:
			printf("brc");
			g_Uart1_Baud	=	115200;
			bsp_SaveBaudData();
			bsp_DelayUS(5);
			comSetBaud(COM1,g_Uart1_Baud);
			break;
		
		default:
			break;		
	}
	
}

/*
*********************************************************************************************************
*	�� �� ��:		Analyze_B_Command
*	����˵��: 	��������B��ͷ�����ݣ���ִ�ж�Ӧָ��Ĺ��ܡ�
								ע�⣺��ȷ���������������ʽ����ȷ�ġ�
*	��    ��:  	����ָ��	-	_ucPointer
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static	void Analyze_B_Command( uint8_t	* _ucBuf	,	uint16_t	uslen)
{
	char *	p;
	char *	_ucTemp;
	
	/*	��ȡ�����ַ	*/
	p	=	(char *)_ucBuf;

	/*	��ȡ�����е�һ��S���ڵĵ�ַ���������ֱ�ӷ������ݰ�	*/
	p	=	strrchr(p,'B');

	/*	ȷ����������#	*/
		_ucTemp	=	p+2;
		/*	--------BRC0X#����-----------	*/
		if(*_ucTemp	==	'C')		
		{
			bsp_SetBps_Command(p);
		}
	
}


/*
*********************************************************************************************************
*	�� �� ��:	CST_UART
*	����˵��: 	����������������CST��
*	��    ��:  	ucData-���� 
*	�� �� ֵ: 	TRUE:���ҵ�CST �� FALSE:δ�ҵ�CST
*********************************************************************************************************
*/
static uint8_t CST_UART(uint8_t	 ucData)
{
	static uint8_t _ucCst = 0;
	if(ucData	==	'C')
	{
		_ucCst ++;
	}
	if(ucData	== 'S')
	{
		_ucCst ++;
	}
	if(ucData	== 'T')
	{
		if(_ucCst == 2)
		{
			_ucCst = 0 ;
			return TRUE;
		}
	}
	return FALSE;		
}

/*
*********************************************************************************************************
*	�� �� ��:	CST_NET
*	����˵��: 	����������������CST��
*	��    ��:  	ucData-���� 
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
static uint8_t CST_NET(uint8_t	 ucData)
{
	static uint8_t _ucCst = 0;
	if(ucData	==	'C')
	{
		_ucCst ++;
	}
	if(ucData	== 'S')
	{
		_ucCst ++;
	}
	if(ucData	== 'T')
	{
		if(_ucCst == 2)
		{
			_ucCst = 0 ;
			return TRUE;
		}
	}
	return FALSE;		
}

/*
*********************************************************************************************************
*	�� �� ��:	bootloaderCommand
*	����˵��: 	bootloader�����������ʶ��
*	��    ��:  	ucData-���� 
*	�� �� ֵ: 	TRUE��������ȷ  ��  FALSE�����ݴ���
*********************************************************************************************************
*/
static uint8_t bootloaderCommand(uint8_t ucData)
{
	static uint8_t _ucBoot = 0;
	uint8_t * p ;
	p = &ucData;

	switch ( *p )
	{
		case 0x72 :
			_ucBoot ++;
			break;
		case 0x68 :
			_ucBoot ++;
			break;
		case 0xaa :
			_ucBoot ++;
			break;
		case 0xbb :
			_ucBoot ++;
			break;
		case 0x16 :
			_ucBoot ++;
			break;

		default:
			break;
	}

	if(_ucBoot == 4)
	{
		_ucBoot = 0;
		return TRUE;
	}
	else if(_ucBoot > 4)
	{
		_ucBoot = 0;
		return FALSE;
	}
	else {
		return FALSE;
	}		
}

/*
*********************************************************************************************************
*	�� �� ��:	bsp_RunBootLoader
*	����˵��: 	bootloader��������ִ�У�����APP�̼����£��Ͱ汾�ŷ��ء�
*	��    ��:  	_ucBuf-�������� ��_uslen:���ݳ���
*	�� �� ֵ: 	TRUE��������ȷ  ��  FALSE�����ݴ���
*********************************************************************************************************
*/
static void bsp_RunBootLoader(uint8_t * _ucBuf , uint16_t _uslen)
{
	uint8_t ref_BUf[6] = {0x72,0x68,0xbb,MAJOR_VERSION,MINOR_VERSION,0x16};
	uint8_t _usPos , _ucBufNum= 0;
	uint8_t * p ;
	p = _ucBuf;
	
	for(_ucBufNum = 0; _ucBufNum <= _uslen; _ucBufNum++)
	{
		if(*(p+_ucBufNum) == 0x72)
		{
			_usPos = _ucBufNum;
			/*	���¹̼�ָ��--[0x72��0x68��0xaa, xx,xx,0x16] */
			if((*(p+_usPos+1) == 0x68) && (*(p+_usPos+2) == 0xAA) && (*(p+_usPos+5) == 0x16))
			{
				/*	ͬһ�汾�������� */
				if( (*(p+_usPos+3) == g_OtaVersion[0]) && (*(p+_usPos+4) == g_OtaVersion[1]) )
				{
					comSendBuf(COM1,ref_BUf,sizeof(ref_BUf));
				}
				else
				{ 
					#if DEBUGMODE
						printf("OTA	\r\n");
					#endif
					set_BootLoader_flag();
					 __set_FAULTMASK(1);
					HAL_NVIC_SystemReset();		
				}
			}
			/*	��ѯ�汾��ָ��--[0x72��0x68��0xbb,0x16] */
			if( (*(p+_usPos+1) == 0x68) && (*(p+_usPos+2) == 0xBB) && (*(p+_usPos+3) == 0x16) )
			{
				comSendBuf(COM1,ref_BUf,sizeof(ref_BUf));
			}
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��:	Analyze_Cst_Protocol
*	����˵��: 	����Э��������Ӵ��ڽ��ջ�����ȡ���ݲ�������
*	��    ��:  	_ucBuf-���� �� _uslen- ���鳤��	, _ucPort:COM�ڣ����ڣ�
*	�� �� ֵ: 	��STBV
*********************************************************************************************************
*/
static void Analyze_Cst_Protocol(uint8_t	* _ucBuf	,	uint16_t _uslen ,COM_PORT_E _ucPort)
{
	char * p;
	p = (char *)_ucBuf;
	if(strchr(p, 'S')	!=	NULL)	/* S��ͷ��ָ��	*/
	{
		Analyze_S_Command(_ucBuf,_uslen,_ucPort);
	}
	if(strchr(p, 'T')	!=	NULL)	/*	T��ͷ��ָ��	*/
	{
		Analyze_T_Command(_ucBuf,_uslen,_ucPort);
	}
	if(strstr(p, "BRC")	!=	NULL)	/*	B��ͷ��ָ��	*/	
	{
		Analyze_B_Command(_ucBuf,_uslen);
	}
	if(strstr(p,"VER")	!=	NULL)	/*	��ѯ�汾��	*/
	{
		Analyze_VER_Command(_ucPort);
	}
}


/*
*********************************************************************************************************
*	�� �� ��:	uart_protocol_handle
*	����˵��: 	����ȡ���ݲ�����Э��������Ӵ��ڽ��ջ�����ȡ���ݲ���������bsp.c����
*					���ڱ����ڶ���OTA�Ϳɱ䲨���ʵĹ��ܡ�
*	��    ��:  	��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
void uart_protocol_handle(void)
{
	uint8_t ucData	=	0;
	uint8_t	ucDataEndFlag	=	0;
	static	uint8_t ucUartBuf[60];		//����ʵ����Ŀ��ָ���������
	static uint16_t us_Uart1_Pos = 0;		//����1 ��ָ��
	
	while(1)
	{
		/*	�ӽ��ջ�������ȡ1�ֽڣ���������1-�����ݣ�0-������	*/
		if(comGetChar(COM1,&ucData) == 1)
		{
			if(ucDataEndFlag	==	0)
			{
				if (us_Uart1_Pos < sizeof(ucUartBuf))
				{
					ucUartBuf[us_Uart1_Pos++] = ucData;
					
					/*	����ָ���� '#'Ϊ������	*/
					if (ucData == '#')
					{
						Analyze_Cst_Protocol(ucUartBuf,us_Uart1_Pos-1,COM1);
						ucDataEndFlag	=	1;
					}
					/*	������	*/
					if(CST_UART(ucData) == TRUE)
					{
						Analyze_CST_Command(COM1);
						ucDataEndFlag	=	1;
					}
					/*	bootloader��ص�ָ�� */
					if(bootloaderCommand(ucData) == TRUE)
					{
						bsp_RunBootLoader(ucUartBuf,us_Uart1_Pos);
						ucDataEndFlag	=	1;
					}
				}
				else
				{
					us_Uart1_Pos = 0 ;
				}
			}
			if(ucDataEndFlag == 1)
			{
				us_Uart1_Pos	=	0	;
				memset(ucUartBuf,0,sizeof(ucUartBuf));	/*	�������� */
				break;
			}
			continue;	/* ���ܻ������ݣ��������� */
		}
		break;
	}	
}



/*
*********************************************************************************************************
*	�� �� ��:	net_protocol_handle
*	����˵��: 	����ȡ���ݲ�����Э��������Ӵ��ڽ��ջ�����ȡ���ݲ���������bsp.c����
*	��    ��:  	��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
void net_protocol_handle(void)
{
	uint8_t ucData	=	0;
	uint8_t	ucDataEndFlag	=	0;
	static	uint8_t ucNetBuf[60];		//����ʵ����Ŀ��ָ���������

	static uint8_t _ucNCst	=	0;
	static uint16_t us_Uart3_Pos = 0;		//����1 ��ָ��
	
	while(1)
	{
		/*	�ӽ��ջ�������ȡ1�ֽڣ���������1-�����ݣ�0-������	*/
		if(comGetChar(COM3,&ucData)==1)
		{
			if(ucDataEndFlag	==	0)
			{
				if (us_Uart3_Pos < sizeof(ucNetBuf))
				{
					ucNetBuf[us_Uart3_Pos++] = ucData;
					
					/*	���#	*/
					if (ucData == '#')
					{
						Analyze_Cst_Protocol(ucNetBuf,us_Uart3_Pos-1,COM3);
						ucDataEndFlag	=	1;
					}
					if(CST_NET(ucData) == TRUE)
					{
						Analyze_CST_Command(COM3);
						ucDataEndFlag	=	1;
					}	
				}
				else
				{
					us_Uart3_Pos	=	0	;
				}
			}
			if(ucDataEndFlag == 1)
			{
				us_Uart3_Pos	=	0	;
				_ucNCst = 0 ;
				memset(ucNetBuf,0,sizeof(ucNetBuf));
			}
			continue;	/* ���ܻ������ݣ��������� */
		}
		break;
	}	
}
