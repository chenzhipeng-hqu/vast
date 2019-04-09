/**
 ******************************************************************************
 * @file    simulation_iic.h
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
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIMULATION_I2C_H__
#define __SIMULATION_I2C_H__

#ifdef __cplusplus
	extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "stm32f2xx_hal.h"

/***********************************************
                    define
***********************************************/
#define			SIMULATION_IIC						2
		
/*	simulation iic scl config		*/
//#define		SIMULATION_IIC_SCL_PORT		GPIOE
//#define		SIMULATION_IIC_SCL_PIN		GPIO_PIN_6	
//#define 	SIMULATION_IIC_SCL_GPIO_EN  __GPIOE_CLK_ENABLE
//#define		SIMULATION_IIC_SCL_HIGH		HAL_GPIO_WritePin(SIMULATION_IIC_SCL_PORT, SIMULATION_IIC_SCL_PIN, GPIO_PIN_SET)
//#define		SIMULATION_IIC_SCL_LOW		HAL_GPIO_WritePin(SIMULATION_IIC_SCL_PORT, SIMULATION_IIC_SCL_PIN, GPIO_PIN_RESET) 

/*	simulation iic sda config		*/
//#define		SIMULATION_IIC_SDA_PORT		GPIOE
//#define		SIMULATION_IIC_SDA_PIN		GPIO_PIN_14
//#define 	SIMULATION_IIC_SDA_GPIO_EN  __GPIOE_CLK_ENABLE	
//#define		SIMULATION_IIC_SDA_HIGH		HAL_GPIO_WritePin(SIMULATION_IIC_SDA_PORT, SIMULATION_IIC_SDA_PIN, GPIO_PIN_SET)
//#define		SIMULATION_IIC_SDA_LOW		HAL_GPIO_WritePin(SIMULATION_IIC_SDA_PORT, SIMULATION_IIC_SDA_PIN, GPIO_PIN_RESET) 
//#define		SIMULATION_IIC_SDA_READ		HAL_GPIO_ReadPin(SIMULATION_IIC_SDA_PORT, SIMULATION_IIC_SDA_PIN)

/***********************************************
                    typedef
***********************************************/
typedef enum
{
	SI2C_SCL,
	SI2C_SDA
}SI2C_PINx;

typedef enum
{
	SI2C_PIN_LOW,
	SI2C_PIN_HIGH
}SI2C_PIN_STATE;

typedef enum
{
	SI2C_FUNC_INIT,
	SI2C_FUNC_DELAY,
	SI2C_FUNC_SDAOUT,
	SI2C_FUNC_SDAIN,
	SI2C_FUNC_SDA_HIGH,
	SI2C_FUNC_SDA_LOW,
	SI2C_FUNC_SCL_HIGH,
	SI2C_FUNC_SCL_LOW,
	SI2C_FUNC_SDA_READ,
}SI2C_SEL_FUNCx;
	
typedef struct SI2C_HandleTypeDef
{
	uint8_t 	(*Init)(void);
	void 			(*Delay)(void);
	void 			(*SdaSetOut)(void);
	void 			(*SdaSetIn)(void);
	void 			(*GpioWrite)(SI2C_PINx pin, SI2C_PIN_STATE pinState);
	uint8_t		(*SdaRead)(void);
	uint8_t		(*pSi2cProcess)(struct SI2C_HandleTypeDef *pSi2c, SI2C_SEL_FUNCx selFunc);
	
}SI2C_HandleTypeDef;

/***********************************************
                   variable
***********************************************/

/***********************************************
               function prototypes
***********************************************/
uint8_t simulationI2cInit(SI2C_HandleTypeDef *pSi2c) ;
uint8_t i2cSendByte(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t byte);
void i2cSendBuff(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t *pBuf, uint16_t len);
uint8_t i2cReadByte(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr);
uint8_t i2cReadBuff(SI2C_HandleTypeDef *pSi2c, uint8_t devAddr, uint8_t subAddr, uint8_t *pBuf, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif  /* __SIMULATION_I2C_H__ */

/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



