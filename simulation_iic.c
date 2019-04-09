/**
 ******************************************************************************
 * @file    simulation_iic.c
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-Dec-2018
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
#include "simulation_iic.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

/***********************************************
                   variable
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   function
***********************************************/

/**
  * @brief  simulationI2cStart.  when scl high, sda falling edge
  * @param  
  * @retval 
  */
static void simulationI2cStart(SI2C_HandleTypeDef *pSi2c) 
{
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAOUT);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
}

/**
  * @brief  simulationI2cStop.	when scl high, sda rising edge
  * @param  
  * @retval 
  */
static void simulationI2cStop(SI2C_HandleTypeDef *pSi2c) 
{
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAOUT);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
}

/**
  * @brief  simulationI2cAck.	when scl ninth pulse, sda is low 
  * @param  
  * @retval 
  */
void simulationI2cAck(SI2C_HandleTypeDef *pSi2c) 
{
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAOUT);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
}

/**
  * @brief  simulationI2cNack.	when scl ninth pulse, sda is high 
  * @param  
  * @retval 
  */
static void simulationI2cNack(SI2C_HandleTypeDef *pSi2c) 
{
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAOUT);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
}

/**
  * @brief  simulationI2cSendByte.
  * @param  
  * @retval 
  */
static void simulationI2cSendByte(SI2C_HandleTypeDef *pSi2c, uint8_t byte) 
{
	uint8_t i;
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAOUT);
	
	for(i=0 ;i<8; i++)
	{
		pSi2c->pSi2cProcess(pSi2c, (byte&0x80)? SI2C_FUNC_SDA_HIGH : SI2C_FUNC_SDA_LOW);		
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
		
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
		
		if(i==7)  // release sda
			pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
		byte<<=1;
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	}
}

/**
  * @brief  simulationI2cReadByte.
  * @param  
  * @retval 
  */
static uint8_t simulationI2cReadByte(SI2C_HandleTypeDef *pSi2c) 
{
	uint8_t value = 0, i;

	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAIN);
	
	for(i=0 ;i<8; i++)
	{
		value<<=1;
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
		
		if(pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_READ))
			value |= 0x01;
		
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	}
	
	return value;
}

/**
  * @brief  simulationI2cWaitAck.
  * @param  
  * @retval 0:ack; 1:no ack
  */
static uint8_t simulationI2cWaitAck(SI2C_HandleTypeDef *pSi2c) 
{
	uint8_t t;
	
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDAIN);
	
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_HIGH);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	
	t = pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SDA_READ);

	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_SCL_LOW);
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
		
	return t;
}

/**
  * @brief  simulationI2cInit.
  * @param  
  * @retval 
  */
uint8_t simulationI2cInit(SI2C_HandleTypeDef *pSi2c) 
{	
	uint8_t ret = 0;
	
	if(pSi2c->pSi2cProcess != NULL)
	{
		pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_INIT);
	}
	else
	{
		ret = 1;
	}

	return ret;
}

/**
  * @brief  i2cSendByte.
  * @param  
  * @retval 
  */
uint8_t i2cSendByte(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t byte)
{
	uint8_t ret = 0;
	
	simulationI2cStart(pSi2c);
	simulationI2cSendByte(pSi2c, devAddr);
	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cSendByte(pSi2c, subAddr);
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cSendByte(pSi2c, byte);
	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
	
	return ret;
}


/**
  * @brief  i2cSendByte.
  * @param  
  * @retval 
  */
void i2cSendBuff(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t *pBuf, uint16_t len)
{
	uint8_t i;
	
	simulationI2cStart(pSi2c);
	simulationI2cSendByte(pSi2c, devAddr);
	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cSendByte(pSi2c, subAddr);
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	for (i=0; i<len; i++)
	{	
		simulationI2cSendByte(pSi2c, pBuf[i]);
		if(simulationI2cWaitAck(pSi2c))
		{
			simulationI2cStop(pSi2c);
		}
	}
	
	simulationI2cStop(pSi2c);
	
	pSi2c->pSi2cProcess(pSi2c, SI2C_FUNC_DELAY);
}

/**
  * @brief  i2cReadByte.
  * @param  
  * @retval 
  */
uint8_t i2cReadByte(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr)
{
	uint8_t temp;
	
	simulationI2cStart(pSi2c);
	
	simulationI2cSendByte(pSi2c, devAddr);	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cSendByte(pSi2c, 0x6e);
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cStart(pSi2c);
	
	simulationI2cSendByte(pSi2c, devAddr);	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	temp = simulationI2cReadByte(pSi2c);
	
	simulationI2cNack(pSi2c);	
	simulationI2cStop(pSi2c);
	
	return temp;
}


/**
  * @brief  i2cReadByte.
  * @param  
  * @retval 
  */
uint8_t i2cReadBuff(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t *pBuf, uint16_t len)
{
	uint8_t i;
	
	simulationI2cStart(pSi2c);
	
	simulationI2cSendByte(pSi2c, devAddr);	
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
	
	simulationI2cSendByte(pSi2c, subAddr);
	if(simulationI2cWaitAck(pSi2c))
	{
		simulationI2cStop(pSi2c);
	}
		
	for (i=0; i<len; i++)
	{	
		simulationI2cStart(pSi2c);
		simulationI2cSendByte(pSi2c, devAddr+1);	
		if(simulationI2cWaitAck(pSi2c))
		{
			simulationI2cStop(pSi2c);
			break;
		}		
		pBuf[i] = simulationI2cReadByte(pSi2c);
	}
	
	simulationI2cNack(pSi2c);
	simulationI2cStop(pSi2c);	
		
	return i;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



