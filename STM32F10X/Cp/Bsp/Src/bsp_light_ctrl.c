/*
*********************************************************************************************************
*
*	ģ������ : ��Դ��������Ϳ���ģ��
*	�ļ����� : bsp_light_ctrl.c
*	��    �� : V1.0
*	˵    �� : �������й�Դͨ��������ֵ������ģʽ���Ϳ��ƿ��غʹ������ƵȺ���
*	�޸ļ�¼ :
*		�汾��  ����       	����    ˵��
*		V1.0    2024-05-28 	Alans  	��ʽ����
*
*********************************************************************************************************
*/
#include "bsp.h"


/*********************************************************
*	----IO��������----	
**********************************************************/
/*	--��Դͨ������	*/
#define	Switch_R_ON		(Switch_R_Port->BSRR	=	Switch_R_Pin);										//�ߵ�ƽ
#define	Switch_R_OFF	(Switch_R_Port->BSRR = (uint32_t)Switch_R_Pin << 16u;);		//�͵�ƽ

#define	Switch_G_ON		(Switch_G_Port->BSRR	=	Switch_G_Pin);
#define	Switch_G_OFF	(Switch_G_Port->BSRR = (uint32_t)Switch_G_Pin << 16u;);

#define	Switch_B_ON		(Switch_B_Port->BSRR	=	Switch_B_Pin);
#define	Switch_B_OFF	(Switch_B_Port->BSRR = (uint32_t)Switch_B_Pin << 16u;);

/*	--��Դ��Դ����ͨ������	*/
#define	Power_R_ON		(Power_R_Port->BSRR	=	Power_R_Pin);											//�ߵ�ƽ
#define	Power_R_OFF		(Power_R_Port->BSRR = (uint32_t)Power_R_Pin << 16u;);		//�͵�ƽ

#define	Power_G_ON		(Power_G_Port->BSRR	=	Power_G_Pin);
#define	Power_G_OFF		(Power_G_Port->BSRR = (uint32_t)Power_G_Pin << 16u;);

#define	Power_B_ON		(Power_B_Port->BSRR	=	Power_B_Pin);
#define	Power_B_OFF		(Power_B_Port->BSRR = (uint32_t)Power_B_Pin << 16u;);

/*	--�����������ŵ�ƽ��ȡ	*/
//#define		Trig_R_ON			((Trig_IN1_Port->IDR & Trig_IN1_Pin)	==	(uint32_t)GPIO_PIN_RESET);	//�����͵�ƽ=�д�������
//#define		Trig_R_OFF		((Trig_IN1_Port->IDR & Trig_IN1_Pin)	!=	(uint32_t)GPIO_PIN_RESET);	//�����ߵ�ƽ=�޴�������

//#define		Trig_G_ON			((Trig_IN2_Port->IDR & Trig_IN2_Pin)	==	(uint32_t)GPIO_PIN_RESET);
//#define		Trig_G_OFF		((Trig_IN2_Port->IDR & Trig_IN2_Pin)	!=	(uint32_t)GPIO_PIN_RESET);

//#define		Trig_B_ON			((Trig_IN3_Port->IDR & Trig_IN3_Pin)	==	(uint32_t)GPIO_PIN_RESET);
//#define		Trig_B_OFF		((Trig_IN3_Port->IDR & Trig_IN3_Pin)	!=	(uint32_t)GPIO_PIN_RESET);



/*	-��Դ�ṹ��,����ǰ��λ��Ҫ����ʵ��������룬��λ���������֮��ᱻ��ʼ����*/
LIGHT_DATA g_CHx[LIGHT_CHANNEL_NUM] = 
{
	{Power_R_Port,Power_R_Pin,Switch_R_Port,Switch_R_Pin,0,0,0,0},
	{Power_G_Port,Power_G_Pin,Switch_G_Port,Switch_G_Pin,0,0,0,0},
	{Power_B_Port,Power_B_Pin,Switch_B_Port,Switch_B_Pin,0,0,0,0}
};


/**	
 * --����ģʽ��־λ		
 * 1:����ģʽ		
 * 0:����ģʽ	
**/
uint8_t g_CtrlMode	=	0;
/**	
 * --�����־λ		
 * 1:ͨ������		
 * 2:�������
 * ��enum����ERROR_STATE��	
**/
uint8_t g_ErrFlag	=	0;  

uint8_t g_ErrNum	=	0;

/*
*********************************************************************************************************
*	�� �� ��: bsp_ModeCtrl
*	����˵��: ģʽ���ƺ�������bsp_uart_protocol.c����
*	��    ��:  ��
*	�� �� ֵ: ��
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
*	�� �� ��: bsp_InitLightVar
*	����˵��: ��ʼ����Դͨ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitLightVar(void)
{
	if(bsp_CheckFlashSucess() == FALSE)
	{
		g_CtrlMode	=	MODE_H;		//�ϵ�Ĭ���ǳ���״̬
	}


	/* ��ÿ����Դͨ���ṹ���Ա������һ��ȱʡֵ */
	for (uint8_t i = 0; i < LIGHT_CHANNEL_NUM ; i++)
	{
		if(bsp_CheckFlashSucess() == FALSE)
		{
			g_CHx[i].State	=	LIGHT_OFF	;		
			g_CHx[i].value		=	0;	
		}
		/*	����ֵ �������ϵ�Ĭ��Ϊ0	*/
		g_CHx[i].TrigState	=	TRIG_OFF;	/*	����״̬���ϵ�Ĭ��-�޴���*/
	}
	
	for(uint8_t j =1;j	<	(LIGHT_CHANNEL_NUM	+	1	);	j++)
	{
		g_CHx[j-1].channel		=	j;		/*		ͨ����		*/
	}
}



static void bsp_InitLightHard(void)
{
  HAL_GPIO_WritePin(GPIOA, PowerCTR_G_Pin|PowerCTR_B_Pin|PowerCTR_R_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, SwCTR_R_Pin|SwCTR_G_Pin|SwCTR_B_Pin, GPIO_PIN_SET);
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLight
*	����˵��: ��ʼ����Դͨ��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLight(void)
{
	bsp_InitLightVar();
	bsp_InitLightHard();
	
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_SetBrightness
*	����˵��: ���Ӷ�Ӧͨ��������ֵ,�����ֵ�󱣳����ֵ
*	��    ��:  ch-��Դͨ��
*	�� �� ֵ: 	TRUE-�ɹ�			FALSE-ʧ��         
*********************************************************************************************************
*/
int bsp_SetBrightness(uint8_t _ucChannel	,	uint16_t	_usValue)
{
	/*	ͨ����	*/
	if(_ucChannel	<=	LIGHT_CHANNEL_NUM)
	{
		/*	ͨ������ֵ���������ֵ	*/
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
*	�� �� ��: bsp_IncLightValue
*	����˵��: ���Ӷ�Ӧͨ��������ֵ,�����ֵ�󱣳����ֵ
*	��    ��:  ch-��Դͨ��
*	�� �� ֵ: 	0-�ɹ�			1-ʧ��         
*********************************************************************************************************
*/
int bsp_KeyIncLightValue(const uint16_t _usDisPlayPage)
{
	uint16_t ch	=	0;	//ͨ����
	
	ch	=	_usDisPlayPage	-	1;	//��Դͨ����	= ��ʾҳ�� - 1 
	
	if(	(ch	<	LIGHT_CHANNEL_NUM))
	{
		if(g_CHx[ch].value	>=	LIGHT_VALUE_MAX)
		{
			g_CHx[ch].value	=	LIGHT_VALUE_MAX;
			/*	DACˢ���¼�	*/
			bsp_DacUpdata();
			return TRUE;
		}
		else
		{
			g_CHx[ch].value	++;
			
			/*	DACˢ���¼�	*/
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
*	�� �� ��: bsp_DecLightValue
*	����˵��: ���Ӷ�Ӧͨ��������ֵ,�����ֵ�󱣳����ֵ
*	��    ��:  ch-��Դͨ��
*	�� �� ֵ: 	0-�ɹ�			1-ʧ��         
*********************************************************************************************************
*/
int bsp_KeyDecLightValue(const uint16_t _usDisPlayPage)
{
	uint16_t ch	=	0;	//ͨ����
	
	ch	=	_usDisPlayPage	-	1;	//��Դͨ����	= ��ʾҳ�� - 1 
	
	if(	(ch<LIGHT_CHANNEL_NUM) )
	{
		if(g_CHx[ch].value	<=	LIGHT_VALUE_MIN)
		{
			g_CHx[ch].value	=	LIGHT_VALUE_MIN;
			
			/*	DACˢ���¼�	*/
			bsp_DacUpdata();
			return 0;
		}
		else
		{
			g_CHx[ch].value	--;
			
			/*	DACˢ���¼�	*/
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
*	�� �� ��: bsp_KeyIncMode
*	����˵��: ����ģʽ+1 ����Ϊ1
*	��    ��:  ch-��Դͨ��
*	�� �� ֵ: 	0-�ɹ�			1-ʧ��         
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
*	�� �� ��: bsp_KeyDecMode
*	����˵��: ����ģʽ-1 ����Ϊ0
*	��    ��:  ch-��Դͨ��
*	�� �� ֵ: 	0-�ɹ�			1-ʧ��         
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
*	�� �� ��: bsp_LightCtrl
*	����˵��: ��Դ����״̬�޸ģ�����Դ����ֵ�ж�-����ֵΪ0�رչ�Դ
*	��    ��: _ucChannel-ͨ��				_ucONOFF-	����-1   ��  �ر�-0		
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_LightCtrl(uint8_t	_ucChannel,uint8_t	_ucONOFF)
{
	LIGHT_DATA	*pBtn	;
	pBtn	=	&g_CHx[_ucChannel];
	
	/*	������Դͨ��	*/
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
	/*	�رչ�Դͨ��	*/
	else 	/*pBtn	==	LIGHT_OFF	*/
	{
		if(pBtn->State	==	LIGHT_ON)
		{
			pBtn->State	=	LIGHT_OFF;
		}
	}
	/*	��Դͨ��Ϊ0���ж�Ϊ�رչ�Դ	*/
	if(pBtn->value	 == 0)
	{
		pBtn->State	=	LIGHT_OFF;
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LightQuickCtrlTrigON
*	����˵��: ������Ӧ�������룬���жϴ���״̬�������������ڷ�������ʱ�����жϺ�������
*	��    ��: _usCh		��Դ�ṹ�����
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_LightQuickCtrlTrigON(uint16_t _usCh)
{
	if(g_CtrlMode	==	MODE_H)
	{
		g_CHx[_usCh].State	=	LIGHT_OFF;
		g_CHx[_usCh].power_gpio->BSRR = (uint32_t)g_CHx[_usCh].power_pin << 16u;			//�رչ�Դ��Դ
		g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//�ر�ͨ��
	}
	else
	{
		g_CHx[_usCh].State	=	LIGHT_ON;
		g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//������Դ��Դ
		g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin << 16u;	//����ͨ��
	}
	
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LightQuickCtrlTrigOFF
*	����˵��: ������Ӧ�����Ͽ������жϴ���״̬�������������ڷ�������ʱ�����жϺ�������
*	��    ��: _usCh		��Դ�ṹ�����
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_LightQuickCtrlTrigOFF(uint16_t _usCh)
{
	if(g_CtrlMode	==	MODE_H)
	{
		g_CHx[_usCh].State	=	LIGHT_ON;
		g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//������Դ��Դ
		g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//����ͨ��
	}
	else
	{
		g_CHx[_usCh].State	=	LIGHT_OFF;
		g_CHx[_usCh].power_gpio->BSRR = (uint32_t)g_CHx[_usCh].power_pin << 16u;			//�رչ�Դ��Դ
		g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin << 16u;	//�ر�ͨ��
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LightChannel_ON
*	����˵��: ��Դ�򿪺�����
*	��    ��: _usCh��ͨ������ 0��ͨ��1 ��1��ͨ��2	��2��ͨ��3
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_LightChannel_ON(uint16_t	_usCh)
{
	g_CHx[_usCh].power_gpio->BSRR	=	g_CHx[_usCh].power_pin;				//������Դ��Դ
	g_CHx[_usCh].channel_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].channel_pin	<< 16u;		//�ر�ͨ��
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LightChannel_OFF
*	����˵��: ��Դ�رպ�����
*	��    ��: _usCh��ͨ������ 0��ͨ��1 ��1��ͨ��2	��2��ͨ��3
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_LightChannel_OFF(uint16_t	_usCh)
{
	g_CHx[_usCh].power_gpio->BSRR	=	(uint32_t)g_CHx[_usCh].power_pin	<< 16u;				//�رչ�Դ��Դ
	g_CHx[_usCh].channel_gpio->BSRR	=	g_CHx[_usCh].channel_pin;		//����ͨ��
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
*	�� �� ��: bsp_LightCtrlHandler
*	����˵��: ��Դ���ƺ�������Ҫ��ѯ��ѯ����ִ�У���bsp.c���á�
							stateΪ1-������Դ��stateΪ0-�رչ�Դ
*	��    ��: ��
*	�� �� ֵ: ��       
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
*	�� �� ��: bsp_DetectLightState
*	����˵��: ��Դ״̬��⺯������Ҫ��ѯ��ѯ����ִ�У���bsp.c���á�
*	��    ��: ��
*	�� �� ֵ: ��       
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
*	�� �� ��: bsp_ErrorHandler
*	����˵��: �����봦��������Ҫ��ѯִ�У���bsp.c���á�
*	��    ��: ��
*	�� �� ֵ: ��       
*********************************************************************************************************
*/
void bsp_ErrorHandler(void)
{
	static uint8_t i = 0;
	/*	ͨ������	*/
	if(	(g_ErrFlag  <= B_OCP_FLAG)  && (g_ErrFlag != 0))
	{
		/*	�ر�ͨ�� */
		bsp_All_LightChannel_off();
		/*	��������	*/
		bsp_SaveFlashData();
		bsp_DisplayErrAip650(g_ErrFlag);
		
		/*	�ر��ж�	*/
		DISABLE_INT();
		while(1)
		{
			/*	�ر��ж�	*/
			DISABLE_INT();
			/*	ι��	*/
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
		
		/*	��λ */
		//NVIC_SystemReset();
	}
}

