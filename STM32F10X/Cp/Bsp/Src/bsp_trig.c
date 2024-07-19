/*
*********************************************************************************************************
*
*	ģ������ : ��������ģ�� (�ⲿ����IO)
*	�ļ����� : bsp_trig.c
*	��    �� : V1.0
*	˵    �� : ɨ�������������������˲����ƣ����а���FIFO�����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*
*	�޸ļ�¼ :
*		�汾��  ����       		 ����     ˵��
*		V1.0    2024-05-01 		 alan  		
*
*********************************************************************************************************
*/
#include "bsp.h"


/*	
* �����¼���־λ		
* 1:���� | 0:δ������������
*/
uint8_t g_TrigFlag	=	0;

/*	--��ʼ������GPIOʱ�ӣ�����ɾ��	*/
#define	ALL_TRIG_GPIO_CLK_ENABLE()	{	\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
};


///*	���崥������Ľṹ��	*/
//typedef struct
//{
//	GPIO_TypeDef* gpio;
//	uint16_t pin;
//	uint8_t ActiveLevel;		/*	������ƽ	*/
//	//uint8_t	ExtiLine;				/*	�ж���	*/
//}TRIG_GPIO_T;



/*	��������˿ڽṹ��	*/
TRIG_GPIO_T	g_trig_gpio_list[LIGHT_CHANNEL_HARD_NUM]	=	{
	{Trig_IN1_Port,Trig_IN1_Pin,0},			/* Trig1  , �����ƽΪ0���͵�ƽ��*/
	{Trig_IN2_Port,Trig_IN2_Pin,0},			/* Trig2  , �����ƽΪ0���͵�ƽ��*/
	{Trig_IN3_Port,Trig_IN3_Pin,0}			/* Trig3  , �����ƽΪ0���͵�ƽ��*/
};


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitTrigHard
*	����˵��: Ӳ����ʼ�������˿ڣ����ж����ã��Ƿ�ʹ�ã��������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitTrigHard(void)
{	
	GPIO_InitTypeDef gpio_init;
	uint8_t i;

	/* ��1������GPIOʱ�� */
	ALL_TRIG_GPIO_CLK_ENABLE();
	
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;  /* �ж����� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  			/* GPIO�ٶȵȼ� */
  	gpio_init.Pull = GPIO_NOPULL;
	
	for (i = 0; i < LIGHT_CHANNEL_HARD_NUM; i++)
	{
		gpio_init.Pin = g_trig_gpio_list[i].pin;
		HAL_GPIO_Init(g_trig_gpio_list[i].gpio, &gpio_init);
	}
	
	/*	�����жϣ����ﻹ�����һ��*/	
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}



/*
*********************************************************************************************************
*	�� �� ��: TrigActive
*	����˵��: �жϴ������Ƿ��ⲿ����
*	��    ��: _id
*	�� �� ֵ: ����ֵ1 ��ʾ�д�����0��ʾ�޴���
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
*	�� �� ��: IsTrigFunc
*	����˵��: �жϴ��������Ƿ���
*	��    ��: ��
*	�� �� ֵ: ����ֵ1 ��ʾ����(��ͨ����0��ʾδ���£��ͷţ�
*********************************************************************************************************
*/
static uint8_t IsTrigFunc(uint8_t _id)
{
	/* С�ڵ��ڴ���ͨ���� */
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
*	�� �� ��: bsp_DetectTrig
*	����˵��: ��ⴥ�����롣������״̬�����뱻�����Եĵ��á�
*	��    ��: IO��id�� ��0��ʼ����
*	�� �� ֵ: 1-�д�������	��	0-�޴�������
*********************************************************************************************************
*/
uint8_t bsp_DetectTrig(uint8_t	_id)
{
	LIGHT_DATA *pBtn;
	pBtn = &g_CHx[_id];
	
	/*	����Ƿ��д�������	*/
	if (_id < LIGHT_CHANNEL_HARD_NUM)
	{
		if (IsTrigFunc(_id))
		{
			if (pBtn->TrigState == 0)
			{
				/*	�д�������	*/
				pBtn->TrigState = TRIG_ON;
				return	1;
			}
		}
		else
		{
			if (pBtn->TrigState == 1)
			{
				/*	�޴�������	*/
				pBtn->TrigState = TRIG_OFF;
				return	0;
			}
		}
	}
	
}




/*
*********************************************************************************************************
*	�� �� ��: bsp_LightTrigHandler
*	����˵��: ��Դ���������������ڶ���ִ�У�ȷ����Դ�ȶ��������bsp.h�б����á�
*	��    ��:	��	
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_TrigHandler(void)
{
	if(g_TrigFlag	==	TRIG_ON)
	{
	/*	---����ģʽ---*/
		if(g_CtrlMode	==	MODE_H)
		{

			/*	--��ѯ��ѯ����ͨ��--	*/
			for(uint8_t	i=0;i<=LIGHT_CHANNEL_HARD_NUM;i++)
			{
				/*	�ٴμ�ⴥ������״̬	*/
				//bsp_DetectTrig(i);
				if(bsp_DetectTrig(i) == TRUE)
				{
					if(g_CHx[i].TrigState	==	TRIG_ON)
					{
						/*	-�д�������Դ�ر�	*/
						g_CHx[i].State	=	LIGHT_OFF;
					}
					else
					{
						
						/*	-�޴�������Դ��	*/
						g_CHx[i].State	=	LIGHT_ON;
					}
				}
			}
		}
		/*	---����ģʽ---	*/
		if(g_CtrlMode	==	MODE_L)
		{
			/*	--��ѯ��ѯ����ͨ��--	*/
			for(uint8_t	j=0;j<=LIGHT_CHANNEL_HARD_NUM;j++)
			{
				/*	�ٴμ�ⴥ������״̬	*/
				//bsp_DetectTrig(j);
				if(bsp_DetectTrig(j) == TRUE)
				{
					if(g_CHx[j].TrigState	==	TRIG_ON)
					{
						/*	-�д�������Դ����	*/
						g_CHx[j].State	=	LIGHT_ON;
					}
					else
					{
						/*	-�޴�������Դ�ر�	*/
						g_CHx[j].State	=	LIGHT_OFF;
					}
				}
			}
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: HAL_GPIO_EXTI_Callback
*	����˵��: 
*	��    ��: ����	-	1   ��  �ر�	-	0		
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
//void	HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	for(uint8_t i	=	0;	i<LIGHT_CHANNEL_HARD_NUM;	i++)
//	{
//		if(g_trig_gpio_list[i].pin	==	GPIO_Pin)
//		{
//			/*	---����Ƿ��д�������	*/
//			if(bsp_DetectTrig(i))
//			{
//				/*	������־λset	*/
//				g_TrigFlag	=	TRIG_ON;
//				g_CHx[i].TrigState	=	TRIG_ON;
//				/*	���ٴ�����	*/
//				//bsp_LightQuickCtrlTrigON(i);
//			}
//			else
//			{
//				g_TrigFlag	=	TRIG_OFF;
//				g_CHx[i].TrigState	=	TRIG_OFF;
//				/*	���ٴ�����	*/
//				//bsp_LightQuickCtrlTrigOFF(i);
//			}
//		}
//	}	
//}

/*
*********************************************************************************************************
*	�� �� ��: bsp_trig_fun
*	����˵��: ��Դ����+����������������ѭ�����жϴ����б����á�
*	��    ��: ��	
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_trig_fun(void)
{
	switch(g_CtrlMode)
	{
		case MODE_H :
			for(uint8_t i	=	0;	i<LIGHT_CHANNEL_HARD_NUM;	i++)
			{
					/*	�д���	*/
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
					/*	�д���	*/
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


