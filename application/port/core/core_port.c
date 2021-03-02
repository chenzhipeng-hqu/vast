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
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/init.h"
#include "core/core.h"

/***********************************************
                    define
***********************************************/
#define LOG_TAG    "core.port"
#include <elog.h>


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
void sig_alm_handler(int sig_num)
{
    if (sig_num = SIGALRM) {
        core_tick_inc();
    }
}

/*typedef void (*__sighandler_t) (int);*/

static int16_t core_tick_init(core_t *core)
{
    //拦截定时器信号。
    __sighandler_t *pre = signal(SIGALRM, sig_alm_handler);
    assert(!pre);
    //printf("pre-sighandler address:%p\n", pre); //pre应该是NULL.

    struct itimerval olditv;
    struct itimerval itv;
    itv.it_interval.tv_sec = 0; //定时周期为1ms。
    itv.it_interval.tv_usec = 1000;
    itv.it_value.tv_sec = 0; //定时器启动以后将在0秒又100微秒以后正式开始计时。
    itv.it_value.tv_usec = 100;
    setitimer(ITIMER_REAL, &itv, &olditv);

    return 0;
}
/**
  * @brief
  * @param
  * @retval
  */
static OS_CPU_SR enter_critical(void)
{
    OS_CPU_SR  cpu_sr = 0;

    return cpu_sr;
}

/**
  * @brief
  * @param
  * @retval
  */
static void exit_critical(OS_CPU_SR cpu_sr)
{
    cpu_sr = cpu_sr;
}

/**
  * @brief
  * @param
  * @retval
  */
//uint32_t get_tick_ops(void)
//{
//return 0;
//}

/**
  * @brief
  * @param
  * @retval
  */
static core_ops_t ops = {
    .enter_critical = enter_critical,
    .exit_critical = exit_critical,
    .tick_init = core_tick_init,
    //.get_tick = get_tick_ops,
};

static int core_init_port(void)
{
    core_init(&ops);

    return 0;
}
postcore_initcall(core_init_port);

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




