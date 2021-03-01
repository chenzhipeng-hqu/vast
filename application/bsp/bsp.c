/*****************************************************************************
* Product: "DPP" example on STM32F4-Discovery board, cooperative QV kernel
* Last Updated for Version: 6.5.0
* Date of the Last Update:  2019-05-09
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/init.h"

#include "bsp.h"
//#include <core/softtimer.h>
//#include <core/croutine.h>

#define LOG_TAG    "bsp"
#include <elog.h>

/* add other drivers if necessary... */

/*MAJOR*/
/*MINOR*/
/*REVISION*/
const char SW_VERSION[] = "0.0.1";
const char HW_VERSION[] = "LINUX";
const char MANUFACTURER[] = "CVTE";
const char buildDate[] = __DATE__;
const char buildTime[] = __TIME__;

/* Local-scope defines -----------------------------------------------------*/

/**
 * @brief configures IWDG and enable watchdog.
 * @param None
 * @retval None
 */
static int startCheck(void)
{
//	int ret = 0;
  printf("[%s %s]\r\n", buildDate, buildTime);
  printf("SW_VERSION:%s\r\n", SW_VERSION);
  printf("HW_VERSION:%s\r\n", HW_VERSION);
  //printf("manufacturer:%s\r\n", MANUFACTURER);
  //printf("UID:0x%08lx %08lx %08lx\r\n", *(__IO uint32_t *)(UID_BASE+8), *(uint32_t *)(UID_BASE+4), *(uint32_t *)(UID_BASE+0));
  //printf("FLASHSIZE_BASE:%dKB\r\n", *(uint16_t *)FLASHSIZE_BASE);
  //printf("PACKAGE_BASE:%d\r\n", *(uint8_t *)PACKAGE_BASE);
  //printf("bss_start:%p, bss_end:%p, size:%ld\r\n", &__bss_start__, &__bss_end__, (long)&__bss_end__ - (long)&__bss_start__);
  //printf("heap_start:0x%08lx, heap_end:0x%08lx, size:%ld\r\n", HEAP_BEGIN, HEAP_END, HEAP_END - HEAP_BEGIN);
  //printf("sstack:%p, estack:%p, size:%ld\r\n", &_sstack, &_estack, (long)&_estack - (long)&_sstack);

	return 0;
}
//postcore_initcall(startCheck);

static int counter = 0;
/**
 * @brief configures IWDG and enable watchdog.
 * @param None
 * @retval None
 */
void sig_alm_handler(int sig_num)
{
    //printf("%s, signal number:%d, counter:%d\n", __FUNCTION__, sig_num, counter);
    if(sig_num = SIGALRM)
    {
        counter++;
    }
}

/*typedef void (*__sighandler_t) (int);*/

static int core_tick_init(void)
{
     //拦截定时器信号。
    __sighandler_t *pre = signal(SIGALRM, sig_alm_handler);
    assert(!pre);
    //printf("pre-sighandler address:%p\n", pre); //pre应该是NULL.

    struct itimerval olditv;
    struct itimerval itv;
    itv.it_interval.tv_sec = 0; //定时周期为1ms。
    itv.it_interval.tv_usec = 1000;
    itv.it_value.tv_sec = 1; //定时器启动以后将在1秒又100微秒以后正式开始计时。
    itv.it_value.tv_usec = 100;
    // setitimer(ITIMER_REAL, &itv, &olditv);

    return 0;
}

/**
 * @brief configures IWDG and enable watchdog.
 * @param None
 * @retval None
 */
static int bsp_init(void)
{
    //vast_uart_bsp_init();
    core_tick_init();

    startCheck();

    return 0;
}
core_initcall(bsp_init);

#if (defined configUSING_CLI || defined VAST_USING_SHELL)
/**
  * @brief  CLICmd_DebugCtrl
  * @param
  * @retval
  */
void CLICmd_Version(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  printf("HCLK=%ldMHz, Tick=%ldms, [%s %s]\r\n", HAL_RCC_GetHCLKFreq()/1000000, HAL_GetTick(), buildDate, buildTime);
  printf("SW_VERSION:%s\r\n", SW_VERSION);
  printf("HW_VERSION:%s\r\n", HW_VERSION);
  printf("manufacturer:%s\r\n", MANUFACTURER);
}

CLI_CMD_EXPORT(version, "print version", CLICmd_Version, 0);

#endif
