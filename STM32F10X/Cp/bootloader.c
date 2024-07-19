/*
*********************************************************************************************************
*
*	模块名称 : 串口升级配置(For STM32F1)
*	文件名称 : bootloader.c
*	版    本 : V1.0
*	说    明 : 用于自定协议的串口升级配置
*	修改记录 :
*		版本号  日期         作者       说明
*		V1.0    2024-06-03  	Alan  	正式发布
*
*
*********************************************************************************************************
*/
//#include "bootloader.h"
#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: NVIC_SetVectorTable
*	功能说明: 设置向量偏移
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void NVIC_SetVectorTable(uint32_t base, uint32_t offset)
{
    /* close interruption*/
    __set_FAULTMASK(1);

    /* set vector table*/
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xffset);
    SCB->VTOR = base | offset;

    /* open interruption*/
    __set_FAULTMASK(0);
}

/*
*********************************************************************************************************
*	函 数 名: set_BootLoader_flag
*	功能说明: 在地址0x8003400写入升级标志位0xaa，
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void set_BootLoader_flag(void)
{
	uint16_t Write_Flash_Data = 0xaa; 
	FLASH_EraseInitTypeDef otaFlash;
	uint32_t PageError = 0;
	
	HAL_FLASH_Unlock();			  //FLASH_Unlock();
	
	/*	配置FLASH擦除结构体	*/
	otaFlash.TypeErase	=	FLASH_TYPEERASE_PAGES;
	otaFlash.PageAddress	=	BOOT_FLAG_ADDR;
	otaFlash.NbPages	=	1; 
	
	HAL_FLASHEx_Erase(&otaFlash, &PageError);				//FLASH_ErasePage(BOOT_FLAG_ADDR);  
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BOOT_FLAG_ADDR, Write_Flash_Data);	  //FLASH_ProgramHalfWord(BOOT_FLAG_ADDR,Write_Flash_Data);   
	
	HAL_FLASH_Lock();		//FLASH_Lock();
}


/*
*********************************************************************************************************
*	函 数 名: bsp_FlashErase
*	功能说明: Flash擦除。
*	形    参：Address-要擦除的地址
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_FlashErase(uint32_t	Address)
{
	FLASH_EraseInitTypeDef	EraseInitStruct;
	uint32_t PageError = 0;
	
	HAL_FLASH_Unlock();	
	EraseInitStruct.TypeErase	=	FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress	=	Address;
	EraseInitStruct.NbPages	=	1;
	HAL_FLASHEx_Erase(&EraseInitStruct,&PageError);
	HAL_FLASH_Lock();
}


uint8_t bsp_CheckFlashSucess(void)
{
	uint16_t i = 0;
	
	i = *(uint16_t*)DATA_SAVE_ADDR	;	
	
	if(i == 	DataSucFlag)
	{
		return TRUE;
	}
	else
	{
		return	FALSE;
	}
	
	
}


/*
*********************************************************************************************************
*	函 数 名: bsp_SaveFlashData
*	功能说明: 保存数据。在地址DATA_SAVE_ADDR写入关键数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SaveFlashData(void)
{
	bsp_FlashErase(DATA_SAVE_ADDR);
	
	HAL_FLASH_Unlock();
	/*	保存标志位	*/
	for(uint8_t i	=	0; i< LIGHT_CHANNEL_HARD_NUM; i++){
		/*	通道亮度值	*/
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ((DATA_SAVE_ADDR+2)	+		(i	*	2)),(uint16_t)g_CHx[i].value);
	}
	/*	显示页面	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2 + 2),	(uint16_t)g_DisPlayPage);
	/*	控制模式	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2	+	4),	(uint16_t)g_CtrlMode);
	
	/*	写入成功标志位	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, DATA_SAVE_ADDR	,	DataSucFlag);
	
	HAL_FLASH_Lock();
}


/*
*********************************************************************************************************
*	函 数 名: bsp_SaveFlashBackupData
*	功能说明: 用于后备的数据保存函数，被定时调用。在地址BACK_DATA_SAVE_ADDR写入关键数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SaveFlashBackupData(void)
{
	uint8_t	i	=	0;
	
	bsp_FlashErase(BACK_DATA_SAVE_ADDR);
	
	HAL_FLASH_Unlock();
	/*	保存标志位	*/
	for(i	=	0; i< LIGHT_CHANNEL_HARD_NUM; i++){
		/*	通道亮度值	*/
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ((BACK_DATA_SAVE_ADDR+2)	+		(i	*	2)),(uint16_t)g_CHx[i].value);
	}
	/*	显示页面	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BACK_DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2 + 2),	(uint16_t)g_DisPlayPage);
	/*	控制模式	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BACK_DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2	+	4),	(uint16_t)g_CtrlMode);
	
	/*	写入成功标志位	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BACK_DATA_SAVE_ADDR	,	(uint16_t)BackDataSucFlag);
	
	HAL_FLASH_Lock();
}


/*
*********************************************************************************************************
*	函 数 名: bsp_ReadFlashData
*	功能说明: 读取关键数据，先读取DATA_SAVE_ADDR区域数据，无数据再读取后备区BACK_DATA_SAVE_ADDR数据
*								需要在开机后被调用。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ReadFlashData(void)
{
	/*	检查写入标志位	*/
	if(*(uint16_t*)DATA_SAVE_ADDR	==	DataSucFlag)
	{
		for(uint8_t	i	=	0; i< LIGHT_CHANNEL_HARD_NUM; i++)
		{
			/*	获取通道亮度值	*/
			g_CHx[i].value	=		(uint16_t)*(uint16_t	*)((DATA_SAVE_ADDR+2)	+		(i	*	2));
		}
		/*	获取显示页面	*/
		g_DisPlayPage	=		(uint8_t)*(uint16_t	*)(DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2 +2));
		
		/*	获取控制模式	*/
		g_CtrlMode	=	(uint8_t) *(uint16_t	*)(DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2	+	4));
		
		g_Uart1_Baud	=		(uint32_t	) *(uint32_t	*)(DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	8));
		
		/*	擦除整页（1K），为写入做准备	*/
		bsp_FlashErase(DATA_SAVE_ADDR);
	}
	else
	{
		/*	检查后备标志位	*/
		if(*(uint16_t *)BACK_DATA_SAVE_ADDR	==	0x88)
		{
			for(uint8_t	i	=	0; i< LIGHT_CHANNEL_HARD_NUM; i++)
			{
				/*	获取通道亮度值	*/
				g_CHx[i].value	=	(uint16_t)	*(uint16_t	*)	(((BACK_DATA_SAVE_ADDR+2)	+		(i	*	2)));
			}
			/*	获取显示页面	*/
			g_DisPlayPage	=	(uint8_t)	*(uint16_t	*)(BACK_DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2));
			/*	获取控制模式	*/
			g_CtrlMode	=	(uint8_t)	*(uint16_t	*)(BACK_DATA_SAVE_ADDR	+	(LIGHT_CHANNEL_HARD_NUM	*	2	+	2));
			
			/*	擦除整页（1K），为写入做准备	*/
			bsp_FlashErase(BACK_DATA_SAVE_ADDR);
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_SaveBaudData
*	功能说明: 读取关键数据，先读取DATA_SAVE_ADDR区域数据，无数据再读取后备区BACK_DATA_SAVE_ADDR数据
*								需要在开机后被调用。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SaveBaudData(void)
{
	bsp_FlashErase(BAUD_DATA_SAVE_ADDR);
	
	HAL_FLASH_Unlock();
	/*	保存标志位	*/

	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, BAUD_DATA_SAVE_ADDR	+	2,	(uint32_t)g_Uart1_Baud);
	/*	写入成功标志位	*/
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, BAUD_DATA_SAVE_ADDR	,	(uint16_t)BaudDataSucFlag);
	
	HAL_FLASH_Lock();
}


uint32_t bsp_CheckBaud(void)
{
	uint32_t _usBaud = 0;
	uint16_t _usFlag	=	0;
	
	_usFlag	=	(uint16_t)	*(uint16_t *) (BAUD_DATA_SAVE_ADDR);
	
	if(_usFlag	==	0x99)
	{
		_usBaud	=	(uint32_t)	*(uint32_t *) ( BAUD_DATA_SAVE_ADDR + 2 );
		return	_usBaud;
	}
	else
	{
		_usBaud = 19200;
		return	_usBaud;
	}
	
}
	



/*****************************	(END OF FILE)	*********************************/

