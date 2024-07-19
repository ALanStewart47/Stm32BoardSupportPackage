#ifndef	__BSP_TRIG_H
#define	__BSP_TRIG_H

#include "bsp.h"

#include "stm32f1xx_hal.h"

#define Trig_IN1_Pin 					GPIO_PIN_0
#define Trig_IN1_Port 				GPIOA
#define Trig_IN1_EXTI_IRQn 		EXTI0_IRQn
#define Trig_IN2_Pin 					GPIO_PIN_1
#define Trig_IN2_Port 				GPIOA
#define Trig_IN2_EXTI_IRQn 		EXTI1_IRQn
#define Trig_IN3_Pin 					GPIO_PIN_2
#define Trig_IN3_Port 				GPIOA
#define Trig_IN3_EXTI_IRQn 		EXTI2_IRQn


/*	定义触发输入的结构体	*/
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	uint8_t ActiveLevel;		/*	触发电平	*/
	//uint8_t	ExtiLine;				/*	中断线	*/
}TRIG_GPIO_T;


///*	触发状态	*/
//typedef	enum
//{
//	TRIG_OFF	=	0,
//	TRIG_ON	
//}TRIG_STATE;

#define TRIG_OFF    0
#define TRIG_ON    1	

extern TRIG_GPIO_T	g_trig_gpio_list[LIGHT_CHANNEL_HARD_NUM];

void bsp_InitTrigHard(void);
void bsp_TrigHandler(void);
uint8_t bsp_DetectTrig(uint8_t	_id);
void	HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void bsp_HAL_GPIO_EXTI(uint8_t ch ,uint16_t GPIO_Pin);
void bsp_trig_fun(void);

#endif

