/**
 ******************************************************************************
 * @file    xxx.h
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

/* Define to prevent recursive inclusion */
#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include <core/init.h>
#include "core/croutine.h"
#include "core/device.h"

/***********************************************
                    define
***********************************************/
#define state_machine_state(state, next_state, wait, handler) \
                            {state, next_state, #state, wait, handler##0, handler##1}

/***********************************************
                    typedef
***********************************************/
enum
{
    STATE_CTR_NEXT  = -1,     /* change to next state */
    STATE_CTR_THIRD = -2,     /* change to next state */
    STATE_CTR_WAIT  = -3,     /* wait for data event */
    STATE_CTR_RETRY = -4,     /* retry current state */
    STATE_CTR_CHAGE = -5,     /* change to other state */
    STATE_CTR_ERROR = -6,     /* error state */
};

//---------------------------------------------------------------------------------------
struct _state_machine;
typedef struct sm_sta_op
{
    ubase_t state, next_state;
    const char *name;
    time_t wait;
    base_t (*op0)(struct _state_machine *, uint8_t *out, size_t maxlen); //进入状态后执行的动作
    base_t (*op1)(struct _state_machine *, const void *arg);             //该状态收到消息后的动作
} state_machine_state_op_t;

typedef struct _state_machine
{
    uint8_t cur_state, trycnt;
    uint8_t init;
    struct soft_timer timer;
    tcb_t tcb;
    void *tx_data;
    size_t size_max;
    void *rx_data;
    const state_machine_state_op_t *op;
    const state_machine_state_op_t *states;
    uint8_t states_size;
    base_t (*process0)(const void *arg, uint8_t *out, size_t len);
    base_t (*process1)(const void *arg);
} state_machine_t;

/***********************************************
               function prototypes
***********************************************/
extern int state_machine_init(state_machine_t *sm);
extern void state_machine_change(state_machine_t *sm, ubase_t state);

/***********************************************
	      		    inline
***********************************************/
static inline int is_sm_state(state_machine_t *sm, int state)
{
    return sm->cur_state == state;
}

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __VAST_TEMPLE_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



