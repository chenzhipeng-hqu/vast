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
#include "vast_core/softtimer.h"
#include "gpio.h"
#include "vast_core/init.h"
#include "vast_easy_key.h"

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
EasyKey_HandleTypeDef	hkey_start;
static key_device_t key_dev;

/***********************************************
                   function
***********************************************/
/**
  * @brief  KeyProcess_Start.
  * @param
  * @retval
  */
uint8_t KeyProcess_Start (EasyKey_HandleTypeDef *pKey, EASYKEY_SEL_FUNCx selFunc)
{
	uint8_t ret = 0;

	switch(selFunc)
	{
		case EASYKEY_FUNC_INIT :
		{
			GPIO_InitTypeDef  GPIO_InitStruct;

			GPIO_InitStruct.Pin       = GPIO_PIN_14;
			GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull      = GPIO_PULLUP;
			GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
			HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

			break;
		}
		case EASYKEY_FUNC_READ :
		{
			ret = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14);
			break;
		}
		case EASYKEY_FUNC_CLICK_ONCE :
		{
			printf("%s click once, t:%ld, %d\r\n", __FUNCTION__, HAL_GetTick(), pKey->Key_Time);
			break;
		}
		case EASYKEY_FUNC_CLICK_DOUB :
		{
			printf("%s click double, t:%ld, %d, %d, %d, %d, %d, %d\r\n"	,
																	__FUNCTION__			,
																	HAL_GetTick()	,
																	pKey->Key_Time		,
																	sizeof(KEY_StateTypeDef),
																	sizeof(KEY_SMachineTypeDef),
																	sizeof(uint8_t),
																	sizeof(uint32_t),
																	sizeof(EasyKey_HandleTypeDef)
																);
			break;
		}
		case EASYKEY_FUNC_PRESS_LONG :
		{
			printf("%s press long, t:%ld, %d\r\n", __FUNCTION__, HAL_GetTick(), pKey->Key_Time);
			break;
		}
		case EASYKEY_FUNC_CONTINOUS :
		{
			printf("%s continous, t:%ld, %d\r\n", __FUNCTION__, HAL_GetTick(), pKey->Key_Time);
			break;
		}
		default:
		{
			printf("%s %d, err key func!\r\n", __FUNCTION__, __LINE__);
			break;
		}
	}

	return ret;
}

/**
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
static void key_tmr_cb(soft_timer_t *st)
{
	st->expires += pdMS_TO_TICKS(10);
	soft_timer_add(st);

	EasyKey_Scan(&hkey_start);
}

/**
  * @brief  vast_easy_key_bsp_init.
  * @param
  * @retval
  */
static int vast_easy_key_bsp_init(void)
{
	hkey_start.pKeyProcess = KeyProcess_Start;
	hkey_start.Key_ContinousTirg = true;
	EasyKey_Init(&hkey_start);

	static struct soft_timer key_tmr =
	{
		.cb = key_tmr_cb,
		.expires = INITIAL_JIFFIES + pdMS_TO_TICKS(10),
	};

	soft_timer_add(&key_tmr);

	//key_dev.ops = &vast_key_ops;
	key_device_register(&key_dev, "key", 0, NULL);

	return 0;
}

device_initcall(vast_easy_key_bsp_init);

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



