#ifndef __BSP_MCP4922_H
#define __BSP_MCP4922_H

//
#include "stdint.h"

#include "stm32f1xx_hal.h"

#define MCP4922_NUM						2		/*	按实际情况来定义*/

#define MCP4922_OUT_CHANNEL		2		/*	MCP4922芯片的输出通道数，请勿修改*/

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

#define	DATA_WIDTH_8_BIT	0				/*	光源亮度分辨率是8位	*/

#define	DATA_WIDTH_8_BIT_SPEC		1	/*	特殊--最大值为0.9，只能到3060*/	

#define	DATA_WIDTH_12_BIT	0				/*	光源亮度分辨率是12位 */


/*	MCP4922的CS引脚的结构体	*/
typedef struct
{
	GPIO_TypeDef*		gpio;
	uint16_t				pin;
}MCP4922_CS_LIST;

/*	MCP4922的序号*/
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
