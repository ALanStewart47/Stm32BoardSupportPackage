#ifndef	__BSP_LIGHT_CTRL_H
#define	__BSP_LIGHT_CTRL_H

#include "stm32f1xx_hal.h"

#define LIGHT_VALUE_MIN		0				/*	亮度最小值（根据自己的项目实际设置）*/
#define LIGHT_VALUE_MAX		255			/*	亮度最大值（根据自己的项目实际设置）*/

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



/*	通道序号	,根据实际项目的要求增加或减少*/
typedef enum
{	
	LIGHT_CHANNEL_1		=	0,		/*	通道一	*/
	LIGHT_CHANNEL_2			,			/*	通道二	*/
	LIGHT_CHANNEL_3						/*	通道三	*/
}LIGHT_CHANNEL_SN;	

/*	控制模式	*/
typedef enum
{
	MODE_L =	0	,			//常灭模式
	MODE_H						//常亮模式
}MODE_SELET;

/*	光源状态	*/
typedef	enum
{
	LIGHT_OFF	=	0,
	LIGHT_ON	
}LIGHT_STATE;

/**
 * 错误标志
 * 用于g_ErrFlag赋值和显示函数
*/
typedef enum
{
	R_OCP_FLAG	=	1,
	G_OCP_FLAG	,
	B_OCP_FLAG	,
	POWERDOWN_FLAG	 
}ERROR_STATE_FLAG;

/*	不同光源的参数	*/
typedef struct
{
	GPIO_TypeDef* power_gpio;		/*	电源控制引脚	*/
	uint16_t power_pin;
	GPIO_TypeDef* channel_gpio;	/*	光源控制引脚	*/
	uint16_t channel_pin;	
	/**/
	uint8_t 	channel;					/*	光源通道序号	-用于显示和通讯之用	*/
	uint8_t		State;						/*	光源开关状态	*/
	uint16_t 	value;						/*	光源亮度	*/
	uint8_t		TrigState;					/*	触发状态	*/
}LIGHT_DATA;

/*	
	控制模式标志位	
	=	1	；常亮模式
	=	0	：常灭模式
*/
extern uint8_t g_CtrlMode;

/**	
 * --错误标志位		
 * 1:常亮模式		
 * 0:常灭模式	
**/
extern uint8_t g_ErrFlag;

/*	光源通道结构体	*/
extern LIGHT_DATA g_CHx[];

uint8_t	bsp_ModeCtrl(uint8_t	_ucMode);

int bsp_SetBrightness(uint8_t _ucChannel	,	uint16_t	_usValue);

int bsp_KeyIncLightValue(const	uint16_t _usDisPlayPage);

int bsp_KeyDecLightValue(const uint16_t _usDisPlayPage);
/*	模式按键- 模式切换业务 */
void bsp_KeyIncMode(void);
/*	模式按键+ 模式切换业务 */
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

