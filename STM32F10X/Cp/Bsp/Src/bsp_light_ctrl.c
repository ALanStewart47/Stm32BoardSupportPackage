/*
*********************************************************************************************************
*
*	模块名称 : 光源参数管理和控制模块
*	文件名称 : bsp_light_ctrl.c
*	版    本 : V1.0
*	说    明 : 包含所有光源通道的亮度值，控制模式，和控制开关和触发控制等函数
*	修改记录 :
*		版本号  日期       	作者    说明
*		V1.0    2024-05-28 	Alans  	正式发布
*
*********************************************************************************************************
*/
#include "bsp.h"


/*********************************************************
*	----IO操作函数----	
**********************************************************/
/*	--光源通道开关	*/
#define	Switch_R_ON		(Switch_R_Port->BSRR	=	Switch_R_Pin);										//高电平
#define	Switch_R_OFF	(Switch_R_Port->BSRR = (uint32_t)Switch_R_Pin << 16u;);		//低电平

#define	Switch_G_ON		(Switch_G_Port->BSRR	=	Switch_G_Pin);
#define	Switch_G_OFF	(Switch_G_Port->BSRR = (uint32_t)Switch_G_Pin << 16u;);

#define	Switch_B_ON		(Switch_B_Port->BSRR	=	Switch_B_Pin);
#define	Switch_B_OFF	(Switch_B_Port->BSRR = (uint32_t)Switch_B_Pin << 16u;);

/*	--光源电源开关通道控制	*/
#define	Power_R_ON		(Power_R_Port->BSRR	=	Power_R_Pin);											//高电平
#define	Power_R_OFF		(Power_R_Port->BSRR = (uint32_t)Power_R_Pin << 16u;);		//低电平

#define	Power_G_ON		(Power_G_Port->BSRR	=	Power_G_Pin);
#define	Power_G_OFF		(Power_G_Port->BSRR = (uint32_t)Power_G_Pin << 16u;);

#define	Power_B_ON		(Power_B_Port->BSRR	=	Power_B_Pin);
#define	Power_B_OFF		(Power_B_Port->BSRR = (uint32_t)Power_B_Pin << 16u;);

/*	--触发输入引脚电平读取	*/
//#define		Trig_R_ON			((Trig_IN1_Port->IDR & Trig_IN1_Pin)	==	(uint32_t)GPIO_PIN_RESET);	//读到低电平=有触发输入
//#define		Trig_R_OFF		((Trig_IN1_Port->IDR & Trig_IN1_Pin)	!=	(uint32_t)GPIO_PIN_RESET);	//读到高电平=无触发输入

//#define		Trig_G_ON			((Trig_IN2_Port->IDR & Trig_IN2_Pin)	==	(uint32_t)GPIO_PIN_RESET);
//#define		Trig_G_OFF		((Trig_IN2_Port->IDR & Trig_IN2_Pin)	!=	(uint32_t)GPIO_PIN_RESET);

//#define		Trig_B_ON			((Trig_IN3_Port->IDR & Trig_IN3_Pin)	==	(uint32_t)GPIO_PIN_RESET);
//#define		Trig_B_OFF		((Trig_IN3_Port->IDR & Trig_IN3_Pin)	!=	(uint32_t)GPIO_PIN_RESET);



/*	-光源结构体,数组前四位需要根据实际情况填入，后几位可以任意填，之后会被初始化掉*/
LIGHT_DATA g_CHx[LIGHT_CHANNEL_NUM] = 
{
	{Power_R_Port,Power_R_Pin,Switch_R_Port,Switch_R_Pin,0,0,0,0},
	{Power_G_Port,Power_G_Pin,Switch_G_Port,Switch_G_Pin,0,0,0,0},
	{Power_B_Port,Power_B_Pin,Switch_B_Port,Switch_B_Pin,0,0,0,0}
};


/**	
 * --控制模式标志位		
 * 1:常亮模式		
 * 0:常灭模式	
**/
uint8_t g_CtrlMode	=	0;
/**	
 * --错误标志位		
 * 1:通道过流		
 * 2:掉电错误
 * 在enum类型ERROR_STATE中	
**/
uint8_t g_ErrFlag	=	0;  

uint8_t g_ErrNum	=	0;

/*
*********************************************************************************************************
*	函 数 名: bsp_ModeCtrl
*	功能说明: 模式控制函数，被bsp_uart_protocol.c调用
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t	bsp_ModeCtrl(uint8_t	_ucMode)
{
	if((_ucMode	!=	MODE_H) && (_ucMode	!=	MODE_L))
	{
		return FALSE;
	}
	else
	{
		g_CtrlMode	=	_ucMode;
		return TRUE;
	}
}




/*
*********************************************************************************************************
*	函 数 名: bsp_InitLightVar
*	功能说明: 初始化光源通道变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitLightVar(void)
{
	if(bsp_CheckFlashSucess() == FALSE)
	{
		g_CtrlMode	=	MODE_H;		//上电默认是常亮状态
	}


	/* 给每个光源通道结构体成员变量赋一组缺省值 */
	for (uint8_t i = 0; i < LIGHT_CHANNEL_NUM ; i++)
	{
		if(bsp_CheckFlashSucess() == FALSE)
		{
			g_CHx[i].State	=	LIGHT_OFF	;		
			g_CHx[i].value		=	0;	
		}
		/*	亮度值 ，初次上电默认为0	*/
		g_CHx[i].TrigState	=	TRIG_OFF;	/*	触发状态：上电默认-无触发*/
	}
	
	for(uint8_t j =1;j	<	(LIGHT_CHANNEL_NUM	+	1	);	j++)
	{
		g_CHx[j-1].channel		=	j;		/*		通道数		*/
	}
}



static void bsp_InitLightHard(void)
{
  HAL_GPIO_WritePin(GPIOA, PowerCTR_G_Pin|PowerCTR_B_Pin|PowerCTR_R_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, SwCTR_R_Pin|SwCTR_G_Pin|SwCTR_B_Pin, GPIO_PIN_SET);
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitLight
*	功能说明: 初始化光源通道
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLight(void)
{
	bsp_InitLightVar();
	bsp_InitLightHard();
	
}


/*
*********************************************************************************************************
*	函 数 名: bsp_SetBrightness
*	功能说明: 增加对应通道的亮度值,到最大值后保持最大值
*	形    参:  ch-光源通道
*	返 回 值: 	TRUE-成功			FALSE-失败         
*********************************************************************************************************
*/
int bsp_SetBrightness(uint8_t _ucChannel	,	uint16_t	_usValue)
{
	/*	通道数	*/
	if(_ucChannel	<=	LIGHT_CHANNEL_NUM)
	{
		/*	通道亮度值不超过最大值	*/
		if(_usValue	>	LIGHT_VALUE_MAX)
		{
			_usValue	=	LIGHT_VALUE_MAX;
			return TRUE;
		}
		else
		{
			g_CHx[_ucChannel].value	=	  _usValue;
			return TRUE;
		}
		bsp_DacUpdata();
	}
	else
	{
		bsp_DacUpdata();
		return FALSE;
	}
}



/*
*********************************************************************************************************
*	函 数 名: bsp_IncLightValue
*	功能说明: 增加对应通道的亮度值,到最大值后保持最大值
*	形    参:  ch-光源通道
*	返 回 值: 	0-成功			1-失败         
*********************************************************************************************************
*/
int bsp_KeyIncLightValue(const uint16_t _usDisPlayPage)
{
	uint16_t ch	=	0;	//通道数
	
	ch	=	_usDisPlayPage	-	1;	//光源通道数	= 显示页面 - 1 
	
	if(	(ch	<	LIGHT_CHANNEL_NUM))
	{
		if(g_CHx[ch].value	>=	LIGHT_VALUE_MAX)
		{
			g_CHx[ch].value	=	LIGHT_VALUE_MAX;
			/*	DAC刷新事件	*/
			bsp_DacUpdata();
			return TRUE;
		}
		else
		{
			g_CHx[ch].value	++;
			
			/*	DAC刷新事件	*/
			bsp_DacUpdata();
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_DecLightValue
*	功能说明: 增加对应通道的亮度值,到最大值后保持最大值
*	形    参:  ch-光源通道
*	返 回 值: 	0-成功			1-失败         
*********************************************************************************************************
*/
int bsp_KeyDecLightValue(const uint16_t _usDisPlayPage)
{
	uint16_t ch	=	0;	//通道数
	
	ch	=	_usDisPlayPage	-	1;	//光源通道数	= 显示页面 - 1 
	
	if(	(ch<LIGHT_CHANNEL_NUM) )
	{
		if(g_CHx[ch].value	<=	LIGHT_VALUE_MIN)
		{
			g_CHx[ch].value	=	LIGHT_VALUE_MIN;
			
			/*	DAC刷新事件	*/
			bsp_DacUpdata();
			return 0;
		}
		else
		{
			g_CHx[ch].value	--;
			
			/*	DAC刷新事件	*/
			bsp_DacUpdata();
			return 0;
		}
	}
	else
	{
		return -1;
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_KeyIncMode
*	功能说明: 控制模式+1 ，切为1
*	形    参:  ch-光源通道
*	返 回 值: 	0-成功			1-失败         
*********************************************************************************************************
*/
void bsp_KeyIncMode(void)
{
	if(g_CtrlMode == 0){
		g_CtrlMode	=	1;
		bsp_DacUpdata();
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_KeyDecMode
*	功能说明: 控制模式-1 ，切为0
*	形    参:  ch-光源通道
*	返 回 值: 	0-成功			1-失败         
*********************************************************************************************************
*/
void bsp_KeyDecMode(void)
{
	if(g_CtrlMode == 1){
		g_CtrlMode	=	0;
		bsp_DacUpdata();
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LightCtrl
*	功能说明: 光源开关状态修改，带光源亮度值判断-亮度值为0关闭光源
*	形    参: _ucChannel-通道				_ucONOFF-	开启-1   ；  关闭-0		
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightCtrl(uint8_t	_ucChannel,uint8_t	_ucONOFF)
{
	LIGHT_DATA	*pBtn	;
	pBtn	=	&g_CHx[_ucChannel];
	
	/*	开启光源通道	*/
	if(_ucONOFF	==	LIGHT_ON)
	{
		if(pBtn->State	==	LIGHT_ON)
		{
		}
		else
		{
			
			pBtn->State	=	LIGHT_ON;
		}
	}
	/*	关闭光源通道	*/
	else 	/*pBtn	==	LIGHT_OFF	*/
	{
		if(pBtn->State	==	LIGHT_ON)
		{
			pBtn->State	=	LIGHT_OFF;
		}
	}
	/*	光源通道为0，判定为关闭光源	*/
	if(pBtn->value	 == 0)
	{
		pBtn->State	=	LIGHT_OFF;
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_LightQuickCtrlTrigON
*	功能说明: 快速响应触发输入，不判断触发状态，本函数仅用于发生触发时，被中断函数调用
*	形    参: _usCh		光源结构体序号
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightQuickCtrlTrigON(uint16_t _usCh)
{
	if(g_CtrlMode	==	MODE_H)
	{
		g_CHx[_usCh].State	=	LIGHT_OFF;
		g_CHx[_usCh].power_gpio->BSRR = (uint32_t)g_CHx[_usCh].power_pin << 16u;			//关闭光源电源
		g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//关闭通道
	}
	else
	{
		g_CHx[_usCh].State	=	LIGHT_ON;
		g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//开启光源电源
		g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin << 16u;	//开启通道
	}
	
}


/*
*********************************************************************************************************
*	函 数 名: bsp_LightQuickCtrlTrigOFF
*	功能说明: 快速响应触发断开，不判断触发状态，本函数仅用于发生触发时，被中断函数调用
*	形    参: _usCh		光源结构体序号
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightQuickCtrlTrigOFF(uint16_t _usCh)
{
	if(g_CtrlMode	==	MODE_H)
	{
		g_CHx[_usCh].State	=	LIGHT_ON;
		g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//开启光源电源
		g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//开启通道
	}
	else
	{
		g_CHx[_usCh].State	=	LIGHT_OFF;
		g_CHx[_usCh].power_gpio->BSRR = (uint32_t)g_CHx[_usCh].power_pin << 16u;			//关闭光源电源
		g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin << 16u;	//关闭通道
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_LightChannel_ON
*	功能说明: 光源打开函数。
*	形    参: _usCh：通道数。 0：通道1 ，1：通道2	，2：通道3
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightChannel_ON(uint16_t	_usCh)
{
	g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//开启光源电源
	g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin	<< 16u;		//关闭通道
}


/*
*********************************************************************************************************
*	函 数 名: bsp_LightChannel_OFF
*	功能说明: 光源关闭函数，
*	形    参: _usCh：通道数。 0：通道1 ，1：通道2	，2：通道3
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightChannel_OFF(uint16_t	_usCh)
{
	g_CHx[_usCh].power_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].power_pin	<< 16u;				//关闭光源电源
	g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//开启通道
}


static void bsp_All_LightChannel_off(void)
{
	for(uint8_t i = 0;i < LIGHT_CHANNEL_HARD_NUM;i++)
	{
		bsp_LightChannel_OFF(i);
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_LightCtrlHandler
*	功能说明: 光源控制函数。需要轮询查询，和执行，被bsp.c调用。
							state为1-开启光源，state为0-关闭光源
*	形    参: 无
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_LightCtrlHandler(void)
{
	for(uint8_t i = 0; i < LIGHT_CHANNEL_HARD_NUM	;	i++)
	{
		if(g_CHx[i].State	==	LIGHT_ON)
		{
			if(g_CHx[i].value	!=	0)
			{
				bsp_LightChannel_ON(i);
			}
			else
				break;
		}
		else
		{
			if(g_CHx[i].State	==	LIGHT_OFF)
			{
				bsp_LightChannel_OFF(i);
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectLightState
*	功能说明: 光源状态检测函数。需要轮询查询，和执行，被bsp.c调用。
*	形    参: 无
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_DetectLightState(void)
{
	for(uint8_t i = 0; i < LIGHT_CHANNEL_HARD_NUM	;	i++)
	{
		if(g_CHx[i].TrigState == TRIG_OFF)
		{
			if(g_CtrlMode == MODE_H)
			{
				if(g_CHx[i].value != 0)
				{
					g_CHx[i].State	=	LIGHT_ON;
				}
				else
				{
						g_CHx[i].State	=	LIGHT_OFF;
				}
			}
			else if( (g_CtrlMode == MODE_L) ||  (g_CHx[i].value == 0 ))
			{
				
				g_CHx[i].State	=	LIGHT_OFF;
			}
		}
	}
}




/*
*********************************************************************************************************
*	函 数 名: bsp_ErrorHandler
*	功能说明: 错误码处理函数。需要轮询执行，被bsp.c调用。
*	形    参: 无
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_ErrorHandler(void)
{
	static uint8_t i = 0;
	/*	通道过流	*/
	if(	(g_ErrFlag  <= B_OCP_FLAG)  && (g_ErrFlag != 0))
	{
		/*	关闭通道 */
		bsp_All_LightChannel_off();
		/*	保存数据	*/
		bsp_SaveFlashData();
		bsp_DisplayErrAip650(g_ErrFlag);
		
		/*	关闭中断	*/
		DISABLE_INT();
		while(1)
		{
			/*	关闭中断	*/
			DISABLE_INT();
			/*	喂狗	*/
			bsp_feedDog();
		}
	}
	i++;
	if((g_ErrFlag == 4 )&& (g_ErrNum == 0))
	{
		g_ErrNum = 1;
		
		bsp_All_LightChannel_off();
		
		bsp_SaveFlashData();
		#if DEBUGMODE
			printf("Power Down \r\n");
			Error_Handler(__FILE__, __LINE__);
		#endif
		
		/*	复位 */
		//NVIC_SystemReset();
	}
}

