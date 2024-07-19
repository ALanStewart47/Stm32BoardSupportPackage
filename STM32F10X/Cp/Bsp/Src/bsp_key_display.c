/*
*********************************************************************************************************
*
*	模块名称 : 按键显示模块
*	文件名称 : bsp_key_display.c
*	版    本 : V1.0
*	说    明 : 按键和显示业务
*	修改记录 :
*		版本号  日期       	作者    说明
*		V1.0    2024-05-28 	Alans  	正式发布
*
*********************************************************************************************************
*/
#include "bsp.h"

/************************************
显示页面变量
		=	0 						: 	第一页，控制模式页面
		=	1 ~ PAGE_MAX 	：	第二页，通道页面
*************************************/
uint8_t	g_DisPlayPage	=	0;

uint8_t	g_test_DisPlayPage	=	0;


/*
*********************************************************************************************************
*	函 数 名:		bsp_ChangeDisPlayPage
*	功能说明: 	改变显示页面， 显示页面会被PAGE_MAX限制
*	形    参:  	_usChannel
*	返 回 值: 	无
*********************************************************************************************************
*/
void bsp_ChangeDisPlayPage(uint16_t	_usChannel)
{
	if(_usChannel	>=	PAGE_MAX)
	{
		g_DisPlayPage	=	PAGE_MAX;
	}
	else
	{
		g_DisPlayPage	=	_usChannel;
	}
	bsp_DacUpdata();
}


/*
*********************************************************************************************************
*	函 数 名:		goToNextPage
*	功能说明: 	页面数+1，带回到第一页功能，该函数被app_Key_Handler()调用
							通过结构体g_CHx 获取通道数，和亮度值
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void bsp_goToNextPage(void)
{
	/*	增加一页	*/
	g_DisPlayPage++;
	
	/*	当页数大于 最大页面数 ，回到第一页*/
	if(g_DisPlayPage >= PAGE_MAX)
	{
		g_DisPlayPage	=	0;
	}
}


/*
*********************************************************************************************************
*	函 数 名:		bsp_ChannelDisplay
*	功能说明: 	数码管显示通道和亮度值	,	AXXX   A-通道数，XXX-亮度值 
							通过结构体g_CHx 获取通道数，和亮度值
*	形    参:  	_usDisplayPage--输入g_DisPlayPage(此刻显示的页面)
*	返 回 值: 	无
*********************************************************************************************************
*/
static	void bsp_ChannelDisplay(uint16_t _usDisplayPage)
{
	uint8_t	ch	=	0;	//通道序号
	
	ch	=	(	_usDisplayPage -	1	);	//通道序号	=	显示页面	-	1
	
	static uint8_t s_DisBuff[4]	=	{0,0,0,0};
	
	if(g_CHx[ch].channel	>	(LIGHT_CHANNEL_NUM)	)
	{
		return;
	}	
	else
	{
		s_DisBuff[0]	=	(g_CHx[ch].channel)%10			;		//通道值
		s_DisBuff[1]	=	(g_CHx[ch].value)%1000/100	;		//亮度值
		s_DisBuff[2]	=	(g_CHx[ch].value)%100/10		;		//
		s_DisBuff[3]	=	(g_CHx[ch].value)%10				;		//

		/*	显示数码管	*/
		Tube_DisNum(s_DisBuff);
	}
}


/*
*********************************************************************************************************
*	函 数 名:		bsp_Mode_HL_Display
*	功能说明: 	数码管显示模式状态,H__X   X=0:常灭，X=1:常亮  
							用全局变量g_CtrlMode 判断处于什么状态
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void bsp_Mode_HL_Display(void)
{
	/*	数组储存H__的字符	*/
	static uint8_t s_DisBuff[4]={12,15,15,0};
	//常灭模式
	if(g_CtrlMode == 0)		
	{
		s_DisBuff[3]	=	0;	
	}
	//常亮模式
	else
	{
		s_DisBuff[3]	=	1;
	}
	/*	显示数码管	*/
	Tube_DisNum(s_DisBuff);
}



/*
*********************************************************************************************************
*	函 数 名:		bsp_DisplayHandler
*	功能说明: 	数码管显示的核心函数，调用即可实现数码管显示功能。该函数通过判断
							全局变量g_DisplayPage来判断显示哪个页面。该函数被bsp.c调用。
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
void bsp_DisplayHandler(void)
{
	for(uint8_t i	=	0;i<PAGE_MAX;i++)
	{
		if(g_DisPlayPage == i)
		{
			/*	页面1时，显示控制状态	*/
			if(g_DisPlayPage	==	0)
			{
				/*	显示控制模式	*/
				bsp_Mode_HL_Display();
			}
			else{
				/*	显示通道和亮度值	*/
				bsp_ChannelDisplay(i);
			}
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名:   bsp_ErrorDisplay
*	功能说明: 	数码管显示错误，内容为ERRx,x为数字
*	形    参:  	错误数字（0-9）
*	返 回 值: 	无
*********************************************************************************************************
*/
uint8_t bsp_ErrorDisplay(void)
{
	uint8_t i = 1 ;
	if(g_ErrFlag == i)
    {
        bsp_DisplayErrAip650(i);
        return TRUE;
    }
	i++;
    return FALSE;
}

/*
*********************************************************************************************************
*	函 数 名: app_Key_Handler
*	功能说明: 从按键FIFO中获取按键键值，执行对应按键的业务，后续有功能新增按
							实际情况增加业务代码。该函数被bsp.c调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void app_Key_Handler(void)
{
	uint8_t ucKeyCode;		/* 按键代码 */
	
	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
	ucKeyCode = bsp_GetKey();	
	
	if (ucKeyCode != KEY_NONE)
	{
		switch (ucKeyCode)
		{	
			/* K1--功能键按下 */
			case KEY_DOWN_K1:			
				bsp_goToNextPage();		//切换按键页面
				break;

			/* K2  +键按下 */
			case KEY_DOWN_K2:	
				if(g_DisPlayPage	==	0)	//处于模式显示页面
				{
					bsp_KeyIncMode();		//切换控制模式
				}
				else
				{
					bsp_KeyIncLightValue(g_DisPlayPage);		//增加亮度值
				}
					
				break;

			/* K3		-键按下	*/
			case KEY_DOWN_K3:	
				if(g_DisPlayPage	==	0)	//处于模式显示页面
				{
					bsp_KeyDecMode();		//切换控制模式
				}
				else
				{
					bsp_KeyDecLightValue(g_DisPlayPage);		//减少亮度值
				}
				break;


			default:
				/* 其它的键值不处理 */
				break;
	
		}
	}
}


