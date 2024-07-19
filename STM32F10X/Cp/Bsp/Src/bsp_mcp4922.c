#include "bsp_mcp4922.h"

#include "bsp.h"


#define MCP4922_CS_1_H		(MCP4922_CS1_Port->BSRR	=	MCP4922_CS1_Pin);											//�ߵ�ƽ
#define MCP4922_CS_1_L		(MCP4922_CS1_Port->BSRR	=	(uint32_t)MCP4922_CS1_Pin << 16u);		//�͵�ƽ

#define MCP4922_CS_2_H		(MCP4922_CS2_Port->BSRR	=	MCP4922_CS2_Pin);											//�ߵ�ƽ
#define MCP4922_CS_2_L		(MCP4922_CS2_Port->BSRR	=	(uint32_t)MCP4922_CS2_Pin << 16u);		//�͵�ƽ

#define MCP4922_SCK_H			(MCP4922_SCK_Port->BSRR	=	MCP4922_SCK_Pin);											//�ߵ�ƽ
#define MCP4922_SCK_L			(MCP4922_SCK_Port->BSRR	=	(uint32_t)MCP4922_SCK_Pin << 16u);		//�͵�ƽ

#define MCP4922_LDAC_H		(MCP4922_LDAC_Port->BSRR	=	MCP4922_CS1_Pin);										//�ߵ�ƽ
#define MCP4922_LDAC_L		(MCP4922_LDAC_Port->BSRR	=	(uint32_t)MCP4922_CS1_Pin << 16u);	//�͵�ƽ

#define MCP4922_SDI_H			(MCP4922_SDI_Port->BSRR	=	MCP4922_SDI_Pin);											//�ߵ�ƽ
#define MCP4922_SDI_L			(MCP4922_SDI_Port->BSRR	=	(uint32_t)MCP4922_SDI_Pin << 16u);		//�͵�ƽ

#define MCP4922_Delay		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();


MCP4922_CS_LIST mcp4922_list[MCP4922_NUM]	=	{
	{MCP4922_CS1_Port,MCP4922_CS1_Pin},			/* Trig1  , �����ƽΪ0���͵�ƽ��*/
	{MCP4922_CS2_Port,MCP4922_CS2_Pin}			/* Trig2  , �����ƽΪ0���͵�ƽ��*/
};

/*	DAC�����¼���־λ	*/
uint8_t	g_DacUpdataFlag	=	0;

/* ʹ��GPIOʱ�� ����GPIOʱ�ӵ�ʵ��������	 */
#define MCP4922_GPIO_CLK_ENABLE() {	\
		__HAL_RCC_GPIOB_CLK_ENABLE();	\
	};

void MCP4922_Init(void)
{
	uint8_t	i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	MCP4922_GPIO_CLK_ENABLE();
	
	GPIO_InitStructure.Mode =	GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pin =	MCP4922_SCK_Pin|MCP4922_SDI_Pin|MCP4922_LDAC_Pin;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	/*	Gpio��ʼ�������ղ�ͬGPIO��Port���޸�*/
	HAL_GPIO_Init(MCP4922_SCK_Port, &GPIO_InitStructure);	  

	/*	--��ʼ��MCP4822��	CS��GPIO*/
	for(i=0;i	<	MCP4922_NUM;i++)
	{
		GPIO_InitStructure.Pin	=	mcp4922_list[i].pin	;
		HAL_GPIO_Init(mcp4922_list[i].gpio, &GPIO_InitStructure);	
	}
	
	bsp_DacUpdata();
}

/*
*********************************************************************************************************
*	�� �� ��: MCP4922_DAC_OutPut
*	����˵��: DACоƬ�����
*	��    ��: 	_ucMcpNum��DACоƬ���MCP4922_N_0��MCP4922_N_1�ȣ���bsp_mcp4922.h�ж���
								channel	�� 0--���ͨ��1	|		1--���ͨ��2
									_usData	:		12λDACֵ
*	�� �� ֵ: ����ֵ1 ��ʾ����(��ͨ����0��ʾδ���£��ͷţ�
*********************************************************************************************************
*/
static	void MCP4922_DAC_OutPut(uint8_t _ucMcpNum,	uint8_t channel, uint16_t _usData)//convert the 12 bit data
{
	char i;
	uint8_t	num;
	uint16_t _u12_Data;

	/*	--������ֵ��0-255ʱ����Ҫ���г���16	*/
	#if DATA_WIDTH_8_BIT
	
		if(_usData	<	256)
		{
			_usData	=	16	*	_usData;
		}
	#endif
	
	/*	--Ӳ�����趨��������Ϊ0.9V�������ֵ*12Լ����0.92412V*/
	#if	DATA_WIDTH_8_BIT_SPEC
		if(_usData	<	256)
		{
			_u12_Data	=	13*(_usData);
		}
		else if(_usData	>=	256)
		{
			_u12_Data	=	3060;
		}
	#endif

	//MCP4922_CS_L;	
	//�͵�ƽ
	mcp4922_list[_ucMcpNum].gpio->BSRR	=	(uint32_t)mcp4922_list[_ucMcpNum].pin	<<	16u;

	/*	�ж�CHANNEL���	*/
	if(channel==0)
		_u12_Data = _u12_Data|0x7000;
	else
		_u12_Data = _u12_Data|0xF000;
	
	
	for(i=0;i<=15;i++)
	{
		if(_u12_Data&0x8000)//MSB ?=1 or 0;
		{
			MCP4922_SDI_H;
		}
		else
		{
			MCP4922_SDI_L;
		}
		MCP4922_Delay;
		MCP4922_SCK_H;//read data SO
		MCP4922_Delay;
		_u12_Data<<=1;
		MCP4922_SCK_L;
		MCP4922_Delay
	}
	//MCP4922_CS_H;
	mcp4922_list[_ucMcpNum].gpio->BSRR	=	mcp4922_list[_ucMcpNum].pin;
	
	MCP4922_Delay;
	MCP4922_LDAC_L;
	MCP4922_Delay;
	MCP4922_LDAC_H;
}


static	uint16_t	getDacNum(uint16_t _usOutChannel)
{
	
	uint16_t	i	=	0;
	if(_usOutChannel	%	2	==	0)
	{
		i	=	_usOutChannel	/	2	;		//�õ���Ҫʹ�õ�DACоƬ����
		return	i;
	}
	else
	{
		i	=	(	_usOutChannel	/	2	)	+	1;
		return	i;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DacUpdata
*	����˵��: Dac���ݸ��±�Ǻ�����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DacUpdata(void)
{
	//if(g_DacUpdataFlag	==	0)
	g_DacUpdataFlag	=	2;
}

/*
*********************************************************************************************************
*	�� �� ��: checkDacUpdataEvent
*	����˵��: Dac���ݸ��±�Ǻ�����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint8_t checkDacUpdataEvent(void)
{
	if(g_DacUpdataFlag	!=	0)
	{
		g_DacUpdataFlag--;
		return TRUE;
	}
	else
	{
		return	FALSE;
	}
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_DacMcp4922Handler
*	����˵��: Dac���ƺ�������bsp.c����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DacMcp4922Handler(void)
{
	uint8_t	num	=	0;
	/*	���DAC�¼��Ƿ���Ҫ����	*/
	if(checkDacUpdataEvent()	==	TRUE)
	{
		/*	R_Channel	*/
		MCP4922_DAC_OutPut(MCP4922_N_1,0,g_CHx[LIGHT_CHANNEL_R].value);
		/*	G_Channel	*/
		MCP4922_DAC_OutPut(MCP4922_N_0,0,g_CHx[LIGHT_CHANNEL_G].value);
		/*	B_Channel	*/
		MCP4922_DAC_OutPut(MCP4922_N_0,1,g_CHx[LIGHT_CHANNEL_B].value);
		
	}
}



