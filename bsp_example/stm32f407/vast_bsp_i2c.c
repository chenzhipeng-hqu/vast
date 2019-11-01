/**
 ******************************************************************************
 * @file    xxx.c
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
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
#include "vast_bsp.h"
#include "vast_core/device.h"
#include "vast_core/init.h"
#include "vast_driver/vast_log.h"
#include "gpio.h"
#include "vast_device/ina219.h"

/***********************************************
                    define
***********************************************/
#define GPIO_PORT_I2C1_SCL  GPIOB
#define PIN_I2C1_SCL      	LL_GPIO_PIN_8

#define GPIO_PORT_I2C1_SDA  GPIOB
#define PIN_I2C1_SDA      	LL_GPIO_PIN_9

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
static struct i2c_bus_device bus_i2c1;

/***********************************************
                   function
***********************************************/
/**
  * @brief  i2c_gpio_init.
  * @param
  * @retval
  */
static void i2c_gpio_init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	LL_GPIO_SetOutputPin(GPIO_PORT_I2C1_SCL, PIN_I2C1_SCL);
	LL_GPIO_SetOutputPin(GPIO_PORT_I2C1_SDA, PIN_I2C1_SDA);

	GPIO_InitStruct.Pin = PIN_I2C1_SCL | PIN_I2C1_SDA;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;   //import
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_PORT_I2C1_SCL, &GPIO_InitStruct);

    return;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static void i2c_udelay(unsigned long us)
{
	uint16_t i, j;

	for (j=0; j<us; j++)
	{
		for (i=0; i< 0xf; i++);
	}

    return;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static void i2c1_set_sda(void *data, int32_t state)
{
//	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//	GPIO_InitStruct.Pin = PIN_I2C1_SDA;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	LL_GPIO_Init(GPIO_PORT_I2C1_SDA, &GPIO_InitStruct);

    if (state)
    {
    	LL_GPIO_SetOutputPin(GPIO_PORT_I2C1_SDA, PIN_I2C1_SDA);
    }
    else
    {
    	LL_GPIO_ResetOutputPin(GPIO_PORT_I2C1_SDA, PIN_I2C1_SDA);
    }
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static void i2c1_set_scl(void *data, int32_t state)
{
//	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//	GPIO_InitStruct.Pin = PIN_I2C1_SCL;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	LL_GPIO_Init(GPIO_PORT_I2C1_SCL, &GPIO_InitStruct);

    if (state == 1)
    {
        LL_GPIO_SetOutputPin(GPIO_PORT_I2C1_SCL, PIN_I2C1_SCL);
    }
    else if (state == 0)
    {
    	LL_GPIO_ResetOutputPin(GPIO_PORT_I2C1_SCL, PIN_I2C1_SCL);
    }
}

/**
  * @brief  i2c1_get_sda.
  * @param
  * @retval
  */
static int32_t i2c1_get_sda(void *data)
{
//	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//	GPIO_InitStruct.Pin = PIN_I2C1_SDA;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
//	LL_GPIO_Init(GPIO_PORT_I2C1_SDA, &GPIO_InitStruct);

//	i2c_udelay(100);

    return (int32_t) LL_GPIO_ReadInputPort(GPIO_PORT_I2C1_SDA) & PIN_I2C1_SDA;
}

/**
  * @brief  i2c1_get_scl.
  * @param
  * @retval
  */
static int32_t i2c1_get_scl(void *data)
{
//	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//	GPIO_InitStruct.Pin = PIN_I2C1_SCL;
//	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
//	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
//	LL_GPIO_Init(GPIO_PORT_I2C1_SCL, &GPIO_InitStruct);

    return (int32_t)LL_GPIO_ReadInputPort(GPIO_PORT_I2C1_SCL) & PIN_I2C1_SCL;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static const struct i2c_bit_ops i2c1_bit_ops =
{
    (void *)0xaa,
    i2c1_set_sda,
    i2c1_set_scl,
    i2c1_get_sda,
    i2c1_get_scl,
	i2c_udelay,
    100,
    5
};

/**
  * @brief  vast_i2c_bsp_init.
  * @param
  * @retval
  */
static int vast_i2c_bsp_init(void)
{
	error_t ret = 0;

	i2c_gpio_init();

    bus_i2c1.priv = (void *)&i2c1_bit_ops;
    ret = i2c_bit_add_bus(&bus_i2c1, "i2c");

	return ret;
}

subsys_initcall(vast_i2c_bsp_init);
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



