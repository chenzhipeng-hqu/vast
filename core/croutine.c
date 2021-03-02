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
#include "core/core.h"
#include "core/croutine.h"

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
static LIST_HEAD(task_ready_list);
static LIST_HEAD(task_block_list);

static tcb_t *current = NULL;

/***********************************************
                   function
***********************************************/
/**
  * @brief
  * @param
  * @retval
  */
int16_t task_create(tcb_t *tcb, tcb_cb_t tcb_cb, const char *name, uint32_t data)
{
    tcb->signal  = 0;
    tcb->data   = data;
    tcb->tcb_cb = tcb_cb;

    object_attach(&task_ready_list, &tcb->obj, name);

    return 0;
}

/**
  * @brief
  * @param
  * @retval
  */
void task_destroy(tcb_t *tcb)
{
    object_detach(NULL, &tcb->obj);
}

/**
  * @brief
  * @param
  * @retval
  */
void task_send_signal(tcb_t *tcb, uint32_t sig)
{
    OS_CPU_SR cpu_sr;

    ENTER_CRITICAL();
    tcb->signal |= sig;
    object_detach(NULL, &tcb->obj);
    object_attach(&task_ready_list, &tcb->obj, tcb->obj.name);
    EXIT_CRITICAL();
}

/**
  * @brief
  * @param
  * @retval
  */
void task_moveto_block_list(tcb_t *tcb)
{
    object_detach(NULL, &tcb->obj);
    object_attach(&task_block_list, &tcb->obj, tcb->obj.name);
}

/**
  * @brief
  * @param
  * @retval
  */
uint32_t task_signal(tcb_t *tcb, uint8_t peek)
{
    uint32_t sig;
    OS_CPU_SR cpu_sr;

    ENTER_CRITICAL();
    sig = tcb->signal;

    if (peek == 0) {
        tcb->signal = 0;
        task_moveto_block_list(&tcb->obj);
    }

    EXIT_CRITICAL();
    return sig;
}

/**
  * @brief
  * @param
  * @retval
  */
tcb_t *task_self(void)
{
    return current;
}

/**
  * @brief
  * @param
  * @retval
  */
void task_schedule(void)
{
    object_t *n = NULL, *cur_obj = NULL;

    list_for_each_entry_safe(cur_obj, n, &task_ready_list, entry) {
        current = (tcb_t *)cur_obj;

        uint32_t start = GET_TIME_TICK();

        current->tcb_cb(current, current->data);

        if (GET_TIME_TICK() - start > 10)
            printf("task %s spent %ldms\r\n", current->obj.name, (uint32_t)(GET_TIME_TICK() - start));

    }
}

/**
  * @brief
  * @param
  * @retval
  */
tcb_t *task_find(const char *name)
{
    object_t *obj = NULL;

    obj = object_find(&task_ready_list, name);

    if (obj == NULL) {
        obj = object_find(&task_block_list, name);
    }

    return (tcb_t *)obj;
}



/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/

