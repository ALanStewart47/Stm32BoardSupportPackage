/*
*********************************************************************************************************
*
*	模块名称 : 数码管驱动IC-驱动模块
*	文件名称 : bsp_aip650.c
*	版    本 : V1.0
*	说    明 : Aip650的底层驱动代码，使用模拟IIC来驱动，基于Hal库
*
*	修改记录 :
*		版本号  日期       		 作者     说明
*		V1.0    2024-06-05 		 alan  		
*
*********************************************************************************************************
*/
#include "bsp.h"

/*	IO操作	*/
#define SCL_Set 		HAL_GPIO_WritePin(AIP650_PORT_CLK, AIP650_PIN_CLK, GPIO_PIN_SET);//(GPIOA->BSRR = 1<<14)
#define SDA_Set 		HAL_GPIO_WritePin(AIP650_PORT_DAT, AIP650_PIN_DAT, GPIO_PIN_SET);//(GPIOA->BSRR = 1<<13)
#define SCL_Reset		HAL_GPIO_WritePin(AIP650_PORT_CLK, AIP650_PIN_CLK, GPIO_PIN_RESET);//(GPIOB->BSRR = 1<<(16+14))
#define SDA_Reset 	    HAL_GPIO_WritePin(AIP650_PORT_DAT, AIP650_PIN_DAT, GPIO_PIN_RESET); //(GPIOB->BSRR = 1<<(16+13))

#define SDA_Read 		(HAL_GPIO_ReadPin(AIP650_PORT_DAT, AIP650_PIN_DAT))

/*	I2C延时 */
#define IIC_uS 5

/* 使能GPIO时钟 */
#define		AIP650_GPIO_CLK_ENABLE(){	\
			__HAL_RCC_GPIOA_CLK_ENABLE();	\
			__HAL_RCC_GPIOB_CLK_ENABLE();	\
			__HAL_RCC_GPIOC_CLK_ENABLE();	\
			__HAL_RCC_GPIOD_CLK_ENABLE();	\
};
	

/** 
*	数码管显示的数字、字母对应的代码数组
*	0,1,2,3,4,5,6,7,8,9,E,r,H,P,-,空		
**/											
const uint8_t DISPLAY_NUM[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x79,0x50,0x76,0x73,0x40,0x00,};


static void AIP650_Wr_RAM(uint8_t Address, uint8_t Data);
							
													
/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 Bsp_InitLedDisplay() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void AIP650_InitHard(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	AIP650_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = AIP650_PIN_CLK;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(AIP650_PORT_CLK, &GPIO_InitStruct);				
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = AIP650_PIN_DAT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(AIP650_PORT_DAT, &GPIO_InitStruct);	
}
													
/*
*********************************************************************************************************
*	函 数 名: AIP650_InitConf
*	功能说明: 配置AIP650芯片，系统使能，开启显示。该函数被 Bsp_InitLedDisplay() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void AIP650_InitConf(void)
{
	//命令代码：4801 ：系统使能，开启显示，最高亮度
	AIP650_Wr_RAM(0x48,1);
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitLedDisplay
*	功能说明: 初始化数码管. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitAip650(void)
{
	AIP650_InitHard();
	AIP650_InitConf();
	bsp_DisplayON();
}


/*
************************************************
* 函 数 名: Delay_uS
* 功能说明: I2C总线位延迟
* 形    参：无
* 返 回 值: 无
************************************************
*/
static void Delay_uS(uint32_t udelay)
{
  __IO uint32_t Delay = udelay * 33 / 16;//(SystemCoreClock / 8U / 1000000U) (0.75)(48)
    
  do
  {
    __NOP();
  }
  while (Delay --);
}


/*
*********************************************************************************************************
*	函 数 名: IIC_Start
*	功能说明: 
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void IIC_Start(void)
{
    SCL_Set;
    Delay_uS(IIC_uS);
    SDA_Set;
    Delay_uS(IIC_uS);
    SDA_Reset;
    Delay_uS(IIC_uS);
    SCL_Reset;
    Delay_uS(IIC_uS);
}

/*
*********************************************************************************************************
*	函 数 名: IIC_Stop
*	功能说明: 
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void IIC_Stop(void)
{
    SCL_Reset;
    Delay_uS(IIC_uS);
    SDA_Reset;
    Delay_uS(IIC_uS);
    SCL_Set;
    Delay_uS(IIC_uS);
    SDA_Set;
    Delay_uS(IIC_uS);
}


/*
*********************************************************************************************************
*	函 数 名: IIC_Wait_Ack
*	功能说明: 
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static uint8_t IIC_Wait_Ack(void)
{	
	uint8_t timeout = 1;
	SCL_Set;
	Delay_uS(5);
	SCL_Reset;
	while((SDA_Read)&&(timeout<=100))
	{
	  timeout++;
	}
	Delay_uS(5);
	SCL_Reset;
	return 0;
}


/*
*********************************************************************************************************
*	函 数 名: IIC_Wr_Byte
*	功能说明: 	I2C写数据
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void IIC_Wr_Byte(uint8_t Data)
{
    uint8_t i = 0;

    for(i = 0; i < 8; i++)
    {
        SCL_Reset;
        Delay_uS(IIC_uS);

        if(Data >> 7)
        {
            SDA_Set;
        }
        else
        {
            SDA_Reset;
        }
        Data <<= 1;
        Delay_uS(IIC_uS);

        SCL_Set;
        Delay_uS(IIC_uS);
    }
    SCL_Reset;
    Delay_uS(IIC_uS);
    SDA_Set;
    Delay_uS(IIC_uS);
}


/*
*********************************************************************************************************
*	函 数 名: AIP650_Wr_RAM
*	功能说明: 	写显存
*	形    参:  	无
*	返 回 值: 	无
*********************************************************************************************************
*/
static void AIP650_Wr_RAM(uint8_t Address, uint8_t Data)
{
    IIC_Start();
    IIC_Wr_Byte(Address);
    IIC_Wait_Ack();
    IIC_Wr_Byte(Data);
    IIC_Wait_Ack();
    IIC_Stop();
}



/*
*********************************************************************************************************
*	函 数 名:	Tube_DisNum
*	功能说明: 	数码管显示数字,仅4位支持整数
*	形    参:  	字符数组
*	返 回 值: 	无
*********************************************************************************************************
*/
void Tube_DisNum(uint8_t *Dis)
{
    /*显示*/
    AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[Dis[0]]);
    AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[Dis[1]]);
    AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[Dis[2]]);
    AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[Dis[3]]);
}


/*
*********************************************************************************************************
*	函 数 名:	bsp_DisplayErrAip650
*	功能说明: 	数码管显示错误，内容为ERRx,x为数字
*	形    参:  	错误数字（0-9）
*	返 回 值: 	无
*********************************************************************************************************
*/
void bsp_DisplayErrAip650(uint8_t Err_code)
{
	/* 错误码不能超过10 */
	if(Err_code	>=	10)
	{
				Err_code=9;
	}
	/* 显示错误码	*/
   AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[10]);					//E
   AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[11]);					//r
   AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[11]);					//r
   AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[Err_code]);		//0~9
	
	//HAL_Delay(100);
}

void bsp_DisplayON(void )
{

	/* 显示-	*/
   AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[14]);		//-
	
	HAL_Delay(2000);
	
}




