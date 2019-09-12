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
#include "vast_ir.h"
#include "vast_core/device.h"
#include "vast_core/softtimer.h"
#include "vast_core/init.h"

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
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
static void ir_tmr_cb(soft_timer_t *st)
{
	st->expires += pdMS_TO_TICKS(10);
	soft_timer_add(st);

	if(IR_Obj.state & CAPTURE_STAT_CAPTURE_DONE)
	{
		for(uint16_t i=0; i<IR_Obj.len; i++)
		{
				printf("IR_Obj[%d]: %d, %d, len=%d\r\n", i,
						IR_Obj.rx_buf[i].timer, IR_Obj.rx_buf[i].pin_state, IR_Obj.len);

		}

		InfraRed_RX_Calculate();

		printf("IR_Obj: 0x%04X, 0x%02X, 0x%02X\r\n",
				IR_Obj.value.address, IR_Obj.value.command, IR_Obj.value.command_check);

		IR_Obj.len = 0;
	}
}

/**
  * @brief  vast_ir_bsp_init.
  * @param
  * @retval
  */
static int vast_ir_bsp_init(void)
{
	InfraRed_RX_Init();

	static struct soft_timer ir_tmr =
	{
		.cb = ir_tmr_cb,
		.expires = INITIAL_JIFFIES + pdMS_TO_TICKS(10),
	};

	soft_timer_add(&ir_tmr);

	return 0;
}

device_initcall(vast_ir_bsp_init);

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



