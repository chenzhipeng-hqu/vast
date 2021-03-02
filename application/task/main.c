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
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "core/init.h"
#include "core/core.h"
#include "core/croutine.h"
#include "core/softimer.h"

/***********************************************
                    define
***********************************************/
#define LOG_TAG    "main"
#include "elog.h"

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
  * @brief
  * @param
  * @retval
  */
int main(int argc, char *argv[])
{
    printf("welcome to use vanca test.\r\n");

    do_init_call();

    log_w("1234");

    while (1) {
        task_schedule();
        softimer_schedule();
    }

    return 0;
}

/**
  * @brief  test_tmr_cb.
  * @param
  * @retval
  */
static void test_tmr_cb(struct softimer * st)
{
    tcb_t *test_tcb = task_find("test");
    task_send_signal(test_tcb, SIG_USR);

	st->expires = GET_TIME_TICK() + 1000;
	softimer_add(st);
}

static struct softimer test_tmr =
{
    .obj = { .name = "test" },
	.expires = 1000,
	.cb = test_tmr_cb,
};

/**
  * @brief 
  * @param
  * @retval
  */
static void test_task_cb(tcb_t *tcb, uint32_t data)
{
    tSTART(tcb);
    log_i("init");

    task_send_signal(tcb, SIG_ALARM);
	softimer_add(&test_tmr);

    for (;;) {
        task_wait_signal(tcb);
        sig_t sig = task_signal(tcb, 0);

        if (sigget(sig, SIG_ALARM)) {
            log_i("alarm");
        }

        if (sigget(sig, SIG_USR)) {
            log_i("user");
        }
    }
    log_i("end");

    tEND(tcb);
}

/**
  * @brief
  * @param
  * @retval
  */
static int task_test_init(void)
{
    int ret = 0;
    static tcb_t tcb_test = {0};

    task_create(&tcb_test, test_task_cb, "test", (uint32_t)0);

    return ret;
}
late_initcall(task_test_init);

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




