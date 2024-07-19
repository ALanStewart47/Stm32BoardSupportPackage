#ifndef __BSP_MCP4922_H
#define __BSP_MCP4922_H

//
#include "stdint.h"

#include "stm32f1xx_hal.h"

#define MCP4922_NUM						2		/*	��ʵ�����������*/

#define MCP4922_OUT_CHANNEL		2		/*	MCP4922оƬ�����ͨ�����������޸�*/

#define MCP4922_CS1_Pin 		GPIO_PIN_2
#define MCP4922_CS1_Port 		GPIOB

#define MCP4922_CS2_Pin 		GPIO_PIN_1
#define MCP4922_CS2_Port 		GPIOB

#define MCP4922_SCK_Pin 		GPIO_PIN_3
#define MCP4922_SCK_Port 		GPIOB

#define MCP4922_SDI_Pin 		GPIO_PIN_4
#define MCP4922_SDI_Port 		GPIOB

#define MCP4922_LDAC_Pin 		GPIO_PIN_5
#define MCP4922_LDAC_Port 	GPIOB

#define MCP4922_Delay		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

#define	DATA_WIDTH_8_BIT	0				/*	��Դ���ȷֱ�����8λ	*/

#define	DATA_WIDTH_8_BIT_SPEC		1	/*	����--���ֵΪ0.9��ֻ�ܵ�3060*/	

#define	DATA_WIDTH_12_BIT	0				/*	��Դ���ȷֱ�����12λ */


/*	MCP4922��CS���ŵĽṹ��	*/
typedef struct
{
	GPIO_TypeDef*		gpio;
	uint16_t				pin;
}MCP4922_CS_LIST;

/*	MCP4922�����*/
typedef enum
{
	MCP4922_N_0	=	0,
	MCP4922_N_1	
}MCP4922SN;




extern	void	MCP4922_Init(void);
//extern void MCP4922_DAC(uint8_t channel,uint16_t data);
extern	void	bsp_DacUpdata(void);
extern	void	bsp_DacMcp4922Handler(void);




#endif 
