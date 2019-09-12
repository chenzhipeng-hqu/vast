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

#ifdef configUSING_CLI
//------------------------------------------------------------------------------
//-   custom command functions. ir
//------------------------------------------------------------------------------
void cliCmdIrSetup(CLI_HandleTypeDef *pCli,int argc, char *argv[])
{
  uint8_t protocal;

  if(argc > 0)
  {
    protocal = str2u32(argv[1]);
    InfraRed_RX_ChangeProtocol(protocal);
    pCli->Init.Write("set ir %d\n", protocal);
  }
}

void cliCmdIrSend(CLI_HandleTypeDef *pCli,int argc, char *argv[])
{
  uint16_t head;
  uint16_t code;
  uint16_t check;

  if(argc > 3)
  {
    head = hex2u32(argv[1]);
    code = hex2u32(argv[2]);
    check = hex2u32(argv[3]);
   // io_irSend(head, code, check);
    pCli->Init.Write("send ir 0x%04X, 0x%X, 0x%X\n", head, code, check);
  }
}

/**
  * @brief  CLICmd_IRCtrl
  * @param
  * @retval
  */
const CLICmdTypedef CLICmd_IRCtrl[] =
{
  {"s", "s protocal(1:NEC/2:RC5/3:RC6/4:RCA/7:SONY/11:MI/14:SAMSUNG/15:PANASONIC/16:TOSHIBA) Exp:s 1", cliCmdIrSetup, 0},
//  {"t", "t head code check", cliCmdIrSend, 0},
	// last command must be all 0s.
  {0, 0, 0, 0}
};

CLI_CMD_EXPORT(ir, "ir dir", 0, CLICmd_IRCtrl);
#endif

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



