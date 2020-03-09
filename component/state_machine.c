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
#include "component/state_machine.h"

#define LOG_TAG    "fsm"
#include <elog.h>

/***********************************************
                    define
***********************************************/
#ifdef configUSING_SM

#define SIGSTACHG               SIG_USR    /* state change */

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
static const state_machine_state_op_t *get_state_op(state_machine_t *sm, ubase_t state)
{
    int i = 0;
    
    state_machine_state_op_t *opt = (state_machine_state_op_t *)sm->states;
    
    for (i = 0; i < sm->states_size; i++, opt++) 
    {
        if (opt->state == state)
        {
            return opt;
        }
    }

    return NULL;
}

/**
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
void state_machine_change(state_machine_t *sm, ubase_t state)
{
    const state_machine_state_op_t *op = get_state_op(sm, state);
    if (op)
    {
        log_d("change state to %s", op->name);

        sm->init = 1;
        if (sm->cur_state != state) 
        {
            sm->cur_state = state;
            sm->trycnt    = 0;
            sm->op        = op;
        }

        task_send_signal(&sm->tcb, SIGSTACHG);
    }
}

/**
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
static void state_machine_timer_handle(struct soft_timer *st)
{
    state_machine_t *sm = (state_machine_t *)st->data;
    task_send_signal(&sm->tcb, SIG_ALARM);
}

/**
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
static void state_machine_schedule(void *arg)
{
    int ret;
    state_machine_t *sm = (state_machine_t *)arg;

redo:
    if (sm->init)
    {
        sm->init = 0;
        ret = sm->op->op0(sm, sm->buffer, sm->size_max);
        if (ret > 0) {
            if (sm->process0) {
            	ret = sm->process0(sm, sm->buffer, ret);
            }
        }
        if (sm->op->wait == 0) goto redo; // now: sm->init == 0

        soft_timer_mod(&sm->timer, jiffies + sm->op->wait);
    }
    else
    {
        ret = sm->op->op1(sm, sm->data);
    }

    if (ret == STATE_CTR_NEXT)
    {
        soft_timer_del(&sm->timer);
        state_machine_change(sm, sm->op->next_state);
        log_d("STATE_CTR_NEXT");
    }
    else if (ret == STATE_CTR_RETRY)
    {
        sm->trycnt++;
        state_machine_change(sm, sm->op->state);
        log_w("STATE_CTR_RETRY:%d", sm->trycnt);
    }
}

/**
  * @brief  key_tmr_cb.
  * @param
  * @retval
  */
static void sm_task_cb(struct task_ctrl_blk *tcb, ubase_t data)
{
    state_machine_t *sm = (state_machine_t *)data;

	tSTART(tcb);

	for(;;) {
        task_wait_signal(tcb);

        sig_t sig = task_signal(tcb);

        /* check state change or time out event */
        if (sigget(sig, SIGSTACHG) || sigget(sig, SIG_ALARM))
        {
            sm->data = NULL;
            state_machine_schedule(sm);
        }
		/* check recvd data event */
        if (sigget(sig, SIG_DATA))
        {
            if (sm->process1) {
                sm->process1(sm);
            }
            state_machine_schedule(sm);
        }

	}
	tEND();
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
int state_machine_init(state_machine_t *sm)
{
	error_t ret = 0;

    sm->timer.cb = state_machine_timer_handle;
    sm->timer.data = (ubase_t)sm;
    sm->data = (void *)sm;

    elog_set_filter_tag_lvl(LOG_TAG, ELOG_LVL_INFO);

	task_create(&sm->tcb, sm_task_cb, (ubase_t)sm);

	return ret;
}

#ifdef configUSING_SM_TEST

#define SIGSTACHG               SIG_USR    /* state change */

//static tcb_t msg_tcb;

typedef enum
{
    STATE_IDLE = 1,
    STATE_GET_DATA,
    STATE_DISPATCH,
    STATE_EXEC,
    STATE_WAIT,
    STATE_RESULT,
} state_machine_state_t;

#pragma pack(1)
typedef struct SmartFrame
{
    uint8_t len;
    uint8_t data[1];
} smart_frame_t;
#define SMART_FRAME_HEAD offset_of(smart_frame_t, data)

static state_machine_t state_machine;
static state_machine_t state_machine2;

#pragma pack()
static int smart_frame_len(const smart_frame_t *frame)
{
    return SMART_FRAME_HEAD + frame->len + 1;
}

/*
 * do idle
 */
static base_t do_get_data0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}
static base_t do_get_data1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);
    if (!arg)    /* time out */
        return STATE_CTR_RETRY;

    struct SmartFrame *pframe = (struct SmartFrame *)arg;

    if (pframe->data[0] == 'n')
        return STATE_CTR_NEXT;

    return STATE_CTR_WAIT;
}

/*
 * do idle
 */
static base_t do_dispatch0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}
static base_t do_dispatch1(state_machine_t *sm, const void *arg)
{
    log_d("%s", __FUNCTION__);
    if (!arg)    /* time out */
        return STATE_CTR_RETRY;

    return STATE_CTR_NEXT;
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
    struct SmartFrame *pframe = (struct SmartFrame *)arg;
    log_d("%s", __FUNCTION__);

    if (pframe->data[0] == 'c')
        state_machine_change(sm, STATE_GET_DATA);

    //if (ret > 0)
    {
        //device_write(serial_plc, 0, arg, ret);
    }
    return STATE_CTR_WAIT;
}

static base_t do_idle20(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
    log_d("%s", __FUNCTION__);
    return 0;
}

static base_t do_idle21(state_machine_t *sm, const void *arg)
{
    struct SmartFrame *pframe = (struct SmartFrame *)arg;
    log_d("%s", __FUNCTION__);

    if (pframe->data[0] == 'c')
        state_machine_change(sm, STATE_GET_DATA);

    //if (ret > 0)
    {
        //device_write(serial_plc, 0, arg, ret);
    }
    return STATE_CTR_WAIT;
}

static const state_machine_state_op_t state_machine_states[] =
{
    state_machine_state(STATE_GET_DATA , STATE_DISPATCH , pdMS_TO_TICKS(2000) , do_get_data)   ,
    state_machine_state(STATE_DISPATCH , STATE_EXEC     , pdMS_TO_TICKS(2000) , do_dispatch)   ,
    state_machine_state(STATE_EXEC     , STATE_WAIT     , pdMS_TO_TICKS(2000) , do_execute)    ,
    state_machine_state(STATE_WAIT     , STATE_RESULT   , pdMS_TO_TICKS(2000) , do_wait_done)  ,
    state_machine_state(STATE_RESULT   , STATE_IDLE     , pdMS_TO_TICKS(2000) , do_get_result) ,
    state_machine_state(STATE_IDLE     , STATE_IDLE     , 0                   , do_idle)       ,
};

static const state_machine_state_op_t state_machine_states2[] =
{
    state_machine_state(STATE_GET_DATA , STATE_RESULT   , pdMS_TO_TICKS(2000) , do_get_data)   ,
    state_machine_state(STATE_RESULT   , STATE_IDLE     , pdMS_TO_TICKS(2000) , do_get_result) ,
    state_machine_state(STATE_IDLE     , STATE_IDLE     , 0                   , do_idle2)       ,
};

//---------------------------------------------------------------------------------------
int is_state_machine_idle(void)
{
    return state_machine.cur_state == STATE_IDLE;
}

static size_t check_smart_frame(const void *arg, size_t len)
{
    state_machine_t *sm = (state_machine_t *)arg;

    smart_frame_t *pframe = get_smart_frame(sm->buffer, len);
    if (!pframe) return 0;
    sm->data = (void *)pframe;

    len = smart_frame_len(pframe);
    log_d("got smart frame[%d]:", len);
    elog_hexdump("prot.rx", 10, (uint8_t *)pframe, len);
    len += ((uint8_t *)pframe - (uint8_t *)sm->data);

    return len;
}

static base_t do_uart_data(const void *arg)
{
    state_machine_t *sm = (state_machine_t *)arg;
    //int len = device_peek(serial_plc, 0, g_frame_buf, sizeof(g_frame_buf));
    int len = 1;
    if (len)
    {
		int ret = check_smart_frame((const void *)sm, len);
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

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static int sm_task_init(void)
{
	error_t ret = 0;

    state_machine.process = do_uart_data;
    state_machine.states = state_machine_states;
    state_machine.states_size = array_size(state_machine_states);
    state_machine_init(&state_machine);
    state_machine_change(&state_machine, STATE_IDLE);

    state_machine2.process = do_uart_data;
    state_machine2.states = state_machine_states2;
    state_machine2.states_size = array_size(state_machine_states2);
    state_machine_init(&state_machine2);
    state_machine_change(&state_machine2, STATE_IDLE);

	return ret;
}
late_initcall(sm_task_init);

/**
 *   * @brief  CLICmd_Print
 *     * @param  
 *       * @retval 
 *         */
void CLICmd_SMachine(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    state_machine_t *sm = &state_machine;

	smart_frame_t *pframe = NULL;
    if(argc == 3) {
        if (!stricmp("2", argv[1])) {
            sm = &state_machine2;
        }
        task_send_signal(&sm->tcb, SIG_DATA);
        pframe = (smart_frame_t *)sm->buffer;
        pframe->len = sprintf((char *)pframe->data, "%s", argv[2]);
    } else {
        pCli->Init.Write("Please input: sm <1|2> <data>.\r\n");
    }
}
CLI_CMD_EXPORT(sm, "state machine test", CLICmd_SMachine, 0);

#endif /*configUSING_SM_TEST*/

/**
  * @}
  */
#endif /*configUSING_SM*/

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




