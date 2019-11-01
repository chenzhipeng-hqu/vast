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
#include <core/softtimer.h>
#include <device/vast_led.h>

/***********************************************
                    define
***********************************************/
#define		LED_NORMAL_FLASH_TIMES		(2U)
#define		LED_FLASH_ON_MS				(configHZ/20U)
#define		LED_FLASH_OFF_MS			(configHZ/10U)
#define		LED_EMCY_FLASH_MS			(configHZ/10U)

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
  * @brief  led_tmr_cb.
  * @param
  * @retval
  */
static void led_tmr_cb(struct soft_timer * st)
{
	led_device_t *led = (led_device_t *)st->data;

	switch(led->mode)
	{
		case CTRL_EMCY:
		{
			st->expires += pdMS_TO_TICKS(100);
			soft_timer_add(st);

			led->ops->toggle(led);
			break;
		}
		case CTRL_NORMAL:
		{
			if(led->flash_cnt == 0)
			{
				led->flash_cnt = LED_NORMAL_FLASH_TIMES;

				st->expires += pdMS_TO_TICKS(configHZ - ((led->flash_cnt) * (LED_FLASH_ON_MS+LED_FLASH_OFF_MS)));
				soft_timer_add(st);
			}
			else if(led->status == 0)
			{
				st->expires += pdMS_TO_TICKS(LED_FLASH_ON_MS);	//10ms
				soft_timer_add(st);
				led->status = 1;
				led->ops->on(led);
			}
			else if(led->status == 1)
			{
				st->expires += pdMS_TO_TICKS(LED_FLASH_OFF_MS);
				soft_timer_add(st);
				led->flash_cnt--;
				led->status = 0;
				led->ops->off(led);
			}
			break;
		}
		default:
			break;
	}
}

/**
  * @brief  led_init.
  * @param
  * @retval
  */
static error_t led_init(device_t *dev)
{
	led_device_t *led = (led_device_t *)dev;
	struct soft_timer *st = &led->tmr;

	st->cb = led_tmr_cb;
	st->data = (ubase_t)led;
	st->expires = INITIAL_JIFFIES;

	return 0;
}

/**
  * @brief  led_ctrl.
  * @param
  * @retval
  */
static error_t led_ctrl(device_t *dev, uint8_t cmd, void *args)
{
	led_device_t *led = (led_device_t *)dev;

	struct soft_timer *st = &led->tmr;

	led->mode = cmd;

	switch (cmd)
	{
		case CTRL_ON:
		{
			led->ops->on(led);
			soft_timer_del(st);
			break;
		}
		case CTRL_OFF:
		{
			led->ops->off(led);
			soft_timer_del(st);
			break;
		}
		case CTRL_EMCY:
		case CTRL_NORMAL:
		{
			led->flash_cnt = LED_NORMAL_FLASH_TIMES;
			st->expires = INITIAL_JIFFIES + pdMS_TO_TICKS(0);
			soft_timer_add(st);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

/**
  * @brief  led_ops.
  * @param
  * @retval
  */
static struct device_ops led_ops =
{
	.init = led_init,
	.ctrl = led_ctrl,
};

error_t led_device_register(led_device_t *led, const char *name, uint32_t flag, void *data)
{
	device_t *dev = &(led->parent);

	dev->ops 		= &led_ops;
	dev->user_data	= data;

	return device_register(dev, name, flag);
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



