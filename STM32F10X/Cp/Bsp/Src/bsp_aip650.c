/*
*********************************************************************************************************
*
*	ģ������ : ���������IC-����ģ��
*	�ļ����� : bsp_aip650.c
*	��    �� : V1.0
*	˵    �� : Aip650�ĵײ��������룬ʹ��ģ��IIC������������Hal��
*
*	�޸ļ�¼ :
*		�汾��  ����       		 ����     ˵��
*		V1.0    2024-06-05 		 alan  		
*
*********************************************************************************************************
*/
#include "bsp.h"

/*	IO����	*/
#define SCL_Set 		HAL_GPIO_WritePin(AIP650_PORT_CLK, AIP650_PIN_CLK, GPIO_PIN_SET);//(GPIOA->BSRR = 1<<14)
#define SDA_Set 		HAL_GPIO_WritePin(AIP650_PORT_DAT, AIP650_PIN_DAT, GPIO_PIN_SET);//(GPIOA->BSRR = 1<<13)
#define SCL_Reset		HAL_GPIO_WritePin(AIP650_PORT_CLK, AIP650_PIN_CLK, GPIO_PIN_RESET);//(GPIOB->BSRR = 1<<(16+14))
#define SDA_Reset 	    HAL_GPIO_WritePin(AIP650_PORT_DAT, AIP650_PIN_DAT, GPIO_PIN_RESET); //(GPIOB->BSRR = 1<<(16+13))

#define SDA_Read 		(HAL_GPIO_ReadPin(AIP650_PORT_DAT, AIP650_PIN_DAT))

/*	I2C��ʱ */
#define IIC_uS 5

/* ʹ��GPIOʱ�� */
#define		AIP650_GPIO_CLK_ENABLE(){	\
			__HAL_RCC_GPIOA_CLK_ENABLE();	\
			__HAL_RCC_GPIOB_CLK_ENABLE();	\
			__HAL_RCC_GPIOC_CLK_ENABLE();	\
			__HAL_RCC_GPIOD_CLK_ENABLE();	\
};
	

/** 
*	�������ʾ�����֡���ĸ��Ӧ�Ĵ�������
*	0,1,2,3,4,5,6,7,8,9,E,r,H,P,-,��		
**/											
const uint8_t DISPLAY_NUM[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x79,0x50,0x76,0x73,0x40,0x00,};


static void AIP650_Wr_RAM(uint8_t Address, uint8_t Data);
							
													
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� Bsp_InitLedDisplay() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
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
*	�� �� ��: AIP650_InitConf
*	����˵��: ����AIP650оƬ��ϵͳʹ�ܣ�������ʾ���ú����� Bsp_InitLedDisplay() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AIP650_InitConf(void)
{
	//������룺4801 ��ϵͳʹ�ܣ�������ʾ���������
	AIP650_Wr_RAM(0x48,1);
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLedDisplay
*	����˵��: ��ʼ�������. �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
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
* �� �� ��: Delay_uS
* ����˵��: I2C����λ�ӳ�
* ��    �Σ���
* �� �� ֵ: ��
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
*	�� �� ��: IIC_Start
*	����˵��: 
*	��    ��:  	��
*	�� �� ֵ: 	��
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
*	�� �� ��: IIC_Stop
*	����˵��: 
*	��    ��:  	��
*	�� �� ֵ: 	��
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
*	�� �� ��: IIC_Wait_Ack
*	����˵��: 
*	��    ��:  	��
*	�� �� ֵ: 	��
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
*	�� �� ��: IIC_Wr_Byte
*	����˵��: 	I2Cд����
*	��    ��:  	��
*	�� �� ֵ: 	��
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
*	�� �� ��: AIP650_Wr_RAM
*	����˵��: 	д�Դ�
*	��    ��:  	��
*	�� �� ֵ: 	��
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
*	�� �� ��:	Tube_DisNum
*	����˵��: 	�������ʾ����,��4λ֧������
*	��    ��:  	�ַ�����
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
void Tube_DisNum(uint8_t *Dis)
{
    /*��ʾ*/
    AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[Dis[0]]);
    AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[Dis[1]]);
    AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[Dis[2]]);
    AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[Dis[3]]);
}


/*
*********************************************************************************************************
*	�� �� ��:	bsp_DisplayErrAip650
*	����˵��: 	�������ʾ��������ΪERRx,xΪ����
*	��    ��:  	�������֣�0-9��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/
void bsp_DisplayErrAip650(uint8_t Err_code)
{
	/* �����벻�ܳ���10 */
	if(Err_code	>=	10)
	{
				Err_code=9;
	}
	/* ��ʾ������	*/
   AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[10]);					//E
   AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[11]);					//r
   AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[11]);					//r
   AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[Err_code]);		//0~9
	
	//HAL_Delay(100);
}

void bsp_DisplayON(void )
{

	/* ��ʾ-	*/
   AIP650_Wr_RAM(CMD_DIG0, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG1, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG2, DISPLAY_NUM[14]);		//-
   AIP650_Wr_RAM(CMD_DIG3, DISPLAY_NUM[14]);		//-
	
	HAL_Delay(2000);
	
}




