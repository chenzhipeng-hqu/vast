/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include <core/softtimer.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "string.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;

/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
#define MB_USE_SOFT_TIMER

#ifdef MB_USE_SOFT_TIMER
static struct soft_timer modbus_tmr = {0};

static void modbus_tmr_cb(struct soft_timer * st)
{
    #if MB_MASTER_RTU_ENABLED > 0
        pxMBMasterPortCBTimerExpired();
    #else
        pxMBPortCBTimerExpired();
    #endif
}

#else
static void TIM_Period_Reload(TIM_HandleTypeDef *htim, uint32_t period)
{
	uint32_t tmpcr1 = htim->Instance->CR1;
	
	tmpcr1 &= ~TIM_CR1_ARPE;
	
	htim->Instance->CR1 = tmpcr1;
	
	htim->Instance->ARR = period;
	
	//HAL_TIM_GenerateEvent(htim, TIM_EventSource_Update);	
}
#endif

BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
	
#ifdef MB_USE_SOFT_TIMER
    usT35TimeOut50us = usTimeOut50us/10;
    modbus_tmr.cb = modbus_tmr_cb;
#else
    usT35TimeOut50us = usTimeOut50us;
  htim6.Init.Period = usT35TimeOut50us;
	
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }

	__HAL_TIM_URS_ENABLE(&htim6);
#endif
    printf("usT35TimeOut50us:%d\r\n", usT35TimeOut50us);
	
  return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
#ifdef MB_USE_SOFT_TIMER
    soft_timer_mod(&modbus_tmr, jiffies + usT35TimeOut50us);
#else
	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_T35);
	
	HAL_TIM_Base_Stop_IT(&htim6);
	
	TIM_Period_Reload(&htim6, usT35TimeOut50us);
	
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	
	HAL_TIM_Base_Start_IT(&htim6);
#endif
	//printf("tmr_T35:%ld\r\n", HAL_GetTick());
}

void vMBMasterPortTimersConvertDelayEnable()
{
	
#ifdef MB_USE_SOFT_TIMER
    soft_timer_mod(&modbus_tmr, jiffies + MB_MASTER_DELAY_MS_CONVERT);
#else
	uint32_t period;
	
  period = MB_MASTER_DELAY_MS_CONVERT * 1000/50;	
	if(period > 65534) {
		period = 65534;
    }

	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
	
	HAL_TIM_Base_Stop_IT(&htim6);
	
	TIM_Period_Reload	(&htim6, period);
	
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	
	__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
	
	HAL_TIM_Base_Start_IT(&htim6);	
#endif
	//printf("tmr_delay:%ld\r\n", HAL_GetTick());
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
#ifdef MB_USE_SOFT_TIMER
    soft_timer_mod(&modbus_tmr, jiffies + MB_MASTER_TIMEOUT_MS_RESPOND);
#else
	uint32_t period;
	
	period = MB_MASTER_TIMEOUT_MS_RESPOND * 1000/50;
	if(period > 65534) {
		period = 65534;
	}

	/* Set current timer mode, don't change it.*/
	vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
	
	HAL_TIM_Base_Stop_IT(&htim6);	
	
	TIM_Period_Reload	(&htim6, period);
	
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	
	__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
	
	HAL_TIM_Base_Start_IT(&htim6);
	
	//uint32_t tickstart = HAL_GetTick();
	//printf("===tickstart1 = %d, arr = %d, cnt = %d\r\n", tickstart, htim6.Instance->ARR, htim6.Instance->CNT);
#endif
	//printf("tmr_responed:%ld\r\n", HAL_GetTick());
}

void vMBMasterPortTimersDisable()
{
#ifdef MB_USE_SOFT_TIMER
    soft_timer_del(&modbus_tmr);
#else
	HAL_TIM_Base_Stop_IT(&htim6);
#endif
	//printf("tmr_del:%ld\r\n", HAL_GetTick());
}

#endif
