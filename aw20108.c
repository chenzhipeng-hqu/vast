/**
 ******************************************************************************
 * @file    led_task.c
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-sep-2018
 * @brief   
 ******************************************************************************
 * @attention
 * 
 ******************************************************************************
 */
    
/** @addtogroup Project
  * @{
  */ 

/***********************************************
                    include
***********************************************/
#include "aw20108.h"

/***********************************************
                    define
***********************************************/
	
/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/

/***********************************************
                   function
***********************************************/
/**
  * @brief  AW20108_SetDIMx.
  * @param  page: @ref AW20108_PAGE(x)
  * @retval 
  */
int AW20108_SetPage(AW20108_HandleTypeDef *pAw20108, uint8_t page)
{
	int ret = 0;
	
	if(page != pAw20108->CurrentPage)
	{
		ret = pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_REG_PAGE, page);
		
		if(ret == 0)
		{
			pAw20108->CurrentPage = page;
		}
	}
	
	return ret;
}

/**
  * @brief  AW20108_SetDIMx.
  * @param  dim: 0~0x3F
  * @retval 
  */
int AW20108_SetDIMx(AW20108_HandleTypeDef *pAw20108, uint8_t dim)
{
	uint8_t i=0;
	int ret = 0;
	
	AW20108_SetPage(pAw20108, AW20108_PAGE1);
	
	for(i=0; i<108; i++)
	{
		ret = pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P1_REG_DIM(i), dim);
	}
	
	return ret;
}

/**
  * @brief  AW20108_SetFADEx.
	* @param  fade: 0~0xFF
  * @retval 
  */
int AW20108_SetFADEx(AW20108_HandleTypeDef *pAw20108, uint8_t fade)
{
	uint8_t i=0;
	
	AW20108_SetPage(pAw20108, AW20108_PAGE2);
	
	for(i=0; i<108; i++)
	{
		pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P2_REG_FADE(i), fade);
	}
	
	return 0;
}

/**
  * @brief  AW20108_SetFATx.
  * @param  pat @ref PAT(x) 0~3
  * @retval 
  */
int AW20108_SetFATx(AW20108_HandleTypeDef *pAw20108, uint8_t pat)
{
	uint8_t i=0;
	
	AW20108_SetPage(pAw20108, AW20108_PAGE3);
	
	for(i=0; i<108; i++)
	{
		pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P3_REG_PAT(i), pat);
	}
	
	return 0;
}

/**
  * @brief  AW20108_Init.
  * @param  
  * @retval 
  */
int AW20108_Init(AW20108_HandleTypeDef *pAw20108)
{
	int ret = 0;
	uint8_t tempVar = 0;
	
	pAw20108->CurrentPage = AW20108_PAGE0;
	
	AW20108_SetPage(pAw20108, AW20108_PAGE0);
	
	//0. soft reset all led status
	tempVar = AW20108_RSTR_SW_RSTN_RESET;
	ret = pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P0_REG_RSTR, tempVar);
	if(ret != 0)
	{
		
	}
	else
	{
		//1. configure registers in page0 set LEDON.ONx=0
		tempVar = AW20108_GCCR_ALLON_DISABLE;
		pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P0_REG_GCCR, tempVar);
		
		//2. set SLPR.SLEEP=0
		tempVar = AW20108_SLPCR_ACTIVE;
		pAw20108->Init.Write(pAw20108->Init.DevAddr, AW20108_P0_REG_SLPCR, tempVar);
	}
	
	return ret;
}

/**
  * @brief  AW20108_Init.
  * @param  
  * @retval 
  */
int AW20108_LEDxOn(AW20108_HandleTypeDef *pAw20108, uint8_t ledxReg, uint8_t ledx)
{
	int ret = 0;
	
	AW20108_SetPage(pAw20108, AW20108_PAGE0);
	
	ret = pAw20108->Init.Write(pAw20108->Init.DevAddr, ledxReg, ledx);
	
	return ret;
}

/**
  * @brief  AW20108_ReadDevID.
  * @param  
  * @retval 
  */
uint8_t AW20108_ReadDevID(AW20108_HandleTypeDef *pAw20108)
{
	AW20108_SetPage(pAw20108, AW20108_PAGE0);
	
	return pAw20108->Init.Read(pAw20108->Init.DevAddr, AW20108_P0_REG_IDR);

}
/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



