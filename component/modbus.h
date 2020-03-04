//################################################################################
//# Copyright Statement: CVTE
//# Copyright (C) 2014 Guangzhou Shiyuan Electronics Co.,Ltd. All rights reserved.
//#      ____________        _______________  ___________
//#     / / ________ \      / / _____   ____|| |  _______|
//#    / / /      \ \ \    / / /   | | |     | | |
//#   | | |        \ \ \  / / /    | | |     | | |_______
//#   | | |         \ \ \/ / /     | | |     | |  _______|
//#   | | |          \ \ \/ /      | | |     | | |
//#    \ \ \______    \ \  /       | | |     | | |_______
//#     \_\_______|    \_\/        |_|_|     |_|_________|
//#
//################################################################################

/**
 ******************************************************************************
 * @file    demo.h
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-sep-2018
 * @brief   
 ******************************************************************************
 * @attention
 * 
 ******************************************************************************
 */
    
/** @addtogroup Project
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODBUS_H__
#define __MODBUS_H__

/***********************************************
                    include
***********************************************/
#include <stdint.h>
#include <stdbool.h>
#include <core/device.h>
#include "core/croutine.h"

/***********************************************
                    define
***********************************************/
#ifdef configUSING_MODBUS

#define	MODBUS_RTU_BUFF_LEN				0x7F
#define MAX_SLAVE_ADDR					0xFF
#define	MAX_HOLDRES						64

#define READ_HOLD_RES					0x03
#define WRITE_MULTI_HOLD_RES			0x10
#define WRITE_SINGLE_HOLD_RES			0x06
#define READ_INPUT_RES					0x04

/***********************************************
                    typedef
***********************************************/
//struct modbus_msg
//{
	//uint16_t slave_addr;
	//uint16_t opcode;
	//uint16_t start_addr;
	//uint16_t reg_cnt;
	//uint16_t  *buf;
//};

typedef struct modbus_device
{
	device_t *dev;
	uint16_t slave_addr;
	tcb_t tcb;
}modbus_t;

/***********************************************
               function prototypes
***********************************************/
int modbus_init(void);
int readInputReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt);
int writeMultiHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data);
int writeSingleHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_val);
int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt);

/***********************************************
					inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#endif
/**
  * @}
  */

#endif  /* __MODBUS_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



