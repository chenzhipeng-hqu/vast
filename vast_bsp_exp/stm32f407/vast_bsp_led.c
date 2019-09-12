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
#include "vast_device/vast_led.h"
#include "vast_driver/vast_log.h"
#include "gpio.h"

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
static led_device_t ledr_dev, ledg_dev;

static const gpio_list_t ledr_gpio = {LED_R_GPIO_Port, LED_R_Pin, 1};
static const gpio_list_t ledg_gpio = {LED_G_GPIO_Port, LED_G_Pin, 1};

/***********************************************
                   function
***********************************************/
/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static void led_gpio_init(void)
{
	LL_GPIO_SetOutputPin(LED_R_GPIO_Port, LED_R_Pin);
    return;
}

/**
  * @brief  led_set_bsp.
  * @param
  * @retval
  */
static err_t led_on_bsp(led_device_t *dev)
{
	gpio_list_t *gpio = (gpio_list_t *)(dev->parent.user_data);

	if(gpio->_default == 0)
	{
		LL_GPIO_SetOutputPin(gpio->GPIOx, gpio->PinMask);
	}
	else
	{
		LL_GPIO_ResetOutputPin(gpio->GPIOx, gpio->PinMask);
	}

    return 0;
}

/**
  * @brief  led_reset_bsp.
  * @param
  * @retval
  */
static err_t led_off_bsp(led_device_t *dev)
{
	gpio_list_t *gpio = (gpio_list_t *)(dev->parent.user_data);

	if(gpio->_default == 0)
	{
		LL_GPIO_ResetOutputPin(gpio->GPIOx, gpio->PinMask);
	}
	else
	{
		LL_GPIO_SetOutputPin(gpio->GPIOx, gpio->PinMask);
	}

    return 0;
}

/**
  * @brief  led_toggle_bsp.
  * @param
  * @retval
  */
static err_t led_toggle_bsp(led_device_t *dev)
{
	gpio_list_t *gpio = (gpio_list_t *)(dev->parent.user_data);
	LL_GPIO_TogglePin(gpio->GPIOx, gpio->PinMask);

    return 0;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static const led_ops_t led_ops =
{
	.on 	= led_on_bsp,
	.off 	= led_off_bsp,
	.toggle	= led_toggle_bsp,
};

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static int vast_led_bsp_init(void)
{
	err_t ret = 0;

	led_gpio_init();

	ledr_dev.ops = &led_ops;
	ret = led_device_register(&ledr_dev, "ledr", 0, (void *)&ledr_gpio);

	ASSERT(ret == VAST_OK,
			printf("ret=%d\r\n", ret);
			return ret
			);

	device_open(&ledr_dev.parent, 0);
	device_ctrl(&ledr_dev.parent, CTRL_NORMAL, NULL);
	device_close(&ledr_dev.parent);

	ledg_dev.ops = &led_ops;
	ret = led_device_register(&ledg_dev, "ledg", 0, (void *)&ledg_gpio);

	ASSERT(ret == VAST_OK,
			printf("ret=%d\r\n", ret);
			return ret
			);

	device_open(&ledg_dev.parent, 0);
	device_ctrl(&ledg_dev.parent, CTRL_EMCY, NULL);
	device_close(&ledg_dev.parent);

//	printf("led_device_t size=%d\r\n", sizeof(led_device_t));

	return 0;
}

device_initcall(vast_led_bsp_init);
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



