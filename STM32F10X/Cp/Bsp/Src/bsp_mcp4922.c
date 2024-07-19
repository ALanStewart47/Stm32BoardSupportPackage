#include "bsp_mcp4922.h"

#include "bsp.h"


#define MCP4922_CS_1_H		(MCP4922_CS1_Port->BSRR	=	MCP4922_CS1_Pin);											//高电平
#define MCP4922_CS_1_L		(MCP4922_CS1_Port->BSRR	=	(uint32_t)MCP4922_CS1_Pin << 16u);		//低电平

#define MCP4922_CS_2_H		(MCP4922_CS2_Port->BSRR	=	MCP4922_CS2_Pin);											//高电平
#define MCP4922_CS_2_L		(MCP4922_CS2_Port->BSRR	=	(uint32_t)MCP4922_CS2_Pin << 16u);		//低电平

#define MCP4922_SCK_H			(MCP4922_SCK_Port->BSRR	=	MCP4922_SCK_Pin);											//高电平
#define MCP4922_SCK_L			(MCP4922_SCK_Port->BSRR	=	(uint32_t)MCP4922_SCK_Pin << 16u);		//低电平

#define MCP4922_LDAC_H		(MCP4922_LDAC_Port->BSRR	=	MCP4922_CS1_Pin);										//高电平
#define MCP4922_LDAC_L		(MCP4922_LDAC_Port->BSRR	=	(uint32_t)MCP4922_CS1_Pin << 16u);	//低电平

#define MCP4922_SDI_H			(MCP4922_SDI_Port->BSRR	=	MCP4922_SDI_Pin);											//高电平
#define MCP4922_SDI_L			(MCP4922_SDI_Port->BSRR	=	(uint32_t)MCP4922_SDI_Pin << 16u);		//低电平

#define MCP4922_Delay		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();


MCP4922_CS_LIST mcp4922_list[MCP4922_NUM]	=	{
	{MCP4922_CS1_Port,MCP4922_CS1_Pin},			/* Trig1  , 激活电平为0（低电平）*/
	{MCP4922_CS2_Port,MCP4922_CS2_Pin}			/* Trig2  , 激活电平为0（低电平）*/
};

/*	DAC更新事件标志位	*/
uint8_t	g_DacUpdataFlag	=	0;

/* 使能GPIO时钟 根据GPIO时钟的实际情况添加	 */
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
	/*	Gpio初始化，按照不同GPIO的Port来修改*/
	HAL_GPIO_Init(MCP4922_SCK_Port, &GPIO_InitStructure);	  

	/*	--初始化MCP4822的	CS的GPIO*/
	for(i=0;i	<	MCP4922_NUM;i++)
	{
		GPIO_InitStructure.Pin	=	mcp4922_list[i].pin	;
		HAL_GPIO_Init(mcp4922_list[i].gpio, &GPIO_InitStructure);	
	}
	
	bsp_DacUpdata();
}

/*
*********************************************************************************************************
*	函 数 名: MCP4922_DAC_OutPut
*	功能说明: DAC芯片输出，
*	形    参: 	_ucMcpNum：DAC芯片序号MCP4922_N_0、MCP4922_N_1等，在bsp_mcp4922.h中定义
								channel	： 0--输出通道1	|		1--输出通道2
									_usData	:		12位DAC值
*	返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static	void MCP4922_DAC_OutPut(uint8_t _ucMcpNum,	uint8_t channel, uint16_t _usData)//convert the 12 bit data
{
	char i;
	uint8_t	num;
	uint16_t _u12_Data;

	/*	--当亮度值是0-255时，需要进行乘以16	*/
	#if DATA_WIDTH_8_BIT
	
		if(_usData	<	256)
		{
			_usData	=	16	*	_usData;
		}
	#endif
	
	/*	--硬件上设定了最大输出为0.9V，故最大值*12约等于0.92412V*/
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
	//低电平
	mcp4922_list[_ucMcpNum].gpio->BSRR	=	(uint32_t)mcp4922_list[_ucMcpNum].pin	<<	16u;

	/*	判断CHANNEL序号	*/
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
		i	=	_usOutChannel	/	2	;		//得到需要使用的DAC芯片数量
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
*	函 数 名: bsp_DacUpdata
*	功能说明: Dac数据更新标记函数。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DacUpdata(void)
{
	//if(g_DacUpdataFlag	==	0)
	g_DacUpdataFlag	=	2;
}

/*
*********************************************************************************************************
*	函 数 名: checkDacUpdataEvent
*	功能说明: Dac数据更新标记函数。
*	形    参:  无
*	返 回 值: 无
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
*	函 数 名: bsp_DacMcp4922Handler
*	功能说明: Dac控制函数，被bsp.c调用
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DacMcp4922Handler(void)
{
	uint8_t	num	=	0;
	/*	检查DAC事件是否需要更新	*/
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



