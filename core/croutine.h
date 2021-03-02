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
#ifndef __CO_ROUTINE_H__
#define __CO_ROUTINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "core/object.h"

/***********************************************
                    define
***********************************************/
#define tSTART(tcb) switch((tcb)->signal)       {case 0: task_moveto_block_list(tcb);
#define tEND(tcb)   task_destroy(tcb);}

#define task_wait_signal(tcb)                               \
     {                                                      \
         return; default:;                                  \
     }

#define sigget(signal, sig) ((signal) &  (sig))

/***********************************************
                    typedef
***********************************************/
enum {
    SIG_ALARM = 1 << 0,
    SIG_DATA  = 1 << 1,
    SIG_EVT   = 1 << 2,
    SIG_USR   = 1 << 3,
};

typedef uint32_t        sig_t;

typedef void (*tcb_cb_t)(struct task_ctrl_blk *, uint32_t);

typedef struct task_ctrl_blk {
    struct object       obj;
    tcb_cb_t            tcb_cb;
    uint32_t            data;
    uint32_t            event;
    uint32_t            signal;
} tcb_t;

/***********************************************
               function prototypes
***********************************************/
int16_t     task_create     ( tcb_t *tcb, tcb_cb_t tcb_cb, const char *name, uint32_t data);
void        task_destroy    ( tcb_t *tcb);
tcb_t       *task_self      ( void);
void        task_schedule   ( void);
uint32_t    task_signal     ( tcb_t *tcb, uint8_t peek);
void        task_send_signal( tcb_t *tcb, uint32_t sig);
tcb_t       *task_find      ( const char *name);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TEMPLE_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

