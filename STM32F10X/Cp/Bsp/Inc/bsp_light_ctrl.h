#ifndef	__BSP_LIGHT_CTRL_H
#define	__BSP_LIGHT_CTRL_H

#include "stm32f1xx_hal.h"

#define LIGHT_VALUE_MIN		0				/*	������Сֵ�������Լ�����Ŀʵ�����ã�*/
#define LIGHT_VALUE_MAX		255			/*	�������ֵ�������Լ�����Ŀʵ�����ã�*/

#define LIGHT_CHANNEL_R  LIGHT_CHANNEL_1
#define LIGHT_CHANNEL_G  LIGHT_CHANNEL_2
#define LIGHT_CHANNEL_B  LIGHT_CHANNEL_3

#define Switch_R_Pin 		GPIO_PIN_7
#define Switch_R_Port 	GPIOB

#define Switch_G_Pin	 	GPIO_PIN_8
#define Switch_G_Port		GPIOB

#define Switch_B_Pin 		GPIO_PIN_9
#define Switch_B_Port 	GPIOB

#define Power_R_Pin 		GPIO_PIN_12
#define Power_R_Port 		GPIOA

#define Power_G_Pin 		GPIO_PIN_8
#define Power_G_Port 		GPIOA

#define Power_B_Pin 		GPIO_PIN_11
#define Power_B_Port 		GPIOA



/*	ͨ�����	,����ʵ����Ŀ��Ҫ�����ӻ����*/
typedef enum
{	
	LIGHT_CHANNEL_1		=	0,		/*	ͨ��һ	*/
	LIGHT_CHANNEL_2			,			/*	ͨ����	*/
	LIGHT_CHANNEL_3						/*	ͨ����	*/
}LIGHT_CHANNEL_SN;	

/*	����ģʽ	*/
typedef enum
{
	MODE_L =	0	,			//����ģʽ
	MODE_H						//����ģʽ
}MODE_SELET;

/*	��Դ״̬	*/
typedef	enum
{
	LIGHT_OFF	=	0,
	LIGHT_ON	
}LIGHT_STATE;

/**
 * �����־
 * ����g_ErrFlag��ֵ����ʾ����
*/
typedef enum
{
	R_OCP_FLAG	=	1,
	G_OCP_FLAG	,
	B_OCP_FLAG	,
	POWERDOWN_FLAG	 
}ERROR_STATE_FLAG;

/*	��ͬ��Դ�Ĳ���	*/
typedef struct
{
	GPIO_TypeDef* power_gpio;		/*	��Դ��������	*/
	uint16_t power_pin;
	GPIO_TypeDef* channel_gpio;	/*	��Դ��������	*/
	uint16_t channel_pin;	
	/**/
	uint8_t 	channel;					/*	��Դͨ�����	-������ʾ��ͨѶ֮��	*/
	uint8_t		State;						/*	��Դ����״̬	*/
	uint16_t 	value;						/*	��Դ����	*/
	uint8_t		TrigState;					/*	����״̬	*/
}LIGHT_DATA;

/*	
	����ģʽ��־λ	
	=	1	������ģʽ
	=	0	������ģʽ
*/
extern uint8_t g_CtrlMode;

/**	
 * --�����־λ		
 * 1:����ģʽ		
 * 0:����ģʽ	
**/
extern uint8_t g_ErrFlag;

/*	��Դͨ���ṹ��	*/
extern LIGHT_DATA g_CHx[];

uint8_t	bsp_ModeCtrl(uint8_t	_ucMode);

int bsp_SetBrightness(uint8_t _ucChannel	,	uint16_t	_usValue);

int bsp_KeyIncLightValue(const	uint16_t _usDisPlayPage);

int bsp_KeyDecLightValue(const uint16_t _usDisPlayPage);
/*	ģʽ����- ģʽ�л�ҵ�� */
void bsp_KeyIncMode(void);
/*	ģʽ����+ ģʽ�л�ҵ�� */
void bsp_KeyDecMode(void);
	
void bsp_InitLight(void);

void bsp_LightCtrl(uint8_t	_ucChannel,uint8_t	_ucONOFF);

void bsp_LightQuickCtrlTrigON(uint16_t _usCh);

void bsp_LightQuickCtrlTrigOFF(uint16_t _usCh);

void bsp_LightCtrlHandler(void);

void bsp_ErrorHandler(void);

void bsp_DetectLightState(void);

void bsp_LightChannel_ON(uint16_t	_usCh);
void bsp_LightChannel_OFF(uint16_t	_usCh);

#endif

