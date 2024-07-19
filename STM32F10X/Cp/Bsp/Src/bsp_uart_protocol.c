/*
*********************************************************************************************************
*
*	模块名称 : 串口通讯协议处理模块
*	文件名称 : bsp_uart_protocol.c
*	版    本 : V1.0
*	说    明 : 依赖于bsp_uart_fifo.h，根据自有协议，处理串口fifo数组的数据
*	修改记录 :
*		版本号  日期       	作者    说明
*		V1.0    2024-05-28 	Alans  	正式发布
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
*	函 数 名:		send_ChannelBrightness
*	功能说明: 	发送形参通道的亮度值
*	形    参:  	_ucChannel	：	光源通道
*	返 回 值: 	无
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
*	函 数 名:		Analyze_VER_Command
*	功能说明: 	回复版本号
*	形    参:  	无
*	返 回 值: 	无
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
*	函 数 名:		Analyze_Tx_Command
*	功能说明: 	分析Tx数据，返回当前模式
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	无
*********************************************************************************************************
*/
static void Analyze_Tx_Command(uint8_t	_ucMode,COM_PORT_E _ucPort)
{
	uint8_t	_ucReBuf[1]	=	{0};
	
	/*	为常亮指令	*/
	if(_ucMode	==	MODE_H)
	{
		if(bsp_ModeCtrl(MODE_H))
		{
			_ucReBuf[0]	=	'h';
			/*	返回 'h' 表示切换成功	*/
			comSendBuf(_ucPort,_ucReBuf,sizeof(_ucReBuf));
			
			bsp_ChangeDisPlayPage(0);
		}
	}
	/*	为常灭指令	*/
	else if(_ucMode	==	MODE_L)
	{
		if(bsp_ModeCtrl(MODE_L))
		{
			_ucReBuf[0]	=	'l';
			/*	返回 'l' 表示切换成功	*/
			comSendBuf(_ucPort,_ucReBuf,sizeof(_ucReBuf));
			
			bsp_ChangeDisPlayPage(0);
		}
	}
	else{
	}
}


/*
*********************************************************************************************************
*	函 数 名:		Analyze_S_Command
*	功能说明: 	分析所有S开头的数据，并执行对应指令的功能。
								注意：须确保传进来的数组格式是正确的。
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	无
*********************************************************************************************************
*/
static	void Analyze_T_Command( uint8_t	* _ucBuf	,	uint16_t	uslen,COM_PORT_E _ucPort)
{
	char *	p;

	/*	获取数组地址	*/
	p	=	(char *)_ucBuf;
	
	/*	获取数组中第一个T所在的地址，后面可以直接分析数据包	*/
	p	=	strchr(p,'T');

	/*	确保数组中有#	*/
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
					/*	模式切为常亮	*/
					Analyze_Tx_Command(MODE_H,_ucPort);
				break;
				
				case	'L':
					#if DEBUGMODE
						printf("Mode L	\r\n");
					#endif
					/*	模式切为常灭	*/
					Analyze_Tx_Command(MODE_L,_ucPort);
				break;
				
				case	'#' :
					#if DEBUGMODE
						printf("T -HorL	\r\n");
					#endif
					/*	返回模式函数	*/
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
*	函 数 名:		cmd_SxHandler
*	功能说明: 	查询类型指令，SX#命令类型的数据分析， eg:	SA#
								注意：须确保传进来的数组格式是正确的。
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	无
*********************************************************************************************************
*/
void Analyze_Sx_Command(char * _ucPointer,COM_PORT_E _ucPort)
{
	char * p;
	uint8_t _ucChannel	=	0;
	
	p =	_ucPointer;	
	
		p++;
	
	/*	需要大于等于通道A，才进行分析	*/
	if('	*P	'	>= 'A' )
	{
		/*	通道数	*/
		_ucChannel	=	(uint16_t)(*p - 'A');
	
		if(_ucChannel	<=	LIGHT_CHANNEL_NUM)
		{
			#if DEBUGMODE
				//printf("Channel is %d \r\n",_ucChannel);
			
			#endif
		
			/*	添加返回对应通道亮度值的函数	*/
			send_ChannelBrightness(_ucChannel,_ucPort);
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名:		cmd_Sx0xxxHandler
*	功能说明: 	控制类型指令。SX0XXX#命令类型的数据分析， eg:	SA0255。
								注意：须确保传进来的数组格式是正确的。
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	true of false
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
	
	/*	需要大于等于通道A，才进行分析	*/
	if('	*P	'	>= 'A' )
	{
		/*	通道数	*/
		_ucChannel	=	(uint16_t)(*p - 'A');
		if(_ucChannel	<	LIGHT_CHANNEL_NUM)
		{
			/*	亮度值	*/
			_usBrightness	=	(*(p+2)-'0')*100	+	(*(p+3)-'0')*10	+(*(p+4)-'0');
			
			/*	调用改变亮度的API	*/
			bsp_SetBrightness(_ucChannel,_usBrightness);
			/*	显示对应通道的页面*/
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
*	函 数 名:		Analyze_S_Command
*	功能说明: 	分析所有S开头的数据，并执行对应指令的功能。
								注意：须确保传进来的数组格式是正确的。
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	无
*********************************************************************************************************
*/
static	void Analyze_S_Command( uint8_t	* _ucBuf	,	uint16_t	uslen,COM_PORT_E _ucPort)
{
	char *	p;
	char *	_ucTemp;
	
	/*	获取数组地址	*/
	p	=	(char *)_ucBuf;
	
	/*	获取数组中第一个S所在的地址，后面可以直接分析数据包	*/
	p	=	strchr(p,'S');

	
	/*	确保数组中有#	*/
	if(	strchr(p,'#') != NULL	)
	{
		_ucTemp	=	p+2;
	/*	--------SX#命令--查询光源亮度----------	*/
		if(*_ucTemp	==	'#')		
		{
			Analyze_Sx_Command(p,_ucPort);
		}
		else if(*_ucTemp	==	'0')									
		{
			_ucTemp	= _ucTemp+4;
			/*	--------SX0XXX#命令--设置光源亮度---------	*/
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
*	函 数 名:	Analyze_SetBps_Command
*	功能说明: 波特率可调功能，0-约等于心跳包 ，1~8：波特率档位。 仅用于串口1可调。
*	形    参:  数组指针	-	_ucPointer
*	返 回 值:  true of false
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
*	函 数 名:		Analyze_B_Command
*	功能说明: 	分析所有B开头的数据，并执行对应指令的功能。
								注意：须确保传进来的数组格式是正确的。
*	形    参:  	数组指针	-	_ucPointer
*	返 回 值: 	无
*********************************************************************************************************
*/
static	void Analyze_B_Command( uint8_t	* _ucBuf	,	uint16_t	uslen)
{
	char *	p;
	char *	_ucTemp;
	
	/*	获取数组地址	*/
	p	=	(char *)_ucBuf;

	/*	获取数组中第一个S所在的地址，后面可以直接分析数据包	*/
	p	=	strrchr(p,'B');

	/*	确保数组中有#	*/
		_ucTemp	=	p+2;
		/*	--------BRC0X#命令-----------	*/
		if(*_ucTemp	==	'C')		
		{
			bsp_SetBps_Command(p);
		}
	
}


/*
*********************************************************************************************************
*	函 数 名:	CST_UART
*	功能说明: 	串口心跳包，返回CST。
*	形    参:  	ucData-输入 
*	返 回 值: 	TRUE:查找到CST ， FALSE:未找到CST
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
*	函 数 名:	CST_NET
*	功能说明: 	网口心跳包，返回CST。
*	形    参:  	ucData-输入 
*	返 回 值: 	无
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
*	函 数 名:	bootloaderCommand
*	功能说明: 	bootloader相关命令检测与识别，
*	形    参:  	ucData-输入 
*	返 回 值: 	TRUE：数据正确  ，  FALSE：数据错误
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
*	函 数 名:	bsp_RunBootLoader
*	功能说明: 	bootloader相关命令的执行，包括APP固件更新，和版本号返回。
*	形    参:  	_ucBuf-数据输入 ，_uslen:数据长度
*	返 回 值: 	TRUE：数据正确  ，  FALSE：数据错误
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
			/*	更新固件指令--[0x72，0x68，0xaa, xx,xx,0x16] */
			if((*(p+_usPos+1) == 0x68) && (*(p+_usPos+2) == 0xAA) && (*(p+_usPos+5) == 0x16))
			{
				/*	同一版本号则不升级 */
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
			/*	查询版本号指令--[0x72，0x68，0xbb,0x16] */
			if( (*(p+_usPos+1) == 0x68) && (*(p+_usPos+2) == 0xBB) && (*(p+_usPos+3) == 0x16) )
			{
				comSendBuf(COM1,ref_BUf,sizeof(ref_BUf));
			}
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名:	Analyze_Cst_Protocol
*	功能说明: 	串口协议分析，从串口接收缓冲区取数据并分析。
*	形    参:  	_ucBuf-数组 ， _uslen- 数组长度	, _ucPort:COM口（串口）
*	返 回 值: 	无STBV
*********************************************************************************************************
*/
static void Analyze_Cst_Protocol(uint8_t	* _ucBuf	,	uint16_t _uslen ,COM_PORT_E _ucPort)
{
	char * p;
	p = (char *)_ucBuf;
	if(strchr(p, 'S')	!=	NULL)	/* S开头的指令	*/
	{
		Analyze_S_Command(_ucBuf,_uslen,_ucPort);
	}
	if(strchr(p, 'T')	!=	NULL)	/*	T开头的指令	*/
	{
		Analyze_T_Command(_ucBuf,_uslen,_ucPort);
	}
	if(strstr(p, "BRC")	!=	NULL)	/*	B开头的指令	*/	
	{
		Analyze_B_Command(_ucBuf,_uslen);
	}
	if(strstr(p,"VER")	!=	NULL)	/*	查询版本号	*/
	{
		Analyze_VER_Command(_ucPort);
	}
}


/*
*********************************************************************************************************
*	函 数 名:	uart_protocol_handle
*	功能说明: 	串口取数据并进行协议分析，从串口接收缓冲区取数据并分析，被bsp.c调用
*					串口比网口多了OTA和可变波特率的功能。
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
void uart_protocol_handle(void)
{
	uint8_t ucData	=	0;
	uint8_t	ucDataEndFlag	=	0;
	static	uint8_t ucUartBuf[60];		//根据实际项目的指令长度来设置
	static uint16_t us_Uart1_Pos = 0;		//串口1 读指针
	
	while(1)
	{
		/*	从接收缓冲区读取1字节，非阻塞。1-有数据，0-无数据	*/
		if(comGetChar(COM1,&ucData) == 1)
		{
			if(ucDataEndFlag	==	0)
			{
				if (us_Uart1_Pos < sizeof(ucUartBuf))
				{
					ucUartBuf[us_Uart1_Pos++] = ucData;
					
					/*	常规指令，检测 '#'为结束符	*/
					if (ucData == '#')
					{
						Analyze_Cst_Protocol(ucUartBuf,us_Uart1_Pos-1,COM1);
						ucDataEndFlag	=	1;
					}
					/*	心跳包	*/
					if(CST_UART(ucData) == TRUE)
					{
						Analyze_CST_Command(COM1);
						ucDataEndFlag	=	1;
					}
					/*	bootloader相关的指令 */
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
				memset(ucUartBuf,0,sizeof(ucUartBuf));	/*	数组清零 */
				break;
			}
			continue;	/* 可能还有数据，继续分析 */
		}
		break;
	}	
}



/*
*********************************************************************************************************
*	函 数 名:	net_protocol_handle
*	功能说明: 	网口取数据并进行协议分析，从串口接收缓冲区取数据并分析，被bsp.c调用
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
void net_protocol_handle(void)
{
	uint8_t ucData	=	0;
	uint8_t	ucDataEndFlag	=	0;
	static	uint8_t ucNetBuf[60];		//根据实际项目的指令长度来设置

	static uint8_t _ucNCst	=	0;
	static uint16_t us_Uart3_Pos = 0;		//串口1 读指针
	
	while(1)
	{
		/*	从接收缓冲区读取1字节，非阻塞。1-有数据，0-无数据	*/
		if(comGetChar(COM3,&ucData)==1)
		{
			if(ucDataEndFlag	==	0)
			{
				if (us_Uart3_Pos < sizeof(ucNetBuf))
				{
					ucNetBuf[us_Uart3_Pos++] = ucData;
					
					/*	检测#	*/
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
			continue;	/* 可能还有数据，继续分析 */
		}
		break;
	}	
}
