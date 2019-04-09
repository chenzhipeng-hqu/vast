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
#include <stdio.h>
#include "tsl2591.h"

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
  * @brief TSL2591_Init.
  * @param  
  * @retval 
  */
int TSL2591_Init(TSL2591_HandleTypeDef *pTsl2591)
{
	pTsl2591->Init.Write(TSL2591_I2C_ADDRESS, 
													TSL2591_COMMAND_BIT 
														| TSL2591_REG_ENABLE,
													TSL2591_ENABLE_POWERON 
														| TSL2591_ENABLE_AEN 
														| TSL2591_ENABLE_AIEN 
														| TSL2591_ENABLE_NPIEN
											);
	
//	printf("addr:0x%02x, reg:0x%02x, val:0x%02x\r\n", 
//													TSL2591_I2C_ADDRESS, 
//													TSL2591_COMMAND_BIT 
//														| TSL2591_REG_ENABLE,
//													TSL2591_ENABLE_POWERON 
//														| TSL2591_ENABLE_AEN 
//														| TSL2591_ENABLE_AIEN 
//														| TSL2591_ENABLE_NPIEN
//											);
	
	return 0;
}

/**
  * @brief TSL2591_GetChxVal.
	* @param  ch: 0/1
  * @retval 
  */
uint16_t TSL2591_GetChxVal(TSL2591_HandleTypeDef *pTsl2591, uint8_t ch)
{
	uint16_t tempVar = 0;
	
	tempVar = pTsl2591->Init.Read(TSL2591_I2C_ADDRESS, TSL2591_COMMAND_BIT 
														| TSL2591_REG_CHAN0_LOW+(2*ch));
	
	tempVar |= pTsl2591->Init.Read(TSL2591_I2C_ADDRESS, TSL2591_COMMAND_BIT 
														| TSL2591_REG_CHAN0_HIGH+(2*ch)) << 8;
	
	return tempVar;
}

/**
  * @brief TSL2591_GetDevID.
	* @param  ch: 0/1
  * @retval 
  */
uint8_t TSL2591_GetDevID(TSL2591_HandleTypeDef *pTsl2591)
{
	uint8_t tempVar = 0;
	
	tempVar = pTsl2591->Init.Read(TSL2591_I2C_ADDRESS, 
																TSL2591_COMMAND_BIT 
																	| TSL2591_REG_DEVICE_ID);
	
//	printf("addr:0x%02x, reg:0x%02x, val:0x%02x\r\n", 
//													TSL2591_I2C_ADDRESS, 
//													TSL2591_COMMAND_BIT 
//														| TSL2591_REG_DEVICE_ID,
//													tempVar
//											);
		
	return tempVar;
}

/**
  * @brief TSL2591_GetDevID.
	* @param  ch: 0/1
  * @retval 
  */
uint8_t TSL2591_GetDevStatus(TSL2591_HandleTypeDef *pTsl2591)
{
	uint8_t tempVar = 0;
	
	tempVar = pTsl2591->Init.Read(TSL2591_I2C_ADDRESS,
																TSL2591_COMMAND_BIT 
																	| TSL2591_REG_DEVICE_STATUS);
		
	
//	printf("addr:0x%02x, reg:0x%02x, val:0x%02x\r\n", 
//													TSL2591_I2C_ADDRESS, 
//													TSL2591_COMMAND_BIT 
//														| TSL2591_REG_DEVICE_STATUS,
//													tempVar
//											);
	return tempVar;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



