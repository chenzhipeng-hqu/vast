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
#include "component/modbus.h"
#include "component/state_machine.h"

#define LOG_TAG    "modbus"
#include <elog.h>

/***********************************************
                    define
***********************************************/
#ifdef configUSING_MODBUS

/***********************************************
                    typedef
***********************************************/
enum
{
    STATE_IDLE = 1,
    STATE_EXEC,
    STATE_WAIT,
    STATE_RESULT,
};

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
  * @brief  vast_func.
  * @param
  * @retval
  */
int readInputReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt)
{
    int ret = 0;
    state_machine_change(&sm_modbus, STATE_EXEC);
    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeMultiHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data)
{
    int ret = 0;
    state_machine_change(&sm_modbus, STATE_EXEC);
    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeSingleHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_val)
{
    int ret = 0;
    state_machine_change(&sm_modbus, STATE_EXEC);
    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt)
{
    int ret = 0;
    state_machine_change(&sm_modbus, STATE_EXEC);
    return ret;
}

/*
 * do idle
 */
static base_t do_execute0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}
static base_t do_execute1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);
    if (!arg)    /* time out */
        return STATE_CTR_RETRY;

    return STATE_CTR_NEXT;
}

/*
 * do idle
 */
static base_t do_wait_done0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}
static base_t do_wait_done1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);
    if (!arg)    /* time out */
        return STATE_CTR_RETRY;

    return STATE_CTR_NEXT;
}

/*
 * do idle
 */
static base_t do_get_result0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}
static base_t do_get_result1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);
    if (!arg)    /* time out */
        return STATE_CTR_RETRY;

    return STATE_CTR_NEXT;
}

static base_t do_idle0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}

static base_t do_idle1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);

    if (arg)    /* time out */
        return STATE_CTR_NEXT;
    //if (ret > 0)
    {
        //device_write(serial_plc, 0, arg, ret);
    }
    return STATE_CTR_WAIT;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
static const state_machine_state_op_t state_machine_states[] =
{
    state_machine_state(STATE_EXEC     , STATE_WAIT     , pdMS_TO_TICKS(2000) , do_execute)    ,
    state_machine_state(STATE_WAIT     , STATE_RESULT   , pdMS_TO_TICKS(2000) , do_wait_done)  ,
    state_machine_state(STATE_RESULT   , STATE_IDLE     , pdMS_TO_TICKS(2000) , do_get_result) ,
    state_machine_state(STATE_IDLE     , STATE_EXEC     , 0                   , do_idle)       ,
};

static base_t do_uart_data(const void *arg)
{
    state_machine_t *sm = (state_machine_t *)arg;
    //int len = device_peek(serial_plc, 0, g_frame_buf, sizeof(g_frame_buf));
    int len = 1;
    if (len)
    {
		//int ret = check_smart_frame((const void *)sm, len);
        
        sm->data = (void *)sm->buffer;
        int ret = 1;
		if (ret)
		{
			//device_read(serial_plc, 0, g_frame_buf, ret);
			if (len > ret)
				task_send_signal(&sm->tcb, SIG_DATA);
		}
		else
		{
			//if (len > 0xFF)
				//device_read(serial_plc, 0, g_frame_buf, len);
		}
    }

    return 0;
}

static state_machine_t sm_modbus;
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int modbus_init(void)
{
    sm_modbus.process = do_uart_data;
    sm_modbus.states = state_machine_states;
    sm_modbus.states_size = array_size(state_machine_states);
    state_machine_init(&sm_modbus);
    state_machine_change(&sm_modbus, STATE_IDLE);

    return 0;
}
late_initcall(modbus_init);

/**
 *   * @brief  CLICmd_Print
 *     * @param  
 *       * @retval 
 *         */
void CLICmd_SMmodbus(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    state_machine_t *sm = &sm_modbus;

    if(argc == 2) {
        task_send_signal(&sm->tcb, SIG_DATA);
        sprintf((char *)sm->buffer, "%s", argv[2]);
    } else {
        pCli->Init.Write("Please input: smmb <data>.\r\n");
    }
}
CLI_CMD_EXPORT(smmb, "state machine modbus test", CLICmd_SMmodbus, 0);

/**
  * @}
  */
#endif /*configUSING_xxx*/

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




