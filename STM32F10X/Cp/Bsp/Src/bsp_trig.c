/*
*********************************************************************************************************
*
*	模块名称 : 触发驱动模块 (外部输入IO)
*	文件名称 : bsp_trig.c
*	版    本 : V1.0
*	说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*
*	修改记录 :
*		版本号  日期       		 作者     说明
*		V1.0    2024-05-01 		 alan  		
*
*********************************************************************************************************
*/
#include "bsp.h"


/*	
* 触发事件标志位		
* 1:发生 | 0:未发生（结束）
*/
uint8_t g_TrigFlag	=	0;

/*	--初始化所有GPIO时钟，按需删除	*/
#define	ALL_TRIG_GPIO_CLK_ENABLE()	{	\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
};


///*	定义触发输入的结构体	*/
//typedef struct
//{
//	GPIO_TypeDef* gpio;
//	uint16_t pin;
//	uint8_t ActiveLevel;		/*	触发电平	*/
//	//uint8_t	ExtiLine;				/*	中断线	*/
//}TRIG_GPIO_T;



/*	触发输入端口结构体	*/
TRIG_GPIO_T	g_trig_gpio_list[LIGHT_CHANNEL_HARD_NUM]	=	{
	{Trig_IN1_Port,Trig_IN1_Pin,0},			/* Trig1  , 激活电平为0（低电平）*/
	{Trig_IN2_Port,Trig_IN2_Pin,0},			/* Trig2  , 激活电平为0（低电平）*/
	{Trig_IN3_Port,Trig_IN3_Pin,0}			/* Trig3  , 激活电平为0（低电平）*/
};


/*
*********************************************************************************************************
*	函 数 名: bsp_InitTrigHard
*	功能说明: 硬件初始化触发端口，带中断配置，是否使用，还需待定
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTrigHard(void)
{	
	GPIO_InitTypeDef gpio_init;
	uint8_t i;

	/* 第1步：打开GPIO时钟 */
	ALL_TRIG_GPIO_CLK_ENABLE();
	
	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;  /* 中断输入 */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  			/* GPIO速度等级 */
  	gpio_init.Pull = GPIO_NOPULL;
	
	for (i = 0; i < LIGHT_CHANNEL_HARD_NUM; i++)
	{
		gpio_init.Pin = g_trig_gpio_list[i].pin;
		HAL_GPIO_Init(g_trig_gpio_list[i].gpio, &gpio_init);
	}
	
	/*	配置中断，这里还需待定一下*/	
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}



/*
*********************************************************************************************************
*	函 数 名: TrigActive
*	功能说明: 判断触发口是否被外部触发
*	形    参: _id
*	返 回 值: 返回值1 表示有触发，0表示无触发
*********************************************************************************************************
*/
static uint8_t TrigActive(uint8_t _id)
{
	uint8_t level;
	
	if ((g_trig_gpio_list[_id].gpio->IDR & g_trig_gpio_list[_id].pin) == 0)
	{
		level = 0;
	}
	else
	{
		level = 1;
	}

	if (level == g_trig_gpio_list[_id].ActiveLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: IsTrigFunc
*	功能说明: 判断触发输入是否发生
*	形    参: 无
*	返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static uint8_t IsTrigFunc(uint8_t _id)
{
	/* 小于等于触发通道数 */
	if (_id <= LIGHT_CHANNEL_HARD_NUM)
	{
		if (TrigActive(_id)) 
		{
			return 1;		
		}
		return 0;
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_DetectTrig
*	功能说明: 检测触发输入。非阻塞状态，必须被周期性的调用。
*	形    参: IO的id， 从0开始编码
*	返 回 值: 1-有触发输入	，	0-无触发输入
*********************************************************************************************************
*/
uint8_t bsp_DetectTrig(uint8_t	_id)
{
	LIGHT_DATA *pBtn;
	pBtn = &g_CHx[_id];
	
	/*	检测是否有触发输入	*/
	if (_id < LIGHT_CHANNEL_HARD_NUM)
	{
		if (IsTrigFunc(_id))
		{
			if (pBtn->TrigState == 0)
			{
				/*	有触发输入	*/
				pBtn->TrigState = TRIG_ON;
				return	1;
			}
		}
		else
		{
			if (pBtn->TrigState == 1)
			{
				/*	无触发输入	*/
				pBtn->TrigState = TRIG_OFF;
				return	0;
			}
		}
	}
	
}




/*
*********************************************************************************************************
*	函 数 名: bsp_LightTrigHandler
*	功能说明: 光源触发处理函数，属于二次执行，确保光源稳定输出，在bsp.h中被调用。
*	形    参:	无	
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_TrigHandler(void)
{
	if(g_TrigFlag	==	TRIG_ON)
	{
	/*	---常亮模式---*/
		if(g_CtrlMode	==	MODE_H)
		{

			/*	--轮询查询触发通道--	*/
			for(uint8_t	i=0;i<=LIGHT_CHANNEL_HARD_NUM;i++)
			{
				/*	再次检测触发输入状态	*/
				//bsp_DetectTrig(i);
				if(bsp_DetectTrig(i) == TRUE)
				{
					if(g_CHx[i].TrigState	==	TRIG_ON)
					{
						/*	-有触发，光源关闭	*/
						g_CHx[i].State	=	LIGHT_OFF;
					}
					else
					{
						
						/*	-无触发，光源打开	*/
						g_CHx[i].State	=	LIGHT_ON;
					}
				}
			}
		}
		/*	---常灭模式---	*/
		if(g_CtrlMode	==	MODE_L)
		{
			/*	--轮询查询触发通道--	*/
			for(uint8_t	j=0;j<=LIGHT_CHANNEL_HARD_NUM;j++)
			{
				/*	再次检测触发输入状态	*/
				//bsp_DetectTrig(j);
				if(bsp_DetectTrig(j) == TRUE)
				{
					if(g_CHx[j].TrigState	==	TRIG_ON)
					{
						/*	-有触发，光源开启	*/
						g_CHx[j].State	=	LIGHT_ON;
					}
					else
					{
						/*	-无触发，光源关闭	*/
						g_CHx[j].State	=	LIGHT_OFF;
					}
				}
			}
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: HAL_GPIO_EXTI_Callback
*	功能说明: 
*	形    参: 开启	-	1   ；  关闭	-	0		
*	返 回 值: 无       
*********************************************************************************************************
*/
//void	HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	for(uint8_t i	=	0;	i<LIGHT_CHANNEL_HARD_NUM;	i++)
//	{
//		if(g_trig_gpio_list[i].pin	==	GPIO_Pin)
//		{
//			/*	---检测是否有触发输入	*/
//			if(bsp_DetectTrig(i))
//			{
//				/*	触发标志位set	*/
//				g_TrigFlag	=	TRIG_ON;
//				g_CHx[i].TrigState	=	TRIG_ON;
//				/*	快速处理函数	*/
//				//bsp_LightQuickCtrlTrigON(i);
//			}
//			else
//			{
//				g_TrigFlag	=	TRIG_OFF;
//				g_CHx[i].TrigState	=	TRIG_OFF;
//				/*	快速处理函数	*/
//				//bsp_LightQuickCtrlTrigOFF(i);
//			}
//		}
//	}	
//}

/*
*********************************************************************************************************
*	函 数 名: bsp_trig_fun
*	功能说明: 光源控制+触发处理函数，在主循环和中断触发中被调用。
*	形    参: 无	
*	返 回 值: 无       
*********************************************************************************************************
*/
void bsp_trig_fun(void)
{
	switch(g_CtrlMode)
	{
		case MODE_H :
			for(uint8_t i	=	0;	i<LIGHT_CHANNEL_HARD_NUM;	i++)
			{
					/*	有触发	*/
					if((g_trig_gpio_list[i].gpio->IDR & g_trig_gpio_list[i].pin) == 0)
					{
						bsp_LightChannel_OFF(i);
					}
					else if(((g_trig_gpio_list[i].gpio->IDR & g_trig_gpio_list[i].pin) != 0) && ( g_CHx[i].value != 0 ))
					{
						bsp_LightChannel_ON(i);
					}
					else
					{
						bsp_LightChannel_OFF(i);
					}
			}
		break;
			
		case MODE_L :
			for(uint8_t i	=	0;	i<LIGHT_CHANNEL_HARD_NUM;	i++)
			{
					/*	有触发	*/
					if(((g_trig_gpio_list[i].gpio->IDR & g_trig_gpio_list[i].pin) == 0) &&  ( g_CHx[i].value != 0 ) )
					{
						bsp_LightChannel_ON(i);
					}
					else
					{
						bsp_LightChannel_OFF(i);
					}
			}
			break;
	}
}


